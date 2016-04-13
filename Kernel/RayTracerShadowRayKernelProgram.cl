#include "Kernel/RayCast.cl"

#define DIRECTIONAL 	1
#define POINT		2

#ifndef LIGHTTYPE
#define LIGHTTYPE	DIRECTIONAL
#endif

__kernel void ComputeDirectionalSphereShadowTexture(
		write_only image2d_t shadowTexture, read_only image2d_t positionTexture, 
		__constant float3* light, __constant float4* spheres, __local bool* shadowReduce)
{
	__local size_t idX, idY, idZ, d;
	idX = get_global_id(0);
	idY = get_global_id(1);
	d = get_global_size(2);

	size_t localID = get_local_id(2);


	__local int2 threadCoord;
	threadCoord = (int2)(idX, idY);

	__local float4 position;
	position = read_imagef(positionTexture, threadCoord.xy);
	
	__private float3 rayDirection;
	#if LIGHTTYPE == DIRECTIONAL
		rayDirection = -1.0f * light[0];
		shadowReduce[localID] = RayCastSphere(rayDirection, position.xyz, spheres[localID]);
	#elif LIGHTTYPE == POINT
		__private float3 toLight = light[0] - position.xyz;
		__private float len = length(toLight);
		rayDirection = normalize(light[0] - position.xyz);

		float result = RayCastSpheref(rayDirection, position.xyz, spheres[localID]);
		shadowReduce[localID] = (result > 0.0f) && (result < len);
	#endif


	work_group_barrier(CLK_LOCAL_MEM_FENCE);

	size_t reductionVar = d;

	while(reductionVar > 1)
	{
		int loss = (reductionVar%2);
		reductionVar = (reductionVar >> 1);
		if(localID < (reductionVar))
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
		write_imagef(shadowTexture, threadCoord.xy, (float4)(1.0f * shadowReduce[0], 0.0f, 0.0f, 1.0f));
	}
}

__kernel void ComputeDirectionalAABBShadowTexture(
		write_only image2d_t shadowTexture, read_only image2d_t positionTexture,
		__constant float3* light, __constant float* aabbs, __local bool* shadowReduce)
{
	__private size_t idX, idY, idZ, numAABBS;
	idX = get_global_id(0);		//Gets the X position of the pixel in the texture this thread is working with
	idY = get_global_id(1);		//Gets the Y position of the pixel in the texture this thread is working with
	numAABBS = get_global_size(2);	//Gets the number of AABBS calculating shadows for
	
	__private size_t localID;
	__private size_t minID; 
	__private size_t maxID; 

       	localID = get_local_id(2);//Gets the index of AABB this thread is calculating the shadows for
	minID = localID * 6;	//Gets the index of the minimum point of the AABB
	maxID = minID + 3;	//Gets the index of the Maximum point of the AABB

	minID = 6;
	maxID = 9;

	__private int2 threadCoord;
	threadCoord = (int2)(idX, idY);

	__private float3 rayPosition;
	rayPosition = read_imagef(positionTexture, threadCoord).xyz;	//Get the position of the surface in the pixel this thread is concerned with

	__private float3 rayDirection;
	rayDirection = -1.0f * light[0];				//Travel to the light source, not from it

	float3 minimum = (float3)( aabbs[minID] , aabbs[minID + 1], aabbs[minID + 2] );
	float3 maximum = (float3)( aabbs[minID + 3], aabbs[minID + 4], aabbs[minID + 5] );

	shadowReduce[localID] = RayCastAABB(rayDirection, rayPosition, minimum, maximum);
	
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

	shadowResult.x = ((sphereShadow.x + aabbShadow.x) > 0.0f);

	write_imagef(shadowTexture, threadCoord, shadowResult);

}
