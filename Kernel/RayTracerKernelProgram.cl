#include "Kernel/GObjectStruct.cl"
#include "Kernel/MaterialStruct.cl"
#include "Kernel/RayCast.cl"
#include "Kernel/TextureMap.cl"
#include "Kernel/Transmission.cl"
#include "Kernel/Illuminate.cl"

#define NUMPIXELS 64

size_t ReduceIntersection(__private size_t index1, __private size_t index2, __private float val1, __private float val2)
{
	__private bool wantMax = ((val1 <= 0.0f) || (val2 <= 0.0f));
	__private bool wantFirst = (wantMax && (val1 > val2)) || ((!wantMax) && (val1 < val2));
	return wantFirst * index1 + (!wantFirst) * index2;
}

//;Here be dragons	
__kernel void RayTrace
(
	write_only image2d_t localTexture, write_only image2d_t reflectionTexture, 
	write_only image2d_t transmissionTexture, write_only image2d_t globalMaterialTexture,
	read_only image2d_t positionTexture, read_only image2d_t diffuseTexture,
	read_only image2d_t normalTexture, read_only image2d_t materialTexture, 
	read_only image2d_array_t textureArray,
	__constant float4* spheres, __constant float* aabbs,
	__constant struct GObject* gObjects, __constant struct Material* materials,
	__constant float3* cameraPosition, __constant float16* transformations,
	__constant float3* lightColor, __constant float3* lightDirection, __constant float2* lightIntensity,
	__local float* valReduce, __local size_t* indexReduce
)
{
	//;Get thread information
	__private size_t idX, idY;
	__private size_t localX, localY, localID;
	__private size_t depth;
	__private size_t localWidth, localHeight;
	__private size_t pixelReduceOffset;
	__private size_t shadowReduceOffset, reflectionReduceOffset, transmissionReduceOffset;

	__private int2 threadCoord;
	idX = get_global_id(0);
	idY = get_global_id(1);

	depth = get_global_size(2);
	shadowReduceOffset = 0 * depth;
	reflectionReduceOffset = 1 * depth;
	transmissionReduceOffset = 2 * depth;

	localWidth = get_local_size(0);
	localHeight = get_local_size(1);

	localX = get_local_id(0);
	localY = get_local_id(1);
	localID = get_local_id(2);

	__private size_t localPixelID = localY * localWidth + localX;

	pixelReduceOffset = depth * 3 * (localPixelID);


	threadCoord = (int2)(idX, idY);


	//;Determine the data this thread will operate on
	__private struct GObject threadObj;
	__private struct Material threadMaterial;
	__private bool isAABB, isSphere;
	__private size_t threadSphereID, threadAABBID;
	__private float4 threadSphere;
	__private float3 threadAABBMin, threadAABBMax;
	__private float16 threadSphereTransform;


	threadObj = gObjects[localID];
	threadMaterial = materials[threadObj.materialID];
	isSphere = (threadObj.colliderType == 1);
	isAABB = (threadObj.colliderType == 2);
	
	threadSphereID = threadObj.colliderID * isSphere;
	threadAABBID = threadObj.colliderID * 6 * isAABB;

	threadSphere = spheres[threadSphereID];
	threadSphereTransform = transformations[threadSphereID];
	threadAABBMin = (float3)(aabbs[threadAABBID], aabbs[threadAABBID + 1], aabbs[threadAABBID + 2]);
	threadAABBMax = (float3)(aabbs[threadAABBID + 3], aabbs[threadAABBID + 4], aabbs[threadAABBID + 5]);

	//;Get data the workgroup will be operating on
	__private float3 initialPosition, initialNormal, toViewer;
	__private float4 initialMaterial, initialDiffuse;

	initialPosition = read_imagef(positionTexture, threadCoord).xyz;
	initialNormal = read_imagef(normalTexture, threadCoord).xyz;
	initialDiffuse = read_imagef(diffuseTexture, threadCoord);
	initialMaterial = read_imagef(materialTexture, threadCoord);

	__private size_t pixelObjID;	
	__private struct GObject pixelObj;

	pixelObjID = (size_t)(initialMaterial.w);
	pixelObj = gObjects[pixelObjID];

	__private struct Material pixelMaterial;
	__private bool pixelIsSphere, pixelIsAABB;
	__private size_t pixelSphereID, pixelAABBID;
	__private float4 pixelSphere;
	__private float3 pixelAABBMin, pixelAABBMax;

	pixelMaterial = materials[pixelObj.materialID];

	pixelIsSphere = (pixelObj.colliderType == 1);
	pixelIsAABB = (pixelObj.colliderType == 2);
	
	pixelSphereID = pixelObj.colliderID * pixelIsSphere;
	pixelAABBID = pixelObj.colliderID * 6 * pixelIsAABB;

	pixelSphere = spheres[pixelSphereID];
	pixelAABBMin = (float3)(aabbs[pixelAABBID], aabbs[pixelAABBID + 1], aabbs[pixelAABBID + 2]);
	pixelAABBMax = (float3)(aabbs[pixelAABBID + 3], aabbs[pixelAABBID + 4], aabbs[pixelAABBID + 5]);

	toViewer = normalize(cameraPosition[0] - initialPosition);
	initialNormal = normalize(initialNormal);

	//;Determine 1st gen ray directions
	__private struct TransmissionRay transmissionRay;
	__private float3 initialShadowDirection, reflectionDirection, transmissionDirection, initialTransmissionPosition;
	__private bool totalInternalReflection;

	initialShadowDirection = -lightDirection[0];
	
	reflectionDirection = normalize(-toViewer - 2.0f * dot(-toViewer, initialNormal) * initialNormal);

	transmissionRay = CalculateTransmissionRaySphere(pixelSphere, pixelMaterial.indexOfRefraction, initialPosition, initialNormal, toViewer);
	initialTransmissionPosition = transmissionRay.pos;
	transmissionDirection = transmissionRay.direction;
	totalInternalReflection = transmissionRay.TIR;

	//;Perform different passes
	__private float4 localColor, reflectionColor, transmissionColor;

	//;Perform shadow pass
	__private float sphereShadowResult, aabbShadowResult;
	sphereShadowResult = RayCastSpheref(initialShadowDirection, initialPosition, threadSphere) * isSphere;
	aabbShadowResult = RayCastAABBf(initialShadowDirection, initialPosition, threadAABBMin, threadAABBMax) * isAABB;
	__private bool sphereShadowBool = (sphereShadowResult > 0.0f);
	__private bool aabbShadowBool = (aabbShadowResult > 0.0f);

	//;Perform reflection pass
	__private float sphereReflectionResult, aabbReflectionResult;
	__private float3 possibleReflectionNormal;

	possibleReflectionNormal = RayCastAABBv(reflectionDirection, initialPosition, threadAABBMin, threadAABBMax) * isAABB;
	aabbReflectionResult = max(fabs(possibleReflectionNormal.x), max( fabs(possibleReflectionNormal.y), fabs(possibleReflectionNormal.z))) * isAABB;
	sphereReflectionResult = RayCastSpheref(reflectionDirection, initialPosition, threadSphere) * isSphere;

	//;Perform transmission pass
	__private float sphereTransmissionResult, aabbTransmissionResult;
	__private float3 possibleTransmissionNormal;

	possibleTransmissionNormal = RayCastAABBv(transmissionDirection, initialTransmissionPosition, threadAABBMin, threadAABBMax) * isAABB;
	aabbTransmissionResult = max(fabs(possibleTransmissionNormal.x), max(fabs(possibleTransmissionNormal.y), fabs(possibleTransmissionNormal.z))) * isAABB;

	sphereTransmissionResult = RayCastSpheref(transmissionDirection, initialTransmissionPosition, threadSphere) * isSphere;


	//;Setup reduction memory
	//;shadow reduction memory
	valReduce[localID + shadowReduceOffset + pixelReduceOffset] = (1.0f - threadMaterial.transmittedCoefficient) * (sphereShadowBool || aabbShadowBool);
	//;Reflection reduction memory
	valReduce[localID + reflectionReduceOffset + pixelReduceOffset] = sphereReflectionResult + aabbReflectionResult;
	indexReduce[localID + reflectionReduceOffset + pixelReduceOffset] = localID;
	//;Transmission
	indexReduce[localID + transmissionReduceOffset + pixelReduceOffset] = localID;
	valReduce[localID + transmissionReduceOffset + pixelReduceOffset] = sphereTransmissionResult + aabbTransmissionResult;

	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	//;Reduce results
	__private size_t reductionVar = depth;
	while(reductionVar > 1)
	{
		int loss = (reductionVar % 2);
		reductionVar = (reductionVar >> 1);
		if(localID < reductionVar)
		{
			//;Shadow reduction
			valReduce[localID + shadowReduceOffset + pixelReduceOffset] += valReduce[localID + reductionVar + shadowReduceOffset + pixelReduceOffset];
			//;Reflection reduction
			__private size_t one = indexReduce[localID + reflectionReduceOffset + pixelReduceOffset];
			__private size_t two = indexReduce[localID + reflectionReduceOffset + reductionVar + pixelReduceOffset];
			indexReduce[localID + reflectionReduceOffset + pixelReduceOffset] = ReduceIntersection(one, two, valReduce[one + reflectionReduceOffset + pixelReduceOffset], valReduce[two + reflectionReduceOffset + pixelReduceOffset]);
			//;Transmission reduction
			one = indexReduce[localID + transmissionReduceOffset + pixelReduceOffset];
			two = indexReduce[localID + reductionVar + transmissionReduceOffset + pixelReduceOffset];
			indexReduce[localID + transmissionReduceOffset + pixelReduceOffset] = ReduceIntersection(one, two, valReduce[one + transmissionReduceOffset + pixelReduceOffset], valReduce[two + transmissionReduceOffset + pixelReduceOffset]);
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		if(loss && localID == 0)
		{
			//;Shadow reduction
			valReduce[0 + shadowReduceOffset + pixelReduceOffset] += valReduce[2 * reductionVar + shadowReduceOffset + pixelReduceOffset];
			//;ReflectionReduction
			__private size_t one = indexReduce[0 + reflectionReduceOffset + pixelReduceOffset];
			__private size_t two = indexReduce[2 * reductionVar + reflectionReduceOffset + pixelReduceOffset];
			indexReduce[0 + reflectionReduceOffset + pixelReduceOffset] = ReduceIntersection(one, two, valReduce[one + reflectionReduceOffset + pixelReduceOffset], valReduce[two + reflectionReduceOffset + pixelReduceOffset]);
			//;Transmission reduction
			one = indexReduce[0 + transmissionReduceOffset + pixelReduceOffset];
			two = indexReduce[2 * reductionVar + transmissionReduceOffset + pixelReduceOffset];
			indexReduce[0 + transmissionReduceOffset + pixelReduceOffset] = ReduceIntersection(one, two, valReduce[one + transmissionReduceOffset + pixelReduceOffset], valReduce[two + transmissionReduceOffset + pixelReduceOffset]);

		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	}
	work_group_barrier(CLK_LOCAL_MEM_FENCE);

	__private float reductionVal;
	reductionVal = valReduce[0 + shadowReduceOffset + pixelReduceOffset];

	//;Perform initial illumination pass
	__private float4 localMaterialVector;
	localMaterialVector = (float4)(pixelMaterial.ambientCoefficient, pixelMaterial.diffuseCoefficient, pixelMaterial.specularCoefficient, pixelMaterial.specularPower);

	localColor = PhongDirectionalIllumination(initialPosition, initialNormal, reductionVal, toViewer, lightColor[0], lightDirection[0], lightIntensity[0], localMaterialVector, initialDiffuse, pixelMaterial.specularColor);


	//;Determine surface properties of reflection
	__private  bool isReflection;
	__private struct GObject reflectionObject;
	__private struct Material reflectionMaterial;
	__private bool isReflectionSphere, isReflectionAABB;


	reductionVal = valReduce[indexReduce[0 + reflectionReduceOffset + pixelReduceOffset] + reflectionReduceOffset + pixelReduceOffset];
	isReflection = (reductionVal > 0.0f);
	reflectionObject = gObjects[indexReduce[0 + reflectionReduceOffset + pixelReduceOffset]];
	reflectionMaterial = materials[reflectionObject.materialID];
	isReflectionSphere = (reflectionObject.colliderType == 1) && isReflection;
	isReflectionAABB = (reflectionObject.colliderType == 2) && isReflection;


	__private size_t reflectionSphereID, reflectionAABBID;
	__private float4 reflectionSphere;
	__private float16 reflectionSphereTransform;
	__private float3 reflectionAABBMin, reflectionAABBMax;

	reflectionSphereID = reflectionObject.colliderID * isReflectionSphere;
	reflectionAABBID = reflectionObject.colliderID * 6 * isReflectionAABB;
	reflectionSphere = spheres[reflectionSphereID];
	reflectionSphereTransform = transformations[reflectionSphereID];

	reflectionAABBMin = (float3)(aabbs[reflectionAABBID], aabbs[reflectionAABBID + 1], aabbs[reflectionAABBID + 2]);
	reflectionAABBMax = (float3)(aabbs[reflectionAABBID + 3], aabbs[reflectionAABBID + 4], aabbs[reflectionAABBID + 5]);

	__private float3 reflectionPosition;
	__private float3 reflectionNormal;
	__private float4 reflectionDiffuse;
	__private float4 reflectionSpecular;
	__private float4 reflectionMaterialVector;
	
	
	//;Position and normal
	reflectionPosition = (initialPosition + reductionVal * reflectionDirection) * isReflection;

	__private float reflectionAdjustment = sqrt(reflectionSphere.w * reflectionSphere.w - dot(reflectionSphere.xyz - reflectionPosition.xyz, reflectionSphere.xyz - reflectionPosition.xyz)) * isReflectionSphere;

	reflectionPosition -= reflectionAdjustment * reflectionDirection;
	reductionVal -= reflectionAdjustment;

	//;TODO find another way to do this
	__local float3 publicReflectionNormal[NUMPIXELS];
	if(localID == indexReduce[0 + reflectionReduceOffset + pixelReduceOffset])
	{
		publicReflectionNormal[localPixelID] = possibleReflectionNormal;
	}

	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
	reflectionNormal = normalize((publicReflectionNormal[localPixelID] * isReflectionAABB) + ((reflectionPosition - reflectionSphere.xyz) * isReflectionSphere));

	//;UVs
	__private float4 reflectionUVs;
	reflectionUVs = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

	if(isReflectionSphere)
	{
		reflectionUVs.xy = TextureMapSphere(reflectionPosition, reflectionSphereTransform, reflectionSphere, reflectionMaterial.tile).xy;
	}
	else if(isReflectionAABB)
	{
		reflectionUVs.xy = TextureMapAABB(reflectionPosition, reflectionNormal, reflectionAABBMin, reflectionAABBMax, reflectionMaterial.tile) * isReflectionAABB;
	}

	reflectionUVs.z = reflectionMaterial.texturePoolID;

	//;Sample texture for diffuse
	const sampler_t sampler = 	CLK_NORMALIZED_COORDS_TRUE	|
					CLK_ADDRESS_REPEAT		|
					CLK_FILTER_LINEAR;
	reflectionDiffuse = read_imagef(textureArray, sampler, reflectionUVs);

	//;Material properties
	reflectionSpecular = reflectionMaterial.specularColor;
	reflectionMaterialVector = (float4)(reflectionMaterial.ambientCoefficient, reflectionMaterial.diffuseCoefficient, reflectionMaterial.specularCoefficient, reflectionMaterial.specularPower);
	


	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
	//;Determine surface properties of transmission
	__private bool isTransmission;
	__private struct GObject transmissionObject;
	__private struct Material transmissionMaterial;
	__private bool isTransmissionSphere, isTransmissionAABB;
	
	reductionVal = valReduce[indexReduce[0 + transmissionReduceOffset + pixelReduceOffset] + transmissionReduceOffset + pixelReduceOffset];
	isTransmission = (reductionVal > 0.0f);
	transmissionObject = gObjects[indexReduce[0 + transmissionReduceOffset + pixelReduceOffset]];
	transmissionMaterial = materials[transmissionObject.materialID];
	isTransmissionSphere = (transmissionObject.colliderType == 1) && isTransmission;
	isTransmissionAABB = (transmissionObject.colliderType == 2) && isTransmission;

	__private size_t transmissionSphereID, transmissionAABBID;
	__private float4 transmissionSphere;
	__private float16 transmissionSphereTransform;
	__private float3 transmissionAABBMin, transmissionAABBMax;

	transmissionSphereID = transmissionObject.colliderID * isTransmissionSphere;
	transmissionAABBID = transmissionObject.colliderID * 6 * isTransmissionAABB;
	transmissionSphere = spheres[transmissionSphereID];
	transmissionSphereTransform = transformations[transmissionSphereID];

	transmissionAABBMin = (float3)(aabbs[transmissionAABBID], aabbs[transmissionAABBID + 1], aabbs[transmissionAABBID + 2]);
	transmissionAABBMax = (float3)(aabbs[transmissionAABBID + 3], aabbs[transmissionAABBID + 4], aabbs[transmissionAABBID + 5]);

	__private float3 transmissionPosition;
	__private float3 transmissionNormal;
	__private float4 transmissionDiffuse;
	__private float4 transmissionSpecular;
	__private float4 transmissionMaterialVector;

	//;Position and normal
	transmissionPosition = (initialTransmissionPosition + reductionVal * transmissionDirection) * isTransmission;

	__private float transmissionAdjustment = sqrt(transmissionSphere.w * transmissionSphere.w - dot(transmissionSphere.xyz - transmissionPosition.xyz, transmissionSphere.xyz - transmissionPosition.xyz)) * isTransmissionSphere;

	transmissionPosition -= transmissionAdjustment * transmissionDirection;
	reductionVal -= transmissionAdjustment;

	//;TODO find another way to do this
	__local float3 publicTransmissionNormal[NUMPIXELS];
	if(localID == indexReduce[0 + transmissionReduceOffset + pixelReduceOffset])
	{
		publicTransmissionNormal[localPixelID] = possibleTransmissionNormal;
	}

	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
	transmissionNormal = normalize((publicTransmissionNormal[localPixelID] * isTransmissionAABB) + ((transmissionPosition - transmissionSphere.xyz) * isTransmissionSphere));

	//;UVs
	__private float4 transmissionUVs;
	transmissionUVs = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

	if(isTransmissionSphere)
	{
		transmissionUVs.xy = TextureMapSphere(transmissionPosition, transmissionSphereTransform, transmissionSphere, transmissionMaterial.tile).xy;
	}
	else if(isTransmissionAABB)
	{
		transmissionUVs.xy = TextureMapAABB(transmissionPosition, transmissionNormal, transmissionAABBMin, transmissionAABBMax, transmissionMaterial.tile) * isTransmissionAABB;
	}

	transmissionUVs.z = transmissionMaterial.texturePoolID;

	//;Sample texture for diffuse
	transmissionDiffuse = read_imagef(textureArray, sampler, transmissionUVs);

	//;Material properties
	transmissionSpecular = transmissionMaterial.specularColor;
	transmissionMaterialVector = (float4)(transmissionMaterial.ambientCoefficient, transmissionMaterial.diffuseCoefficient, transmissionMaterial.specularCoefficient, transmissionMaterial.specularPower);


	//;Perform second shadow pass
	__private float sphereTransShadowResult, aabbTransShadowResult;
	__private float sphereReflShadowResult, aabbReflShadowResult;
	sphereTransShadowResult = RayCastSpheref(initialShadowDirection, transmissionPosition, threadSphere) * isSphere;
	aabbTransShadowResult = RayCastAABBf(initialShadowDirection, transmissionPosition, threadAABBMin, threadAABBMax) * isAABB;

	sphereReflShadowResult = RayCastSpheref(initialShadowDirection, reflectionPosition, threadSphere) * isSphere;
	aabbReflShadowResult = RayCastAABBf(initialShadowDirection, reflectionPosition, threadAABBMin, threadAABBMax) * isAABB;
	__private bool sphereTransShadowBool = (sphereTransShadowResult > 0.0f);
	__private bool aabbTransShadowBool = (aabbTransShadowResult > 0.0f);

	__private bool sphereReflShadowBool = (sphereReflShadowResult > 0.0f);
	__private bool aabbReflShadowBool = (aabbReflShadowResult > 0.0f);
	//;Setup reduction memory
	valReduce[localID + transmissionReduceOffset + pixelReduceOffset] = (1.0f - threadMaterial.transmittedCoefficient) * (sphereTransShadowBool || aabbTransShadowBool);

	valReduce[localID + reflectionReduceOffset + pixelReduceOffset] = (1.0f - threadMaterial.transmittedCoefficient) * (sphereReflShadowBool || aabbReflShadowBool);

	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	//;Reduce results
	reductionVar = depth;
	while(reductionVar > 1)
	{
		int loss = (reductionVar % 2);
		reductionVar = (reductionVar >> 1);
		if(localID < reductionVar)
		{
			//;Shadow reduction
			valReduce[localID + reflectionReduceOffset + pixelReduceOffset] += valReduce[localID + reductionVar + reflectionReduceOffset + pixelReduceOffset];
			valReduce[localID + transmissionReduceOffset + pixelReduceOffset] += valReduce[localID + reductionVar + transmissionReduceOffset + pixelReduceOffset];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		if(loss && localID == 0)
		{
			//;Shadow reduction
			valReduce[0 + reflectionReduceOffset + pixelReduceOffset] += valReduce[2 * reductionVar + reflectionReduceOffset + pixelReduceOffset];
			valReduce[0 + transmissionReduceOffset + pixelReduceOffset] += valReduce[2 * reductionVar + transmissionReduceOffset + pixelReduceOffset];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	}
	work_group_barrier(CLK_LOCAL_MEM_FENCE);


	reductionVal = valReduce[0 + reflectionReduceOffset + pixelReduceOffset];

	//;Illuminate Reflection
	reflectionColor = PhongDirectionalIllumination(reflectionPosition, reflectionNormal, reductionVal, toViewer, lightColor[0], lightDirection[0], lightIntensity[0], reflectionMaterialVector, reflectionDiffuse, reflectionSpecular) * isReflection;


	reductionVal = valReduce[0 + transmissionReduceOffset + pixelReduceOffset];
	
	//;Illuminate transmission
	transmissionColor = PhongDirectionalIllumination(transmissionPosition, transmissionNormal, reductionVal, toViewer, lightColor[0], lightDirection[0], lightIntensity[0], transmissionMaterialVector, transmissionDiffuse, transmissionSpecular) * isTransmission;



	if(localID == 0)
	{
		__private float4 localGlobalMaterial = (float4)(pixelMaterial.localCoefficient, pixelMaterial.reflectedCoefficient, pixelMaterial.transmittedCoefficient, 0.0f);
		__private float4 transGlobalMaterial = (float4)(0.0f, totalInternalReflection * pixelMaterial.transmittedCoefficient, 0.0f, 0.0f);
		__private float4 finalGlobalMaterial = localGlobalMaterial + transGlobalMaterial;

		write_imagef(localTexture, threadCoord, localColor);
		write_imagef(reflectionTexture, threadCoord, reflectionColor);
		write_imagef(transmissionTexture, threadCoord, transmissionColor);
		write_imagef(globalMaterialTexture, threadCoord, finalGlobalMaterial);
	}
}

//;Here be dragons	
__kernel void RayTraceNew
(
	write_only image2d_t localTexture, write_only image2d_t reflectionTexture, 
	write_only image2d_t transmissionTexture, write_only image2d_t globalMaterialTexture,
	read_only image2d_t positionTexture, read_only image2d_t diffuseTexture,
	read_only image2d_t normalTexture, read_only image2d_t materialTexture, 
	read_only image2d_array_t textureArray,
	__constant float4* spheres, __constant float* aabbs,
	__constant struct GObject* gObjects, __constant struct Material* materials,
	__constant float3* cameraPosition, __constant float16* transformations,
	__constant float3* lightColor, __constant float3* lightDirection, __constant float2* lightIntensity,
	__local float* valReduce, __local size_t* indexReduce
)
{
	//;Get thread information
	__private size_t idX, idY, idZ, localID, depth;
	__private int2 threadCoord;
	idX = get_global_id(0);		//One ID for each unique X position of the textures
	idY = get_global_id(1);		//One ID for each unique Y position of the textures
	idZ = get_global_id(2);		//3 IDs for every object

	__private size_t width, height;
	width = get_local_size(0);	//Number of columns of pixels in each work group
	height = get_local_size(1);	//Number of rows of pixels in each work group
	depth = get_local_size(2);	//3 * the number of objects
	threadCoord = (int2)(idX, idY);
	
	__private size_t localX, localY;
	localX = get_local_id(0);
	localY = get_local_id(1);

	__private size_t reductionOffset = width * localY + localX;

	localID = get_local_id(2);	//One for every object

	__private int localPassType = 0;
	__private int reflectionPassType = 1;
	__private int transmissionPassType = 2;

	__private int passType;
       	passType = get_group_id(2);	//0 is local
					//1 is reflection
					//2 is transmission 

	//;Determine the data this thread will operate on
	__private struct GObject threadObj;
	__private struct Material threadMaterial;
	__private bool isAABB, isSphere;
	__private size_t threadSphereID, threadAABBID;
	__private float4 threadSphere;
	__private float3 threadAABBMin, threadAABBMax;
	__private float16 threadSphereTransform;


	threadObj = gObjects[localID];
	threadMaterial = materials[threadObj.materialID];
	isSphere = (threadObj.colliderType == 1);
	isAABB = (threadObj.colliderType == 2);
	
	threadSphereID = threadObj.colliderID * isSphere;
	threadAABBID = threadObj.colliderID * 6 * isAABB;

	threadSphere = spheres[threadSphereID];
	threadSphereTransform = transformations[threadSphereID];
	threadAABBMin = (float3)(aabbs[threadAABBID], aabbs[threadAABBID + 1], aabbs[threadAABBID + 2]);
	threadAABBMax = (float3)(aabbs[threadAABBID + 3], aabbs[threadAABBID + 4], aabbs[threadAABBID + 5]);

	//;Get data the workgroup will be operating on
	__private float3 initialPosition, initialNormal, toViewer;
	__private float4 initialMaterial, initialDiffuse;

	initialPosition = read_imagef(positionTexture, threadCoord).xyz;
	initialNormal = read_imagef(normalTexture, threadCoord).xyz;
	initialDiffuse = read_imagef(diffuseTexture, threadCoord);
	initialMaterial = read_imagef(materialTexture, threadCoord);

	__private size_t pixelObjID;	
	__private struct GObject pixelObj;

	pixelObjID = (size_t)(initialMaterial.w);
	pixelObj = gObjects[pixelObjID];

	__private struct Material pixelMaterial;
	__private bool pixelIsSphere, pixelIsAABB;
	__private size_t pixelSphereID, pixelAABBID;
	__private float4 pixelSphere;
	__private float3 pixelAABBMin, pixelAABBMax;

	pixelMaterial = materials[pixelObj.materialID];

	pixelIsSphere = (pixelObj.colliderType == 1);
	pixelIsAABB = (pixelObj.colliderType == 2);
	
	pixelSphereID = pixelObj.colliderID * pixelIsSphere;
	pixelAABBID = pixelObj.colliderID * 6 * pixelIsAABB;

	pixelSphere = spheres[pixelSphereID];
	pixelAABBMin = (float3)(aabbs[pixelAABBID], aabbs[pixelAABBID + 1], aabbs[pixelAABBID + 2]);
	pixelAABBMax = (float3)(aabbs[pixelAABBID + 3], aabbs[pixelAABBID + 4], aabbs[pixelAABBID + 5]);

	toViewer = normalize(cameraPosition[0] - initialPosition);
	initialNormal = normalize(initialNormal);


	//;Determine 1st gen ray directions

	if(passType == localPassType)
	{
		
		__private float3 initialShadowDirection;
		initialShadowDirection = -lightDirection[0];

		__private float4 localColor;

		//;Perform shadow pass
		__private float sphereShadowResult, aabbShadowResult;
		sphereShadowResult = RayCastSpheref(initialShadowDirection, initialPosition, threadSphere) * isSphere;
		aabbShadowResult = RayCastAABBf(initialShadowDirection, initialPosition, threadAABBMin, threadAABBMax) * isAABB;
		__private bool sphereShadowBool = (sphereShadowResult > 0.0f);
		__private bool aabbShadowBool = (aabbShadowResult > 0.0f);

		//;shadow reduction memory
		valReduce[localID] = (1.0f - threadMaterial.transmittedCoefficient) * (sphereShadowBool || aabbShadowBool);

		//;Reduce results
		__private size_t reductionVar = depth;
		while(reductionVar > 1)
		{
			int loss = (reductionVar % 2);
			reductionVar = (reductionVar >> 1);
			if(localID < reductionVar)
			{
				//;Shadow reduction
				valReduce[localID] += valReduce[localID + reductionVar];
			}
			work_group_barrier(CLK_LOCAL_MEM_FENCE);
			if(loss && localID == 0)
			{
				//;Shadow reduction
				valReduce[0] += valReduce[2 * reductionVar];
			}
			work_group_barrier(CLK_LOCAL_MEM_FENCE);
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);

		//;Perform initial illumination pass
		__private float4 localMaterialVector;
		localMaterialVector = (float4)(pixelMaterial.ambientCoefficient, pixelMaterial.diffuseCoefficient, pixelMaterial.specularCoefficient, pixelMaterial.specularPower);

		localColor = PhongDirectionalIllumination(initialPosition, initialNormal, valReduce[0], toViewer, lightColor[0], lightDirection[0], lightIntensity[0], localMaterialVector, initialDiffuse, pixelMaterial.specularColor);

		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		

		if(localID == 0 && passType == 0)
		{
			__private float4 localGlobalMaterial = (float4)(pixelMaterial.localCoefficient, pixelMaterial.reflectedCoefficient, pixelMaterial.transmittedCoefficient, 0.0f);
			__private float4 finalGlobalMaterial = localGlobalMaterial;

			write_imagef(localTexture, threadCoord, localColor);
			write_imagef(globalMaterialTexture, threadCoord, finalGlobalMaterial);
		}
		

	}
	else if(passType == reflectionPassType)
	{
		__private float3 reflectionDirection;
		reflectionDirection = normalize(-toViewer - 2.0f * dot(-toViewer, initialNormal) * initialNormal);

		__private float4 reflectionColor;

		//;Perform reflection pass
		__private float sphereReflectionResult, aabbReflectionResult;
		__private float3 possibleReflectionNormal;

		possibleReflectionNormal = RayCastAABBv(reflectionDirection, initialPosition, threadAABBMin, threadAABBMax) * isAABB;
		aabbReflectionResult = max(fabs(possibleReflectionNormal.x), max( fabs(possibleReflectionNormal.y), fabs(possibleReflectionNormal.z))) * isAABB;
		sphereReflectionResult = RayCastSpheref(reflectionDirection, initialPosition, threadSphere) * isSphere;

		//;Reflection reduction memory
		valReduce[localID] = sphereReflectionResult + aabbReflectionResult;
		indexReduce[localID] = localID;

		//;Reduce results
		__private size_t reductionVar = depth;
		while(reductionVar > 1)
		{
			int loss = (reductionVar % 2);
			reductionVar = (reductionVar >> 1);
			if(localID < reductionVar)
			{
				//;Reflection reduction
				__private size_t one = indexReduce[localID];
				__private size_t two = indexReduce[localID + reductionVar];
				indexReduce[localID] = ReduceIntersection(one, two, valReduce[one], valReduce[two]);
			}
			work_group_barrier(CLK_LOCAL_MEM_FENCE);
			if(loss && localID == 0)
			{
				//;ReflectionReduction
				__private size_t one = indexReduce[0];
				__private size_t two = indexReduce[2 * reductionVar];
				indexReduce[0] = ReduceIntersection(one, two, valReduce[one], valReduce[two]);
	
			}
			work_group_barrier(CLK_LOCAL_MEM_FENCE);
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		//;Determine surface properties of reflection
		__private  bool isReflection;
		__private struct GObject reflectionObject;
		__private struct Material reflectionMaterial;
		__private bool isReflectionSphere, isReflectionAABB;
		__private float reductionVal;

		reductionVal = valReduce[indexReduce[0]];
		isReflection = (reductionVal > 0.0f);
		reflectionObject = gObjects[indexReduce[0]];
		reflectionMaterial = materials[reflectionObject.materialID];
		isReflectionSphere = (reflectionObject.colliderType == 1) && isReflection;
		isReflectionAABB = (reflectionObject.colliderType == 2) && isReflection;


		__private size_t reflectionSphereID, reflectionAABBID;
		__private float4 reflectionSphere;
		__private float16 reflectionSphereTransform;
		__private float3 reflectionAABBMin, reflectionAABBMax;

		reflectionSphereID = reflectionObject.colliderID * isReflectionSphere;
		reflectionAABBID = reflectionObject.colliderID * 6 * isReflectionAABB;
		reflectionSphere = spheres[reflectionSphereID];
		reflectionSphereTransform = transformations[reflectionSphereID];

		reflectionAABBMin = (float3)(aabbs[reflectionAABBID], aabbs[reflectionAABBID + 1], aabbs[reflectionAABBID + 2]);
		reflectionAABBMax = (float3)(aabbs[reflectionAABBID + 3], aabbs[reflectionAABBID + 4], aabbs[reflectionAABBID + 5]);

		__private float3 reflectionPosition;
		__private float3 reflectionNormal;
		__private float4 reflectionDiffuse;
		__private float4 reflectionSpecular;
		__private float4 reflectionMaterialVector;
	
	
		//;Position and normal
		reflectionPosition = (initialPosition + reductionVal * reflectionDirection) * isReflection;

		__private float reflectionAdjustment = sqrt(reflectionSphere.w * reflectionSphere.w - dot(reflectionSphere.xyz - reflectionPosition.xyz, reflectionSphere.xyz - reflectionPosition.xyz)) * isReflectionSphere;

		reflectionPosition -= reflectionAdjustment * reflectionDirection;
		reductionVal -= reflectionAdjustment;

		//;TODO find another way to do this
		__local float3 publicReflectionNormal;
		if(localID == indexReduce[0])
		{
			publicReflectionNormal = possibleReflectionNormal;
		}

		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
		reflectionNormal = normalize((publicReflectionNormal * isReflectionAABB) + ((reflectionPosition - reflectionSphere.xyz) * isReflectionSphere));

		//;UVs
		__private float4 reflectionUVs;
		reflectionUVs = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

		if(isReflectionSphere)
		{
			reflectionUVs.xy = TextureMapSphere(reflectionPosition, reflectionSphereTransform, reflectionSphere, reflectionMaterial.tile).xy;
		}
		else if(isReflectionAABB)
		{
			reflectionUVs.xy = TextureMapAABB(reflectionPosition, reflectionNormal, reflectionAABBMin, reflectionAABBMax, reflectionMaterial.tile) * isReflectionAABB;
		}

		reflectionUVs.z = reflectionMaterial.texturePoolID;

		//;Sample texture for diffuse
		const sampler_t sampler = 	CLK_NORMALIZED_COORDS_TRUE	|
						CLK_ADDRESS_REPEAT		|
						CLK_FILTER_LINEAR;
		reflectionDiffuse = read_imagef(textureArray, sampler, reflectionUVs);

		//;Material properties
		reflectionSpecular = reflectionMaterial.specularColor;
		reflectionMaterialVector = (float4)(reflectionMaterial.ambientCoefficient, reflectionMaterial.diffuseCoefficient, reflectionMaterial.specularCoefficient, reflectionMaterial.specularPower);
	

		//;TODO: Perform reflection shadow pass

		//;Illuminate Reflection
		reflectionColor = PhongDirectionalIllumination(reflectionPosition, reflectionNormal, 0.0f, toViewer, lightColor[0], lightDirection[0], lightIntensity[0], reflectionMaterialVector, reflectionDiffuse, reflectionSpecular) * isReflection;
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
		if(localID == 0)
		{
			write_imagef(reflectionTexture, threadCoord, reflectionColor);
		}


	}
	else if(passType == transmissionPassType)
	{
		__private float3 transmissionDirection, initialTransmissionPosition;
		__private bool totalInternalReflection;

		struct TransmissionRay transmissionRay;
		transmissionRay = CalculateTransmissionRaySphere(pixelSphere, pixelMaterial.indexOfRefraction, initialPosition, initialNormal, toViewer);
		initialTransmissionPosition = transmissionRay.pos;
		transmissionDirection = transmissionRay.direction;
		totalInternalReflection = transmissionRay.TIR;

		__private float4 transmissionColor;

		//;Perform transmission pass
		__private float sphereTransmissionResult, aabbTransmissionResult;
		__private float3 possibleTransmissionNormal;

		possibleTransmissionNormal = RayCastAABBv(transmissionDirection, initialTransmissionPosition, threadAABBMin, threadAABBMax) * isAABB;
		aabbTransmissionResult = max(fabs(possibleTransmissionNormal.x), max(fabs(possibleTransmissionNormal.y), fabs(possibleTransmissionNormal.z))) * isAABB;

		sphereTransmissionResult = RayCastSpheref(transmissionDirection, initialTransmissionPosition, threadSphere) * isSphere;

		//;Transmission
		indexReduce[localID] = localID;
		valReduce[localID] = sphereTransmissionResult + aabbTransmissionResult;

		__private size_t reductionVar = depth;
		while(reductionVar > 1)
		{
			int loss = (reductionVar % 2);
			reductionVar = (reductionVar >> 1);
			if(localID < reductionVar)
			{
				//;transmission reduction
				__private size_t one = indexReduce[localID];
				__private size_t two = indexReduce[localID + reductionVar];
				indexReduce[localID] = ReduceIntersection(one, two, valReduce[one], valReduce[two]);
			}
			work_group_barrier(CLK_LOCAL_MEM_FENCE);
			if(loss && localID == 0)
			{
				//;transmissionReduction
				__private size_t one = indexReduce[0];
				__private size_t two = indexReduce[2 * reductionVar];
				indexReduce[0] = ReduceIntersection(one, two, valReduce[one], valReduce[two]);
	
			}
			work_group_barrier(CLK_LOCAL_MEM_FENCE);
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);



		//;Determine surface properties of transmission
		__private bool isTransmission;
		__private struct GObject transmissionObject;
		__private struct Material transmissionMaterial;
		__private bool isTransmissionSphere, isTransmissionAABB;
		__private float reductionVal;

		reductionVal = valReduce[indexReduce[0]];
		isTransmission = (reductionVal > 0.0f);
		transmissionObject = gObjects[indexReduce[0]];
		transmissionMaterial = materials[transmissionObject.materialID];
		isTransmissionSphere = (transmissionObject.colliderType == 1) && isTransmission;
		isTransmissionAABB = (transmissionObject.colliderType == 2) && isTransmission;

		__private size_t transmissionSphereID, transmissionAABBID;
		__private float4 transmissionSphere;
		__private float16 transmissionSphereTransform;
		__private float3 transmissionAABBMin, transmissionAABBMax;

		transmissionSphereID = transmissionObject.colliderID * isTransmissionSphere;
		transmissionAABBID = transmissionObject.colliderID * 6 * isTransmissionAABB;
		transmissionSphere = spheres[transmissionSphereID];
		transmissionSphereTransform = transformations[transmissionSphereID];

		transmissionAABBMin = (float3)(aabbs[transmissionAABBID], aabbs[transmissionAABBID + 1], aabbs[transmissionAABBID + 2]);
		transmissionAABBMax = (float3)(aabbs[transmissionAABBID + 3], aabbs[transmissionAABBID + 4], aabbs[transmissionAABBID + 5]);

		__private float3 transmissionPosition;
		__private float3 transmissionNormal;
		__private float4 transmissionDiffuse;
		__private float4 transmissionSpecular;
		__private float4 transmissionMaterialVector;

		//;Position and normal
		transmissionPosition = (initialTransmissionPosition + reductionVal * transmissionDirection) * isTransmission;

		__private float transmissionAdjustment = sqrt(transmissionSphere.w * transmissionSphere.w - dot(transmissionSphere.xyz - transmissionPosition.xyz, transmissionSphere.xyz - transmissionPosition.xyz)) * isTransmissionSphere;

		transmissionPosition -= transmissionAdjustment * transmissionDirection;
		reductionVal -= transmissionAdjustment;

		//;TODO find another way to do this
		__local float3 publicTransmissionNormal;
		if(localID == indexReduce[0])
		{
			publicTransmissionNormal = possibleTransmissionNormal;
		}

		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
		transmissionNormal = normalize((publicTransmissionNormal * isTransmissionAABB) + ((transmissionPosition - transmissionSphere.xyz) * isTransmissionSphere));

		//;UVs
		__private float4 transmissionUVs;
		transmissionUVs = (float4)(0.0f, 0.0f, 0.0f, 0.0f);

		if(isTransmissionSphere)
		{
			transmissionUVs.xy = TextureMapSphere(transmissionPosition, transmissionSphereTransform, transmissionSphere, transmissionMaterial.tile).xy;
		}
		else if(isTransmissionAABB)
		{
			transmissionUVs.xy = TextureMapAABB(transmissionPosition, transmissionNormal, transmissionAABBMin, transmissionAABBMax, transmissionMaterial.tile) * isTransmissionAABB;
		}
	
		transmissionUVs.z = transmissionMaterial.texturePoolID;
	
		//;Sample texture for diffuse
		const sampler_t sampler = 	CLK_NORMALIZED_COORDS_TRUE	|
						CLK_ADDRESS_REPEAT		|
						CLK_FILTER_LINEAR;

		transmissionDiffuse = read_imagef(textureArray, sampler, transmissionUVs);
	
		//;Material properties
		transmissionSpecular = transmissionMaterial.specularColor;
		transmissionMaterialVector = (float4)(transmissionMaterial.ambientCoefficient, transmissionMaterial.diffuseCoefficient, transmissionMaterial.specularCoefficient, transmissionMaterial.specularPower);
	
		//;TODO: Perform transmission shadow pass
	
		//;Illuminate transmission
		transmissionColor = PhongDirectionalIllumination(transmissionPosition, transmissionNormal, 0.0f, toViewer, lightColor[0], lightDirection[0], lightIntensity[0], transmissionMaterialVector, transmissionDiffuse, transmissionSpecular) * isTransmission;


		work_group_barrier(CLK_LOCAL_MEM_FENCE);

		if(localID == 0)
		{

			__private float4 transGlobalMaterial = (float4)(0.0f, totalInternalReflection * pixelMaterial.transmittedCoefficient, 0.0f, 0.0f);
			write_imagef(transmissionTexture, threadCoord, transmissionColor);
		}
		

	}


}
