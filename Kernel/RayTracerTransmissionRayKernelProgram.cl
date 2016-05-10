#include "Kernel/RayCast.cl"
#include "Kernel/TextureMap.cl"
#include "Kernel/GObjectStruct.cl"
#include "Kernel/MaterialStruct.cl"
#include "Kernel/Illuminate.cl"

__kernel void ComputeSphereTransmissionTextures(
		write_only image2d_t outPosition, write_only image2d_t outColor, 
		write_only image2d_t outNormal, write_only image2d_t outMaterial, 
		write_only image2d_t outSpecular, write_only image2d_t outGlobalMaterial,
		write_only image2d_t outDepth,
		read_only image2d_t positionTexture, read_only image2d_t normalTexture,
		read_only image2d_array_t textureArray, __constant float4* spheres,
		__constant float* aabbs,
		__constant struct GObject* gObjects, __constant struct Material* materials,
		__constant float3* cameraPosition, __constant float16* transformations,
		__local float* transmissionReduce, __local size_t* indexReduce)
{
	//;Get thread information
	__private size_t idX, idY, localID, depth;
	idX = get_global_id(0);
	idY = get_global_id(1);

	depth = get_global_size(2);

	localID = get_local_id(2);

	//;Determine the data this thread will operate on
	__private struct GObject startingObjStruct, castObjStruct;
	startingObjStruct = castObjStruct = gObjects[localID];

	__private size_t startingSphereID, castSphereID, castAABBMinID;
	__private bool calculateStarting;
	__private bool calculateSphere;
	__private bool calculateAABB;

	calculateStarting = (startingObjStruct.colliderType == 1);
	calculateSphere = (castObjStruct.colliderType == 1);
	calculateAABB = (castObjStruct.colliderType == 2);
	
	startingSphereID = startingObjStruct.colliderID * calculateStarting;
	castSphereID = castObjStruct.colliderID * calculateSphere;
	castAABBMinID = castObjStruct.colliderID * 6 * calculateAABB;


	__private float4 startingSphere, castSphere; 
	startingSphere = spheres[startingSphereID];
	castSphere = spheres[castSphereID];

	__private float3 castAABBMin, castAABBMax;
	castAABBMin = (float3)(aabbs[castAABBMinID], aabbs[castAABBMinID + 1], aabbs[castAABBMinID + 2]);
	castAABBMax = (float3)(aabbs[castAABBMinID + 3], aabbs[castAABBMinID + 4], aabbs[castAABBMinID + 5]);
	

	indexReduce[localID] = localID * calculateStarting;

	__private int2 threadCoord;
	threadCoord = (int2)(idX, idY);

	__private float3 rayPosition, surfaceNormal, incidentRay, innerRayDirection;
	rayPosition = read_imagef(positionTexture, threadCoord).xyz;
	surfaceNormal = read_imagef(normalTexture, threadCoord).xyz;
	incidentRay = normalize(rayPosition - cameraPosition[0]);
	surfaceNormal = normalize(surfaceNormal);//Don't know if needed, but just in case I forgot to normalize somewhere cause I just wasted half a day solving that problem

	//;Determine the closest startingSphere
	transmissionReduce[localID] = fabs(((length(rayPosition - startingSphere.xyz) / startingSphere.w) - 1.0f) * calculateStarting);

	//transmissionReduce[localID] *= transmissionReduce[localID];

	work_group_barrier(CLK_LOCAL_MEM_FENCE);


	private size_t reductionVar;
	reductionVar = depth;

	
	while(reductionVar > 1)
	{
		int loss = (reductionVar % 2);
		reductionVar = (reductionVar >> 1);
		if(localID < (reductionVar))
		{
			__private bool expr = ((transmissionReduce[localID] <= 0.0f) || (transmissionReduce[localID + reductionVar] <= 0.0f));
			float maximum = max(transmissionReduce[localID], transmissionReduce[localID + reductionVar]);
			float minimum = min(transmissionReduce[localID], transmissionReduce[localID + reductionVar]);

			transmissionReduce[localID] = expr * maximum + (!expr) * minimum;

			expr = transmissionReduce[localID] == transmissionReduce[localID + reductionVar];
			indexReduce[localID] = expr * indexReduce[localID + reductionVar] + (!expr) * indexReduce[localID];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		if(loss && localID == 0)
		{

			__private bool expr = ((transmissionReduce[0] <= 0.0f) || (transmissionReduce[(2 * reductionVar)] <= 0.0f));
			float maximum = max(transmissionReduce[0], transmissionReduce[2 * reductionVar]);
			float minimum = min(transmissionReduce[0], transmissionReduce[2 * reductionVar]);

			transmissionReduce[0] = expr * maximum + (!expr) * minimum;
			
			expr = transmissionReduce[0] == transmissionReduce[2 * reductionVar];
			indexReduce[0] = expr * indexReduce[localID + reductionVar] + (!expr) * indexReduce[0];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
	}
	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
	
	

	/*	
	while(reductionVar > 1)
	{
		int loss = (reductionVar % 2);
		reductionVar = (reductionVar >> 1);
		if(localID < (reductionVar))
		{
			transmissionReduce[localID] = min(transmissionReduce[localID], transmissionReduce[localID + reductionVar]);

			__private bool expr;
			expr = transmissionReduce[localID] == transmissionReduce[localID + reductionVar];
			indexReduce[localID] = expr * indexReduce[localID + reductionVar] + (!expr) * indexReduce[localID];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		if(loss && localID == 0)
		{

			__private bool expr;
			transmissionReduce[0] = min(transmissionReduce[0], transmissionReduce[2 * reductionVar]);

			expr = transmissionReduce[0] == transmissionReduce[2 * reductionVar];
			indexReduce[0] = expr * indexReduce[localID + reductionVar] + (!expr) * indexReduce[0];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
	}
	*/
	
	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	

	//;Get the object information this ray is transmitting through
	startingObjStruct = gObjects[indexReduce[0]];
	startingSphereID = startingObjStruct.colliderID;
	startingSphere = spheres[startingSphereID];

	__private struct Material startingMatStruct;
	startingMatStruct = materials[startingObjStruct.materialID];

	//;Determine the direction of the ray travelling through the medium
	__private float iAngle;
	iAngle = -dot(incidentRay, surfaceNormal);

	__private float alpha;	//ratio of indices of refraction
	alpha = 1.0f / startingMatStruct.indexOfRefraction;
	__private float beta, discr;
	discr = 1.0f + (alpha * alpha * (iAngle * iAngle - 1.0f));

	__private bool totalExternalRefraction;
	totalExternalRefraction = (discr <= 0.0f);

	__private float negate;
	negate = 1.0f - 2.0f * totalExternalRefraction;
	discr = discr * negate;

	beta = alpha * iAngle - sqrt(discr);

	innerRayDirection = normalize((alpha * incidentRay + beta * surfaceNormal) * (!totalExternalRefraction));
	
	//;Determine the exit point properties
	__private float scaleFactor;
	scaleFactor = dot(innerRayDirection, startingSphere.xyz - rayPosition) * 2.0f;
	__private float3 exitPosition = rayPosition + innerRayDirection * scaleFactor;
	__private float3 exitNormal = normalize(exitPosition - startingSphere.xyz);

	//;Determine the direction of the transmission ray
	__private float eAngle = dot(innerRayDirection, exitNormal);
	__private float eAlpha = startingMatStruct.indexOfRefraction;
	__private float eBeta, eDiscr;
	eDiscr = 1.0f + (eAlpha * eAlpha * (eAngle * eAngle - 1.0f));
	__private bool totalInternalRefraction;
	totalInternalRefraction = (eDiscr < 0.0f);

	negate = 1.0f - 2.0f * totalInternalRefraction;
	eDiscr = eDiscr * negate;

	eBeta = eAlpha * eAngle - sqrt(eDiscr);

	__private float3 rayDirection;
	rayDirection = normalize((alpha * innerRayDirection + beta * exitNormal) * (!totalInternalRefraction));

	
	//;Perform ray casting against the appropriate sphere
	indexReduce[localID] = localID * calculateSphere;
	transmissionReduce[localID] = RayCastSpheref(rayDirection, exitPosition, castSphere) * calculateSphere;
	work_group_barrier(CLK_LOCAL_MEM_FENCE);

       	reductionVar = depth;

	while(reductionVar > 1)
	{
		int loss = (reductionVar % 2);
		reductionVar = (reductionVar >> 1);
		if(localID < (reductionVar))
		{
			__private bool expr = ((transmissionReduce[localID] <= 0.0f) || (transmissionReduce[localID + reductionVar] <= 0.0f));
			float maximum = max(transmissionReduce[localID], transmissionReduce[localID + reductionVar]);
			float minimum = min(transmissionReduce[localID], transmissionReduce[localID + reductionVar]);

			transmissionReduce[localID] = expr * maximum + (!expr) * minimum;

			expr = transmissionReduce[localID] == transmissionReduce[localID + reductionVar];
			indexReduce[localID] = expr * indexReduce[localID + reductionVar] + (!expr) * indexReduce[localID];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		if(loss && localID == 0)
		{

			__private bool expr = ((transmissionReduce[0] <= 0.0f) || (transmissionReduce[(2 * reductionVar)] <= 0.0f));
			float maximum = max(transmissionReduce[0], transmissionReduce[2 * reductionVar]);
			float minimum = min(transmissionReduce[0], transmissionReduce[2 * reductionVar]);

			transmissionReduce[0] = expr * maximum + (!expr) * minimum;
			
			expr = transmissionReduce[0] == transmissionReduce[2 * reductionVar];
			indexReduce[0] = expr * indexReduce[localID + reductionVar] + (!expr) * indexReduce[0];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
	}
	work_group_barrier(CLK_LOCAL_MEM_FENCE);

	__private float minSphereDepth = transmissionReduce[0];
	__private size_t minSphereIndex = indexReduce[0];

	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	//;Perform ray casting against the appropriate AABB

		
	transmissionReduce[localID] = 0.0f;
	indexReduce[localID] = localID * calculateAABB;
	__private float3 testNormal = RayCastAABBv(rayDirection, exitPosition, castAABBMin, castAABBMax);
       	transmissionReduce[localID] = max(fabs(testNormal.x), max(fabs(testNormal.y), fabs(testNormal.z))) * calculateAABB;
	
	

	work_group_barrier(CLK_LOCAL_MEM_FENCE);

	reductionVar = depth;

	while(reductionVar > 1)
	{
		int loss = (reductionVar % 2);
		reductionVar = (reductionVar >> 1);
		if(localID < (reductionVar))
		{
			__private bool expr = ((transmissionReduce[localID] <= 0.0f) || (transmissionReduce[localID + reductionVar] <= 0.0f));
			float maximum = max(transmissionReduce[localID], transmissionReduce[localID + reductionVar]);
			float minimum = min(transmissionReduce[localID], transmissionReduce[localID + reductionVar]);

			transmissionReduce[localID] = expr * maximum + (!expr) * minimum;

			expr = transmissionReduce[localID] == transmissionReduce[localID + reductionVar];
			indexReduce[localID] = expr * indexReduce[localID + reductionVar] + (!expr) * indexReduce[localID];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		if(loss && localID == 0)
		{

			__private bool expr = ((transmissionReduce[0] <= 0.0f) || (transmissionReduce[(2 * reductionVar)] <= 0.0f));
			float maximum = max(transmissionReduce[0], transmissionReduce[2 * reductionVar]);
			float minimum = min(transmissionReduce[0], transmissionReduce[2 * reductionVar]);

			transmissionReduce[0] = expr * maximum + (!expr) * minimum;
			
			expr = transmissionReduce[0] == transmissionReduce[2 * reductionVar];
			indexReduce[0] = expr * indexReduce[localID + reductionVar] + (!expr) * indexReduce[0];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
	}
	work_group_barrier(CLK_LOCAL_MEM_FENCE);

	__private float minAABBDepth = transmissionReduce[0];
	__private size_t finalMinAABBIndex = indexReduce[0];
	
	__private bool isSphereCloser;

	isSphereCloser = ((fabs(minSphereDepth) <= fabs(minAABBDepth)) && (minSphereDepth > 0.0f) || (minAABBDepth <= 0.0f));

	//isSphereCloser = true;

	__private float finalDepth = (minSphereDepth * isSphereCloser) + (minAABBDepth * (!isSphereCloser));
	__private size_t finalMinIndex = (minSphereIndex * isSphereCloser) + (finalMinAABBIndex * (!isSphereCloser));

	//finalDepth = minAABBDepth;
	//finalMinIndex = finalMinAABBIndex;

	
	//Get point of collision in worldspace
	__private float3 sphereWorldPosition, aabbWorldPosition, worldPosition;
	__private float4 sphereUVs, aabbUVs, uvs;
	__private float4 sphereColor, aabbColor, color;
	__private float3 sphereWorldNormal, aabbWorldNormal, worldNormal, globalMaterial;
	__private float4 material, sphereMaterial, aabbMaterial;
	__private float4 specular, sphereSpecular, aabbSpecular;
	__private float2 tile, sphereTile, aabbTile;

	sphereWorldPosition = (float3)(0.0f, 0.0f, 0.0f);
	aabbWorldPosition = (float3)(0.0f, 0.0f, 0.0f);
	worldPosition = (float3)(0.0f, 0.0f, 0.0f);
	sphereUVs = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	aabbUVs = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	uvs = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	sphereColor = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	aabbColor = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	color = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	sphereWorldNormal = (float3)(0.0f, 0.0f, 0.0f);
	aabbWorldNormal = (float3)(0.0f, 0.0f, 0.0f);
	worldNormal = (float3)(0.0f, 0.0f, 0.0f);
	material = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	sphereMaterial = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	aabbMaterial = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	specular = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	sphereSpecular = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	aabbSpecular = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	tile = (float2)(0.0f, 0.0f);
	sphereTile = (float2)(0.0f, 0.0f);
	aabbTile = (float2)(0.0f, 0.0f);
	globalMaterial = (float3)(0.0f, 0.0f, 0.0f);

	__private struct GObject sphereObjStruct, aabbObjStruct;
	__private struct Material sphereMatStruct, aabbMatStruct;

	if(totalInternalRefraction || totalExternalRefraction)
	{
		globalMaterial.y = startingMatStruct.transmittedCoefficient;
	}
	else if((finalDepth > 0.0f) /*&& !totalExternalRefraction && !totalInternalRefraction*/)
	{
		
		//objStruct = gObjects[minSphereIndex];
		sphereObjStruct = gObjects[minSphereIndex];
		sphereMatStruct = materials[sphereObjStruct.materialID];

		aabbObjStruct = gObjects[finalMinAABBIndex];
		aabbMatStruct = materials[aabbObjStruct.materialID];

		sphereMaterial = (float4)(sphereMatStruct.ambientCoefficient, sphereMatStruct.diffuseCoefficient, sphereMatStruct.specularCoefficient, sphereMatStruct.specularPower);
		sphereSpecular = sphereMatStruct.specularColor;
		sphereTile = sphereMatStruct.tile;

		aabbMaterial = (float4)(aabbMatStruct.ambientCoefficient, aabbMatStruct.diffuseCoefficient, aabbMatStruct.specularCoefficient, aabbMatStruct.specularPower);
		aabbSpecular = aabbMatStruct.specularColor;
		aabbTile = aabbMatStruct.tile;




		if(isSphereCloser)
		{
		//;Sphere begins here

		
		__private size_t sphereID;//ID of sphere hit
		__private float4 sphere;//Sphere hit

		sphereID = sphereObjStruct.colliderID;

		

		sphere = spheres[sphereID];

		//worldPosition = exitPosition + rayDirection * transmissionReduce[0];
		sphereWorldPosition = exitPosition + rayDirection * minSphereDepth;

		__private float sphereAdjustment = sqrt(sphere.w * sphere.w -
			dot(sphere.xyz - sphereWorldPosition, sphere.xyz - sphereWorldPosition));

		minSphereDepth -= sphereAdjustment;
		sphereWorldPosition -= sphereAdjustment * rayDirection;

		sphereWorldNormal = normalize(sphereWorldPosition - sphere.xyz);

		__private float16 transformation;
		transformation = transformations[sphereID];

		sphereUVs.xy = TextureMapSphere(sphereWorldPosition, transformation, sphere, sphereTile).xy;
		sphereUVs.z = (float)(sphereMatStruct.texturePoolID);

		const sampler_t sphereSampler =	CLK_NORMALIZED_COORDS_TRUE |
							CLK_ADDRESS_REPEAT |
							CLK_FILTER_LINEAR;
			
		sphereColor = (float4)read_imagef(textureArray, sphereSampler, sphereUVs);
		}
		//}
		//else
		//{

		
		const sampler_t aabbSampler =	CLK_NORMALIZED_COORDS_TRUE |
							CLK_ADDRESS_REPEAT |
							CLK_FILTER_LINEAR;

		__private size_t minAABBID;
		__private float3 minAABB, maxAABB;

		minAABBID = aabbObjStruct.colliderID * 6;

		minAABB = (float3)( aabbs[minAABBID] , aabbs[minAABBID + 1], aabbs[minAABBID + 2] );
		maxAABB = (float3)( aabbs[minAABBID + 3] , aabbs[minAABBID + 4], aabbs[minAABBID + 5] );

		aabbWorldNormal = normalize(testNormal);

		aabbWorldPosition = exitPosition + rayDirection * minAABBDepth;

		aabbUVs.xy = TextureMapAABB(aabbWorldPosition, aabbWorldNormal, minAABB, maxAABB, aabbTile);
		aabbUVs.z = (float)(aabbMatStruct.texturePoolID);

		aabbColor = (float4)read_imagef(textureArray, aabbSampler, aabbUVs);
		
	}
	
	work_group_barrier(CLK_LOCAL_MEM_FENCE);

	worldPosition = (isSphereCloser) * sphereWorldPosition + (!isSphereCloser) * aabbWorldPosition;
	worldNormal = (isSphereCloser) * sphereWorldNormal + (!isSphereCloser) * aabbWorldNormal;
	color = (isSphereCloser) * sphereColor + (!isSphereCloser) * aabbColor;
	specular = (isSphereCloser) * sphereSpecular + (!isSphereCloser) * aabbSpecular;
	material = (isSphereCloser) * sphereMaterial + (!isSphereCloser) * aabbMaterial;
	
	__private float4 finalColor;
	__private float shadowIntensity;
	__private float3 lightColor;
	__private float3 lightDirection;
	__private float2 lightIntensity;
	__private float3 camPos;

	//TODO: Replace with actual values
	lightIntensity = (float2)(0.2f, 1.0f);
	lightDirection = normalize((float3)(-1.0f, -1.0f, -0.3f));
	lightColor = (float3)(1.0f, 1.0f, 1.0f);
	shadowIntensity = 0.0f;
	camPos = cameraPosition[0];

       finalColor = (float4)(0.0f, 0.0f, 0.0f, 1.0f);


	finalColor = PhongDirectionalIllumination(worldPosition, worldNormal, shadowIntensity,
		       camPos, lightColor, lightDirection, lightIntensity,
		       material, color, specular);
	
	if(localID == finalMinIndex)
	{
		write_imagef(outPosition, threadCoord, (float4)(worldPosition, 1.0f));	
		write_imagef(outColor, threadCoord, finalColor);
		write_imagef(outNormal, threadCoord, (float4)(worldNormal, 0.0f));
		write_imagef(outMaterial, threadCoord, material);
		write_imagef(outSpecular, threadCoord, specular);
		write_imagef(outGlobalMaterial, threadCoord, (float4)(globalMaterial, 0.0f));
		write_imagef(outDepth, threadCoord, (float4)(finalDepth, 0.0f, 0.0f, 0.0f));
	}

}

__kernel void ReduceTransmissionTextures(	
		write_only image2d_t outPositionTexture, write_only image2d_t outDiffuseTexture,
		write_only image2d_t outNormalTexture, write_only image2d_t outMaterialTexture,
		write_only image2d_t outSpecularTexture,
		read_only image2d_t sPositionTexture, read_only image2d_t sDiffuseTexture,
		read_only image2d_t sNormalTexture, read_only image2d_t sMaterialTexture,
		read_only image2d_t sSpecularTexture, read_only image2d_t sDepthTexture,
		read_only image2d_t aPositionTexture, read_only image2d_t aDiffuseTexture,
		read_only image2d_t aNormalTexture, read_only image2d_t aMaterialTexture,
		read_only image2d_t aSpecularTexture, read_only image2d_t aDepthTexture)
{
	//;Get thread info
	__private size_t idX, idY;
	idX = get_global_id(0);
	idY = get_global_id(1);

	__private int2 threadCoord;
	threadCoord = (int2)(idX, idY);

	__private float sDepth;
	__private float3 sPosition, sNormal;
	__private float4 sDiffuse, sMaterial, sSpecular;


	//;Get relevent data
	sDepth = read_imagef(sDepthTexture, threadCoord).x;
	sPosition = read_imagef(sPositionTexture, threadCoord).xyz;
	sNormal = read_imagef(sNormalTexture, threadCoord).xyz;
	sDiffuse = read_imagef(sDiffuseTexture, threadCoord);
	sMaterial = read_imagef(sMaterialTexture, threadCoord);
	sSpecular = read_imagef(sSpecularTexture, threadCoord);

	__private float aDepth;
	__private float3 aPosition, aNormal;
	__private float4 aDiffuse, aMaterial, aSpecular;

	aDepth = read_imagef(aDepthTexture, threadCoord).x;
	aPosition = read_imagef(aPositionTexture, threadCoord).xyz;
	aNormal = read_imagef(aNormalTexture, threadCoord).xyz;
	aDiffuse = read_imagef(aDiffuseTexture, threadCoord);
	aMaterial = read_imagef(aMaterialTexture, threadCoord);
	aSpecular = read_imagef(aSpecularTexture, threadCoord);

	//;Reduce
	__private float4 outPosition, outNormal;
	__private float4 outDiffuse, outMaterial, outSpecular;

	
	__private bool xExpr = (sDepth > 0.0f);
	__private bool yExpr = (aDepth > 0.0f);
	__private bool negateAll = ((sDepth == 0.0f) && (aDepth == 0.0f));

	__private bool exprA = (((sDepth < aDepth) || !yExpr) && !negateAll);
	__private bool exprB = (((sDepth > aDepth) || !xExpr) && !negateAll);

	__private float exprAF = (float)(exprA);
	__private float exprBF = (float)(exprB);


	outPosition = (float4)( exprAF * sPosition + exprBF * aPosition, 1.0f);
	outNormal = (float4)(exprAF * sNormal + exprBF * aNormal, 0.0f);
	outDiffuse = exprAF * sDiffuse + exprBF * aDiffuse;
	outMaterial = exprAF * sMaterial + exprBF * aMaterial;
	outSpecular = exprAF * sSpecular + exprBF * aSpecular;
}
