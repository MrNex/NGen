#include "Kernel/RayCast.cl"

#include "Kernel/GObjectStruct.cl"
#include "Kernel/MaterialStruct.cl"

#define DIRECTIONAL 	1
#define POINT		2

#ifndef LIGHTTYPE
#define LIGHTTYPE	DIRECTIONAL
#endif


__kernel void ComputeDirectionalSphereShadowTexture(
		write_only image2d_t shadowTexture, read_only image2d_t positionTexture,
		__constant float3* light, __constant struct GObject* gObjects,
		__constant struct Material* materials, __constant float4* spheres, 
		__local bool* shadowReduce, __local float* matReduce)
{
	__local size_t idX, idY, idZ, d;
	idX = get_global_id(0);
	idY = get_global_id(1);
	d = get_global_size(2);

	__private size_t localID = get_local_id(2);

	__private struct GObject objStruct;
	objStruct = gObjects[localID];

	__private bool calculate;
	calculate = (objStruct.colliderType == 1);
	

	__private size_t sphereID = objStruct.colliderID * calculate;
	__private float4 sphere = spheres[sphereID];

	__local int2 threadCoord;
	threadCoord = (int2)(idX, idY);

	__local float4 position;
	position = read_imagef(positionTexture, threadCoord.xy);
	
	__private float3 rayDirection;
	#if LIGHTTYPE == DIRECTIONAL
		rayDirection = -1.0f * light[0];
		shadowReduce[localID] = RayCastSphere(rayDirection, position.xyz, sphere) && calculate;
	#elif LIGHTTYPE == POINT
		__private float3 toLight = light[0] - position.xyz;
		__private float len = length(toLight);
		rayDirection = normalize(light[0] - position.xyz);

		float result = RayCastSpheref(rayDirection, position.xyz, sphere);
		shadowReduce[localID] = (result > 0.0f) && (result < len) && calculate;
	#endif

	matReduce[localID] = (1.0f - materials[objStruct.materialID].transmittedCoefficient) * shadowReduce[localID];

	//matReduce[localID] = (1.0f - materials[onjStruct.materialID].transmittedCoefficient) * shadowReduce[localID];
	work_group_barrier(CLK_LOCAL_MEM_FENCE);

	size_t reductionVar = d;

	while(reductionVar > 1)
	{
		int loss = (reductionVar%2);
		reductionVar = (reductionVar >> 1);
		if(localID < (reductionVar))
		{
			
			matReduce[localID] = shadowReduce[localID] * matReduce[localID] + shadowReduce[localID + reductionVar] * matReduce[localID + reductionVar];
			shadowReduce[localID] |= shadowReduce[localID + reductionVar];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		if(loss)
		{
			matReduce[0] = shadowReduce[0] * matReduce[0] + shadowReduce[2 * reductionVar] * matReduce[2 * reductionVar];
			shadowReduce[0] |= shadowReduce[(2 * reductionVar)];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
	}
	work_group_barrier(CLK_LOCAL_MEM_FENCE);

	if(localID == 0)
	{
		__private float mat = min(matReduce[0], 1.0f);
		write_imagef(shadowTexture, threadCoord.xy, (float4)(mat * shadowReduce[0], 0.0f, 0.0f, 0.0f));
	}
}

__kernel void ComputeDirectionalAABBShadowTexture(
		write_only image2d_t shadowTexture, read_only image2d_t positionTexture,
		__constant float3* light, __constant struct GObject* gObjects,
		__constant struct Material* materials, __constant float* aabbs,
	       	__local bool* shadowReduce, __local size_t* indexReduce)
{
	__private size_t idX, idY, idZ, numAABBS;
	idX = get_global_id(0);		//Gets the X position of the pixel in the texture this thread is working with
	idY = get_global_id(1);		//Gets the Y position of the pixel in the texture this thread is working with
	numAABBS = get_global_size(2);	//Gets the number of AABBS calculating shadows for
	
	__private size_t localID;
	__private size_t minID; 
	__private size_t maxID; 

       	localID = get_local_id(2);//Gets the index of AABB this thread is calculating the shadows for

	__private struct GObject objStruct = gObjects[localID];
	__private bool calculate = (objStruct.colliderType == 2);

	__private size_t colliderID = objStruct.colliderID * calculate;

	minID = localID * 6;	//Gets the index of the minimum point of the AABB
	maxID = minID + 3;	//Gets the index of the Maximum point of the AABB
	//minID = colliderID * 6;
	//maxID = minID + 3;

	__private int2 threadCoord;
	threadCoord = (int2)(idX, idY);

	__private float3 rayPosition;
	rayPosition = read_imagef(positionTexture, threadCoord).xyz;	//Get the position of the surface in the pixel this thread is concerned with

	

	float3 minimum = (float3)( aabbs[minID] , aabbs[minID + 1], aabbs[minID + 2] );
	float3 maximum = (float3)( aabbs[minID + 3], aabbs[minID + 4], aabbs[minID + 5] );
	
	__private float3 rayDirection;
#if LIGHTTYPE == DIRECTIONAL
	rayDirection = normalize(-1.0f * light[0]);				//Travel to the light source, not from it
	shadowReduce[localID] = RayCastAABB(rayDirection, rayPosition, minimum, maximum);
#elif LIGHTTYPE == POINT
	__private float3 toLight = light[0] - rayPosition.xyz;
	__private float len = length(toLight);
	rayDirection = normalize(toLight);
	
	__private float result = RayCastAABBf(rayDirection, rayPosition, minimum, maximum);
	shadowReduce[localID] = ((result > 0.0f) && (result <= len));
#endif
	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	
	size_t reductionVar = numAABBS;

	while(reductionVar > 1)
	{
		int loss = (reductionVar % 2);
		reductionVar = (reductionVar >> 1);
		if(localID < reductionVar)
		{
			shadowReduce[localID] |= shadowReduce[localID + reductionVar];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		if(loss)
		{
			shadowReduce[0] |= shadowReduce[(2 * reductionVar)];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	}

	work_group_barrier(CLK_LOCAL_MEM_FENCE);

	if(localID == 0)
	{
		write_imagef(shadowTexture, threadCoord, (float4)(1.0f * shadowReduce[0], 0.0f, 0.0f, 1.0f));
	}
}

__kernel void ReduceShadowTextures(write_only image2d_t shadowTexture, read_only image2d_t sphereShadowTexture, read_only image2d_t aabbShadowTexture)
{
	__private size_t idX, idY;
	idX = get_global_id(0);
	idY = get_global_id(1);

	__private int2 threadCoord;
	threadCoord = (int2)(idX, idY);

	__private float4 shadowResult;
	__private float4 sphereShadow;
	__private float4 aabbShadow;
	sphereShadow = read_imagef(sphereShadowTexture, threadCoord);
	aabbShadow = read_imagef(aabbShadowTexture, threadCoord);

	shadowResult.x = max(sphereShadow.x, aabbShadow.x);

	write_imagef(shadowTexture, threadCoord, shadowResult);

}
