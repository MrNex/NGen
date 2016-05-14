
__kernel void ReductionKernel
(
 	__global float* globalReduce, __local float* valReduce
)
{
	__private size_t localW, localH, numLocal;	
	localW = get_local_size(0);
	localH = get_local_size(1);
	numLocal = localW * localH;


	__private size_t localX, localY, localID;
	localX = get_local_id(0);
	localY = get_local_id(1);
	localID = localY * localW + localX;

	__private size_t groupW, groupH, numGroups;
	groupW = get_num_groups(0);
	groupH = get_num_groups(1);
	numGroups = groupW * groupH;



	__private size_t groupX, groupY, groupID;
	groupX = get_group_id(0);
	groupY = get_group_id(1);
	groupID = groupY * groupW + groupX;

	valReduce[localID] = globalReduce[groupID * numLocal + localID];
	
	__private size_t reductionVar = numLocal;
	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	work_group_barrier(CLK_GLOBAL_MEM_FENCE);
	while(reductionVar > 1)
	{
		__private int loss = (reductionVar % 2);
		reductionVar = (reductionVar >> 1);
		if(localID < reductionVar)
		{
			valReduce[localID] = valReduce[localID] + valReduce[localID + reductionVar];
			
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		if(loss && localID == 0)
		{
			valReduce[0] = valReduce[0] + valReduce[reductionVar * 2];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	}
	work_group_barrier(CLK_LOCAL_MEM_FENCE);

	if(localID == 0)
	{
		globalReduce[groupID] = valReduce[0];
	}

}

__kernel void ToneReproduction
(
	read_only image2d_t localTexture, read_only image2d_t reflectionTexture,
	read_only image2d_t transmissionTexture, read_only image2d_t globalMaterialTexture,
	__constant float4* luminenceParams, 
	__global float* globalReduce, __local float* valReduce
)
{
	__private size_t idX, idY;
	idX = get_global_id(0);
	idY = get_global_id(1);

	__private size_t localW, localH, numLocal;
	localW = get_local_size(0);
	localH = get_local_size(1);
	numLocal = localW * localH;


	__private size_t localX, localY, localID;
	localX = get_local_id(0);
	localY = get_local_id(1);
	localID = localY * localW + localX;

	__private size_t groupW, groupH, numGroups;
	groupW = get_num_groups(0);
	groupH = get_num_groups(1);
	numGroups = groupW * groupH;

	__private size_t groupX, groupY, groupID;
	groupX = get_group_id(0);
	groupY = get_group_id(1);
	groupID = groupY * groupW + groupX;

	__private int2 threadCoord = (int2)(idX, idY);
	__private float4 localSample, reflectionSample, transmissionSample, materialSample;

	localSample = read_imagef(localTexture, threadCoord);
	reflectionSample = read_imagef(reflectionTexture, threadCoord);
	transmissionSample = read_imagef(transmissionTexture, threadCoord);
	materialSample = read_imagef(globalMaterialTexture, threadCoord);

	__private float4 finalColor = materialSample.x * localSample + materialSample.y * reflectionSample + materialSample.z * transmissionSample;

	__private float lDMax = luminenceParams[0].x;
	__private float lMax = luminenceParams[0].y;

	__private float3 luminence = lMax * finalColor.xyz;
	__private float3 scaleVec = (float3)(0.27f, 0.67f, 0.06f);
	__private float absLumins = dot(luminence, scaleVec);

	//;Ward tone reproduction

	//;First calculate log-average luminence
	//;Start by finding sum of all luminence values using reduction over local threads
	__private float offset = 0.0001f;
	valReduce[localID] = log(absLumins + offset);

	__private size_t reductionVar;
	reductionVar = numLocal;

	work_group_barrier(CLK_LOCAL_MEM_FENCE);
	while(reductionVar > 1)
	{
		__private int loss = (reductionVar % 2);
		reductionVar = (reductionVar >> 1);
		if(localID < reductionVar)
		{
			valReduce[localID] = valReduce[localID] + valReduce[localID + reductionVar];
			
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
		if(loss && localID == 0)
		{
			valReduce[0] = valReduce[0] + valReduce[reductionVar * 2];
		}
		work_group_barrier(CLK_LOCAL_MEM_FENCE);
	}
	work_group_barrier(CLK_LOCAL_MEM_FENCE);

	if(localID == 0)
	{
		globalReduce[groupID] = valReduce[0];
	}

}

float3 Ward(__private float4 color, __private float lDMax, __private float lMax, __private float lAverage)
{

	__private float3 luminence = lMax * color.xyz;

	__private float scaleFactor;
	__private float num = (1.219f + pow((lDMax * 0.5f), 0.4f));
	__private float den = (1.219f + pow(lAverage, 0.4f));

	scaleFactor = powr(num/den, 2.5f);

	__private float3 target;

	target = scaleFactor * luminence;
	
	__private float3 final;
	final = target / lDMax;

	return final;
}

float3 Reinhard(__private float4 color, __private float lDMax, __private float lMax, __private float lAverage)
{
	__private float3 luminence = lMax * color.xyz;
	__private float3 scaled = (0.18f / (lAverage)) * luminence;
	__private float3 target = (scaled / ((float3)(1.0f, 1.0f, 1.0f) + scaled));

	return target;
}
__kernel void ToneReproductionSecond
(
	write_only image2d_t finalTexture,
	read_only image2d_t localTexture, read_only image2d_t reflectionTexture,
	read_only image2d_t transmissionTexture, read_only image2d_t globalMaterialTexture,
	__constant float4* luminenceParams, 
	__global float* globalReduce
)
{
	__private size_t idX, idY;
	idX = get_global_id(0);
	idY = get_global_id(1);

	__private size_t localW, localH, numLocal;
	localW = get_local_size(0);
	localH = get_local_size(1);
	numLocal = localW * localH;

	__private size_t localX, localY, localID;
	localX = get_local_id(0);
	localY = get_local_id(1);
	localID = localY * localW + localX;

	__private size_t groupW, groupH, numGroups;
	groupW = get_num_groups(0);
	groupH = get_num_groups(1);
	numGroups = groupW * groupH;

	__private size_t groupX, groupY, groupID;
	groupX = get_group_id(0);
	groupY = get_group_id(1);
	groupID = groupY * groupW + groupX;

	__private int2 threadCoord = (int2)(idX, idY);

	__private float4 localSample, reflectionSample, transmissionSample, materialSample;

	localSample = read_imagef(localTexture, threadCoord);
	reflectionSample = read_imagef(reflectionTexture, threadCoord);
	transmissionSample = read_imagef(transmissionTexture, threadCoord);
	materialSample = read_imagef(globalMaterialTexture, threadCoord);

	__private float4 finalColor = materialSample.x * localSample + materialSample.y * reflectionSample + materialSample.z * transmissionSample;


	__private float lDMax = luminenceParams[0].x;
	__private float lMax = luminenceParams[0].y;


	__private float lAverage;

	lAverage = exp(globalReduce[0] / (600.0f * 800.0f));

	__private float3 final;
	final = finalColor.xyz;
	//final = Reinhard(finalColor, lDMax, lMax, lAverage);
	//final = Ward(finalColor, lDMax, lMax, lAverage);


	write_imagef(finalTexture, threadCoord, (float4)(final, 1.0f));

}
