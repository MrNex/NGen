#include "Kernel/RayCast.cl"
#include "Kernel/MaterialStruct.cl"
#include "Kernel/GObjectStruct.cl"
#include "Kernel/TextureMap.cl"

__kernel void ComputeSphereReflectionTextures(
		write_only image2d_t outPosition, write_only image2d_t outColor, 
		write_only image2d_t outNormal, write_only image2d_t outMaterial, 
		write_only image2d_t outSpecular, write_only image2d_t outDepth,
		read_only image2d_t positionTexture, read_only image2d_t normalTexture,
		read_only image2d_array_t textureArray, __constant float4* spheres,
		__constant struct GObject* gObjects, __constant struct Material* materials,
		__constant float3* cameraPosition, __constant float16* transformations,
		__local float* reflectionReduce, __local size_t* indexReduce)
{
	__private size_t idX, idY, localID, d;
	idX = get_global_id(0);
	idY = get_global_id(1);
	d = get_global_size(2);

	localID = get_local_id(2);

	__private struct GObject objStruct;
	objStruct = gObjects[localID];

	__private size_t sphereID;
	__private bool calculate;
	calculate = (objStruct.colliderType == 1);
	sphereID = objStruct.colliderID * calculate;

	indexReduce[localID] = localID * calculate;

	__private int2 threadCoord;
	threadCoord = (int2)(idX, idY);

	__private float3 rayPosition;
	rayPosition = read_imagef(positionTexture, threadCoord.xy).xyz;

	__private float3 surfaceNormal;
	surfaceNormal = read_imagef(normalTexture, threadCoord.xy).xyz;

	__private float3 incidentRay;
	incidentRay = normalize(rayPosition - cameraPosition[0]);

	__private float3 rayDirection;
	//rayDirection = incidentRay - 2.0f * dot(incidentRay, surfaceNormal) * surfaceNormal;

	rayDirection = normalize(incidentRay - 2.0f * dot(incidentRay, surfaceNormal) * surfaceNormal);

	__private float4 sphere = spheres[sphereID];

	reflectionReduce[localID] = 0.0f;
	reflectionReduce[localID] = RayCastSpheref(rayDirection, rayPosition, sphere) * calculate;

	work_group_barrier(CLK_LOCAL_MEM_FENCE);


	private size_t reductionVar;
       	reductionVar = d;

	while(reductionVar > 1)
	{
		int loss = (reductionVar % 2);
		reductionVar = (reductionVar >> 1);
		if(localID < (reductionVar))
		{
			__private bool expr = ((reflectionReduce[localID] <= 0.0f) || (reflectionReduce[localID + reductionVar] <= 0.0f));
			float maximum = max(reflectionReduce[localID], reflectionReduce[localID + reductionVar]);
			float minimum = min(reflectionReduce[localID], reflectionReduce[localID + reductionVar]);

			reflectionReduce[localID] = expr * maximum + (!expr) * minimum;

			expr = reflectionReduce[localID] == reflectionReduce[localID + reductionVar];
			indexReduce[localID] = expr * indexReduce[localID + reductionVar] + (!expr) * indexReduce[localID];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		if(loss && localID == 0)
		{

			__private bool expr = ((reflectionReduce[0] <= 0.0f) || (reflectionReduce[(2 * reductionVar)] <= 0.0f));
			float maximum = max(reflectionReduce[0], reflectionReduce[2 * reductionVar]);
			float minimum = min(reflectionReduce[0], reflectionReduce[2 * reductionVar]);

			reflectionReduce[0] = expr * maximum + (!expr) * minimum;
			
			expr = reflectionReduce[0] == reflectionReduce[2 * reductionVar];
			indexReduce[0] = expr * indexReduce[localID + reductionVar] + (!expr) * indexReduce[0];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
	}
	work_group_barrier(CLK_LOCAL_MEM_FENCE);


	//Get point of collision in worldspace
	__private float3 worldPosition;
	__private float4 uvs;
	__private float4 color;
	__private float3 worldNormal;
	__private float4 material;
	__private float4 specular;
	__private float2 tile;

	struct Material matStruct;

	worldPosition = (float3)(0.0f, 0.0f, 0.0f);
	uvs = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	color = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	worldNormal = (float3)(0.0f, 0.0f, 0.0f);
	material = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	specular = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	tile = (float2)(0.0f, 0.0f);
	
	__private float w = 0.0f;
	


	if(reflectionReduce[0] > 0.0f && localID == indexReduce[0])
	{
		objStruct = gObjects[indexReduce[0]];
		;//objStruct = gObjects[2];
		matStruct = materials[objStruct.materialID];
		;//matStruct = materials[4];

		w = 1.0f;
		material = (float4)(matStruct.ambientCoefficient, matStruct.diffuseCoefficient, matStruct.specularCoefficient, matStruct.specularPower) * matStruct.localCoefficient;
		specular = matStruct.specularColor;
		tile = matStruct.tile;

		//Update sphere
		sphereID = objStruct.colliderID;
		sphere = spheres[sphereID];

		worldPosition = rayPosition + rayDirection * reflectionReduce[0];

		float adjustment = sqrt(sphere.w * sphere.w - 
				dot(sphere.xyz - worldPosition, sphere.xyz - worldPosition));

		reflectionReduce[0] -= adjustment;
		worldPosition -= adjustment * rayDirection;

		worldNormal = normalize(worldPosition - sphere.xyz);


		__private float16 transformation;
		transformation = transformations[sphereID];

		uvs.xy = TextureMapSphere(worldPosition, transformation, sphere, tile).xy;
		uvs.z = (float)(matStruct.texturePoolID);

		const sampler_t sampler = 	CLK_NORMALIZED_COORDS_TRUE 	|
						CLK_ADDRESS_REPEAT		|
						CLK_FILTER_LINEAR;

		color = (float4)read_imagef(textureArray, sampler, uvs);
	}

	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	if(localID == indexReduce[0])
	{
		write_imagef(outPosition, threadCoord, (float4)(worldPosition, 1.0f));	
		write_imagef(outColor, threadCoord, color);
		write_imagef(outNormal, threadCoord, (float4)(worldNormal, 0.0f));
		write_imagef(outMaterial, threadCoord, material);
		write_imagef(outSpecular, threadCoord, specular);
		write_imagef(outDepth, threadCoord, (float4)(reflectionReduce[0], 0.0f, 0.0f, 0.0f));
	}


}

__kernel void ComputeAABBReflectionTextures(
		write_only image2d_t outPosition, write_only image2d_t outColor, 
		write_only image2d_t outNormal, write_only image2d_t outMaterial, 
		write_only image2d_t outSpecular, write_only image2d_t outDepth,
		read_only image2d_t positionTexture, read_only image2d_t normalTexture,
		read_only image2d_array_t textureArray, __constant float* aabbs,
		__constant struct GObject* gObjects, __constant struct Material* materials,
		__constant float3* cameraPosition, __local float* reflectionReduce,
		__local size_t* indexReduce)
{
	__private size_t idX, idY, localID, d, minID;
	idX = get_global_id(0);
	idY = get_global_id(1);
	d = get_global_size(2);

	localID = get_local_id(2);

	__private struct GObject objStruct;
	objStruct = gObjects[localID];

	__private bool calculate;
	calculate = (objStruct.colliderType == 2);

	minID = objStruct.colliderID * 6 * calculate;
	indexReduce[localID] = localID;

	__local float3 finalNormal;
	finalNormal= (float3)(0.0f, 0.0f, 0.0f);
	__private int2 threadCoord;
	__private float3 rayPosition, surfaceNormal, incidentRay, rayDirection, aabbMin, aabbMax;
	
	threadCoord = (int2)(idX, idY);
	
	rayPosition = read_imagef(positionTexture, threadCoord).xyz;
	surfaceNormal = read_imagef(normalTexture, threadCoord).xyz;
	
	incidentRay = normalize(rayPosition - cameraPosition[0]);
	rayDirection = normalize(incidentRay - 2.0f * dot(incidentRay, surfaceNormal) * surfaceNormal);

	aabbMin = (float3)( aabbs[minID] , aabbs[minID + 1], aabbs[minID + 2] );
	aabbMax = (float3)( aabbs[minID + 3] , aabbs[minID + 4], aabbs[minID + 5] );

	__private float3 testNormal = RayCastAABBv(rayDirection, rayPosition, aabbMin, aabbMax);
	
	reflectionReduce[localID] = max(fabs(testNormal.x), max(fabs(testNormal.y), fabs(testNormal.z))) * calculate;


	//Reduction begins here
	work_group_barrier(CLK_LOCAL_MEM_FENCE);

	size_t reductionVar = d;

	while(reductionVar > 1)
	{
		int loss = (reductionVar % 2);
		reductionVar = (reductionVar >> 1);
		if(localID < (reductionVar))
		{
			__private bool expr = ((reflectionReduce[localID] <= 0.0f) || (reflectionReduce[localID + reductionVar] <= 0.0f));
			float maximum = max(reflectionReduce[localID], reflectionReduce[localID + reductionVar]);
			float minimum = min(reflectionReduce[localID], reflectionReduce[localID + reductionVar]);

			reflectionReduce[localID] = expr * maximum + (!expr) * minimum;

			expr = reflectionReduce[localID] == reflectionReduce[localID + reductionVar];
			indexReduce[localID] = expr * indexReduce[localID + reductionVar] + (!expr) * indexReduce[localID];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		if(loss && localID == 0)
		{

			__private bool expr = ((reflectionReduce[0] <= 0.0f) || (reflectionReduce[(2 * reductionVar)] <= 0.0f));
			float maximum = max(reflectionReduce[0], reflectionReduce[2 * reductionVar]);
			float minimum = min(reflectionReduce[0], reflectionReduce[2 * reductionVar]);

			reflectionReduce[0] = expr * maximum + (!expr) * minimum;
			
			expr = reflectionReduce[0] == reflectionReduce[2 * reductionVar];
			indexReduce[0] = expr * indexReduce[2 * reductionVar] + (!expr) * indexReduce[0];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
	}
	work_group_barrier(CLK_LOCAL_MEM_FENCE);

	//Reduction ends here
	__private float4 diffuse, material, specular, uvs;
	__private float3 worldPosition, worldNormal;
	__private float2 tile;

	__private struct Material matStruct;

	uvs = diffuse = material = specular = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	worldPosition = worldNormal = (float3)(0.0f, 0.0f, 0.0f);
	uvs = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	tile = (float2)(0.0f, 0.0f);

	if(reflectionReduce[0] > 0.0f && localID == indexReduce[0])
	{
		objStruct = gObjects[indexReduce[0]];
		matStruct = materials[objStruct.materialID];

		finalNormal = normalize(testNormal);
		worldNormal = normalize(testNormal);
		material = (float4)(matStruct.ambientCoefficient, matStruct.diffuseCoefficient, matStruct.specularCoefficient, matStruct.specularPower);
		material *= matStruct.localCoefficient;
		specular = matStruct.specularColor;

		tile = matStruct.tile;

		minID = objStruct.colliderID * 6;

		//aabbMin = (float3)( aabbs[indexReduce[0]] , aabbs[indexReduce[0] + 1], aabbs[indexReduce[0] + 2] );
		//aabbMax = (float3)( aabbs[indexReduce[0] + 3] , aabbs[indexReduce[0] + 4], aabbs[indexReduce[0] + 5] );


		aabbMin = (float3)( aabbs[minID] , aabbs[minID + 1], aabbs[minID + 2] );
		aabbMax = (float3)( aabbs[minID + 3] , aabbs[minID + 4], aabbs[minID + 5] );

		worldPosition = rayPosition + rayDirection * reflectionReduce[0];

		/*
		__private bool xComp, yComp, zComp;
		xComp = (fabs(worldNormal.x) <= fabs(worldNormal.y));
		yComp = (fabs(worldNormal.y) <= fabs(worldNormal.z));
		zComp = !(xComp && yComp);
		__private float3 invertedNormal = (float3)(1.0f * (float)(xComp), 1.0f * (float)(yComp), 1.0f * (float)(zComp));

 		float3 planeSpace = ((worldPosition - aabbMin) / (aabbMax - aabbMin)) * invertedNormal;
		
		xComp = ((planeSpace.x > 0.0f) || (planeSpace.y == 0.0f) || (planeSpace.z == 0.0f));

		uvs.x = (float)(xComp) *  planeSpace.x + (float)(!xComp) * planeSpace.z;

		yComp = ((planeSpace.y > 0.0f) || (planeSpace.z == 0.0f) || (!xComp));

		uvs.y = (float)(yComp) * planeSpace.y + (float)(!yComp) * planeSpace.z;
		
		//Darn, got it backwards.
		uvs.xy = uvs.xy * tile;
		*/
		uvs.xy = TextureMapAABB(worldPosition, worldNormal, aabbMin, aabbMax, tile);

		uvs.z = matStruct.texturePoolID;

		const sampler_t sampler =	CLK_NORMALIZED_COORDS_TRUE	|
						CLK_ADDRESS_REPEAT		|
						CLK_FILTER_LINEAR;

		diffuse = (float4)read_imagef(textureArray, sampler, uvs);
	}

	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	if(localID == indexReduce[0])
	{
		write_imagef(outPosition, threadCoord, (float4)(worldPosition, 1.0f));
		write_imagef(outColor, threadCoord, diffuse);
		write_imagef(outNormal, threadCoord, (float4)(finalNormal, 0.0f));
		write_imagef(outMaterial, threadCoord, material);
		write_imagef(outSpecular, threadCoord, specular);
		write_imagef(outDepth, threadCoord, (float4)(reflectionReduce[0], 0.0f, 0.0f, 0.0f));
	}
}



__kernel void ReduceReflectionTextures(
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
	__private size_t idX, idY;
	idX = get_global_id(0);
	idY = get_global_id(1);

	__private int2 threadCoord;
	threadCoord = (int2)(idX, idY);

	__private float sDepth;
	__private float3 sPosition, sNormal;
	__private float4 sDiffuse, sMaterial, sSpecular;

	
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
	
	write_imagef(outPositionTexture, threadCoord, outPosition);
	write_imagef(outNormalTexture, threadCoord, outNormal);
	write_imagef(outDiffuseTexture, threadCoord, outDiffuse);
	write_imagef(outMaterialTexture, threadCoord, outMaterial);
	write_imagef(outSpecularTexture, threadCoord, outSpecular);

	
}


__kernel void ComputeSphereReflectionTexture(
		write_only image2d_t outPositionTexture, read_only image2d_t positionTexture,
		read_only image2d_t normalTexture, __constant float4* spheres, 
		__constant float3* cameraPosition, __constant float16* transformations,
	       	__local float* reflectionReduce, __local size_t* indexReduce)
{
	__private size_t idX, idY, localID, d;
	idX = get_global_id(0);
	idY = get_global_id(1);
	d = get_global_size(2);

	localID = get_local_id(2);

	indexReduce[localID] = localID;

	__private int2 threadCoord;
	threadCoord = (int2)(idX, idY);

	__private float3 rayPosition;
	rayPosition = read_imagef(positionTexture, threadCoord.xy).xyz;

	__private float3 surfaceNormal;
	surfaceNormal = read_imagef(normalTexture, threadCoord.xy).xyz;

	__private float3 incidentRay;
	incidentRay = normalize(rayPosition - cameraPosition[0]);

	__private float3 rayDirection;
	rayDirection = incidentRay - 2 * dot(incidentRay, surfaceNormal) * surfaceNormal;

	__private float4 sphere = spheres[localID];

	reflectionReduce[localID] = RayCastSpheref(rayDirection, rayPosition, spheres[localID]);

	work_group_barrier(CLK_LOCAL_MEM_FENCE);


	size_t reductionVar = d;

	while(reductionVar > 1)
	{
		int loss = (reductionVar % 2);
		reductionVar = (reductionVar >> 1);
		if(localID < (reductionVar))
		{
			__private bool expr = ((reflectionReduce[localID] <= 0.0f) || (reflectionReduce[localID + reductionVar] <= 0.0f));
			float maximum = max(reflectionReduce[localID], reflectionReduce[localID + reductionVar]);
			float minimum = min(reflectionReduce[localID], reflectionReduce[localID + reductionVar]);

			reflectionReduce[localID] = expr * maximum + (!expr) * minimum;

			expr = reflectionReduce[localID] == reflectionReduce[localID + reductionVar];
			indexReduce[localID] = expr * indexReduce[localID + reductionVar] + (!expr) * indexReduce[localID];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		if(loss)
		{

			__private bool expr = ((reflectionReduce[0] <= 0.0f) || (reflectionReduce[(2 * reductionVar)] <= 0.0f));
			float maximum = max(reflectionReduce[0], reflectionReduce[2 * reductionVar]);
			float minimum = min(reflectionReduce[0], reflectionReduce[2 * reductionVar]);

			reflectionReduce[0] = expr * maximum + (!expr) * minimum;
			
			expr = reflectionReduce[0] == reflectionReduce[2 * reductionVar];
			indexReduce[0] = expr * indexReduce[localID + reductionVar] + (!expr) * indexReduce[0];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
	}
	work_group_barrier(CLK_LOCAL_MEM_FENCE);


	//Get point of collision in worldspace
	__private float3 worldPosition;
	__private float2 uvs;
	__private float3 worldNormal;
	uvs = (float2)(0.0f, 0.0f);
	__private float w = 0.0f;
	if(reflectionReduce[0] > 0.0f)
	{
		w = 1.0f;


		//Update sphere
		sphere = spheres[indexReduce[0]];

		worldPosition = rayPosition + rayDirection * reflectionReduce[0];


		float adjustment = sqrt(sphere.w * sphere.w - dot(sphere.xyz - worldPosition, sphere.xyz - worldPosition));
		worldPosition -= adjustment * rayDirection;

		__private float16 transformation;
		transformation = transformations[indexReduce[0]];
		__private float3 xAxis, yAxis, zAxis, translation;
		xAxis = transformation.lo.lo.xyz;//transformation.even.even.xyz;
		yAxis = transformation.lo.hi.xyz;//transformation.even.odd.xyz;
		zAxis = transformation.hi.lo.xyz;//transformation.odd.even.xyz;
		translation = transformation.odd.odd.xyz;

			
		__private float3 vertex;
		
		worldNormal = worldPosition - sphere.xyz;

		vertex = (float3)(dot(xAxis, worldNormal), dot(yAxis, worldNormal), dot(zAxis, worldNormal));

		worldNormal = normalize(worldNormal);

		__private float3 sphereCenter;
		sphereCenter = spheres[indexReduce[0]].xyz;
		__private float3 dist = worldPosition - sphereCenter;

		float theta = atan2pi(vertex.x, vertex.z);	//0 to 2
		float phi = acospi(vertex.y / sphere.w); //0 to 1

		uvs.x = 0.5 * theta + 0.5f;
		uvs.y = 1.0f - phi;

		
	}
	else
	{
		indexReduce[0] = -1;
	}

	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	if(localID == 0)
	{
		write_imagef(outPositionTexture, threadCoord, (float4)(uvs, indexReduce[0], w));
	}

}
