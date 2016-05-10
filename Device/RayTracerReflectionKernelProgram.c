#include "RayTracerReflectionKernelProgram.h"

#include <stdlib.h>
#include <CL/cl_gl.h>

#include "../Manager/AssetManager.h"
#include "../Manager/KernelManager.h"
#include "../Manager/CollisionManager.h"
#include "../Manager/EnvironmentManager.h"
#include "../Manager/RenderingManager.h"
#include "../Manager/ObjectManager.h"

typedef struct RayTracerReflectionKernelProgram_Members
{
	cl_kernel sphereReflectionKernel;
	cl_kernel aabbReflectionKernel;

	cl_kernel reduceTexturesKernel;
} RayTracerReflectionKernelProgram_Members;

///
//Allocates memory for the members of this kernel program
//
//Returns:
//	An uninitialized pointer to memory for RayTracerReflectionKernelProgram members
static RayTracerReflectionKernelProgram_Members* RayTracerReflectionKernelProgram_AllocateMembers(void);

///
//Initializes the kernel program's members
//
//Parameters:
//	prog: A pointer to the kernel program to initialize the members of
//	buffer: A pointer to The currently active Kernel buffer
static void RayTracerReflectionKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer);

///
//Frees the members of a RayTracerReflection kernel program
//
//Parameters:
//	prog: A pointer to the RayTracerReflection KernelProgram having it's members freed
static void RayTracerReflectionKernelProgram_FreeMembers(KernelProgram* prog);

///
//Executes a RayTracerReflectionKernelProgram
//
//Parameters:
//	prog: A pointer to the RayTracerReflectionKernelProgram to execute
//	buffer: A pointer to the active KernelBuffer containing the context and device on which to execute
//	params: A pointer to a struct containing parameters for Reflection KErnal Program execution
static void RayTracerReflectionKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, struct RayTracerReflectionKernelProgram_ExecutionParameters* params);

///
//Initializes a RayTracerReflectionKernelProgram
//
//Parameters:
//	prog: A pointer to an uninitialized kernel program to initialize as a ray tracer reflection kernel program
//	buffer: A pointer to te KernelBuffer containing the context and device on which to build the kernel
void RayTracerReflectionKernelProgram_Initialize(KernelProgram* prog, struct KernelBuffer* buffer)
{
	KernelProgram_Initialize(prog, "Kernel/RayTracerReflectionRayKernelProgram.cl", buffer);

	prog->members = RayTracerReflectionKernelProgram_AllocateMembers();
	RayTracerReflectionKernelProgram_InitializeMembers(prog, buffer);

	prog->FreeMembers = RayTracerReflectionKernelProgram_FreeMembers;
	prog->Execute = (KernelProgram_ExecuteFunc)RayTracerReflectionKernelProgram_Execute;
}


///
//Allocates memory for the members of this kernel program
//
//Returns:
//	An uninitialized pointer to memory for RayTracerReflectionKernelProgram members
static RayTracerReflectionKernelProgram_Members* RayTracerReflectionKernelProgram_AllocateMembers(void)
{
	return malloc(sizeof(RayTracerReflectionKernelProgram_Members));
}

///
//Initializes the kernel program's members
//
//Parameters:
//	prog: A pointer to the kernel program to initialize the members of
//	buffer: A pointer to The currently active Kernel buffer
static void RayTracerReflectionKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer)
{
	RayTracerReflectionKernelProgram_Members* members = prog->members;
	
	cl_int err;

	members->sphereReflectionKernel = clCreateKernel(prog->clProgram, "ComputeSphereReflectionTextures", &err);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_InitializeMembers :: clCreateKernel :: sphereReflectionKernel");

	members->aabbReflectionKernel = clCreateKernel(prog->clProgram, "ComputeAABBReflectionTextures", &err);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_InitializeMembers :: clCreateKernel :: AABBReflectionsKernel");

	members->reduceTexturesKernel = clCreateKernel(prog->clProgram, "ReduceReflectionTextures", &err);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_InitializeMembers :: clCreateKernel :: reduceTexturesKernel");
}

///
//Frees the members of a RayTracerReflection kernel program
///
//Initializes a RayTracerReflectionKernelProgram
//
//Parameters:
//	prog: A pointer to the RayTracerReflection KernelProgram having it's members freed
static void RayTracerReflectionKernelProgram_FreeMembers(KernelProgram* prog)
{
	RayTracerReflectionKernelProgram_Members* members = prog->members;
	cl_int err;

	free(members);
	prog->members = NULL;
}

static void RayTracerReflectionKernelProgram_ExecuteSphereKernel(
		KernelBuffer* buffer, struct RayTracerReflectionKernelProgram_Members* members, 
		struct RayTracerReflectionKernelProgram_ExecutionParameters* params, unsigned int numGObjects,	
		const size_t numEventsBeforeExecution, cl_event* completeBeforeExecution, cl_event* execution)
{
	RayBuffer* rBuffer = params->srcBuffer;

	//cl_mem texture = AssetManager_LookupTextureCLReference("Test");
	cl_mem texture = assetBuffer->textureArray;

	cl_int err = 0;
	
	//Set kernel arguments

	err = clSetKernelArg(
			members->sphereReflectionKernel, 
			0,
			sizeof(*params->sPosition),
			params->sPosition
			);

	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: sphereReflectionTexture");
	

	err = clSetKernelArg(
			members->sphereReflectionKernel,
			1,
			sizeof(*params->sDiffuse),
			params->sDiffuse
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: outColor");

	err = clSetKernelArg(
			members->sphereReflectionKernel,
			2,
			sizeof(*params->sNormal),
			params->sNormal
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: outNormal");

	err = clSetKernelArg(
			members->sphereReflectionKernel,
			3,
			sizeof(*params->sMaterial),
			params->sMaterial
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: outMaterial");

	err = clSetKernelArg(
			members->sphereReflectionKernel,
			4,
			sizeof(*params->sSpecular),
			params->sSpecular
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: outSpecular");

	
	err = clSetKernelArg(
			members->sphereReflectionKernel,
			5,
			sizeof(*params->sDepth),
			params->sDepth
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: outDepth");
	

	err = clSetKernelArg(
			members->sphereReflectionKernel, 
			6, 
			sizeof(rBuffer->textureRefs[RayBuffer_TextureType_POSITION]), 
			&rBuffer->textureRefs[RayBuffer_TextureType_POSITION]);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: positionTexture");

	err = clSetKernelArg(
			members->sphereReflectionKernel, 
			7, 
			sizeof(rBuffer->textureRefs[RayBuffer_TextureType_NORMAL]), 
			&rBuffer->textureRefs[RayBuffer_TextureType_NORMAL]);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: normalTexture");

	err = clSetKernelArg(
			members->sphereReflectionKernel,
			8,
			sizeof(texture),
			&texture);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: texture");

	err = clSetKernelArg(
			members->sphereReflectionKernel, 
			9, 
			sizeof(*params->spheres), 
			params->spheres);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: spheres");

	err = clSetKernelArg(
			members->sphereReflectionKernel,
			10,
			sizeof(*params->gObjects),
			params->gObjects);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelPRogram_ExecuteSphereKernel :: clSetKErnelArg :: sphereReflectionKernel :: gObjects");

	err = clSetKernelArg(
			members->sphereReflectionKernel,
			11,
			sizeof(*params->materials),
			params->materials);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: materials");

	err = clSetKernelArg(
			members->sphereReflectionKernel, 
			12, 
			sizeof(*params->cameraPosition), 
			params->cameraPosition);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: cameraPosition");

	err = clSetKernelArg(
			members->sphereReflectionKernel, 
			13, 
			sizeof(*params->sphereTransformations), 
			params->sphereTransformations);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: sphereTransformations");

	err = clSetKernelArg(
			members->sphereReflectionKernel, 
			14, 
			sizeof(cl_float) * numGObjects, 
			NULL);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: reflectionReduce");

	err = clSetKernelArg(
			members->sphereReflectionKernel, 
			15, 
			sizeof(size_t) * (numGObjects), 
			NULL);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: indexReduce");


	//Execute KErnel
	const size_t numGlobalSpheres[3] = {rBuffer->textureWidth, rBuffer->textureHeight, numGObjects};
	const size_t numLocalSpheres[3] = {1, 1, numGObjects};

	err = clEnqueueNDRangeKernel
	(
		buffer->clQueue,
		members->sphereReflectionKernel,
		3,
		NULL,
		&numGlobalSpheres[0],
		&numLocalSpheres[0],
		numEventsBeforeExecution,
		completeBeforeExecution,
		&execution[0]
	);

	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteSphereKernel :: clEnqueueNDRangeKernel :: sphereReflectionKernel");


}

static void RayTracerReflectionKernelProgram_ExecuteAABBKernel(
		KernelBuffer* buffer, struct RayTracerReflectionKernelProgram_Members* members, 
		struct RayTracerReflectionKernelProgram_ExecutionParameters* params, unsigned char numGObjects, 
		const size_t numEventsBeforeExecution, cl_event* completeBeforeExecution, cl_event* execution)
{

	RayBuffer* rBuffer = params->srcBuffer;

	cl_int err = 0;



	cl_mem texture = assetBuffer->textureArray;;


	//Set kernel arguments
	err = clSetKernelArg(
			members->aabbReflectionKernel, 
			0,
			sizeof(*params->aPosition), 
			params->aPosition
			);

	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: aabbReflectionTexture");

	err = clSetKernelArg(
			members->aabbReflectionKernel,
			1,
			sizeof(*params->aDiffuse),
			params->aDiffuse
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: outColor");

	err = clSetKernelArg(
			members->aabbReflectionKernel,
			2,
			sizeof(*params->aNormal),
			params->aNormal
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: outNormal");

	err = clSetKernelArg(
			members->aabbReflectionKernel,
			3,
			sizeof(*params->aMaterial),
			params->aMaterial
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: outMaterial");

	err = clSetKernelArg(
			members->aabbReflectionKernel,
			4,
			sizeof(*params->aSpecular),
			params->aSpecular
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: outSpecular");

	
	err = clSetKernelArg(
			members->aabbReflectionKernel,
			5,
			sizeof(*params->aDepth),
			params->aDepth);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: outDepth");
	

	err = clSetKernelArg(
			members->aabbReflectionKernel, 
			6, 
			sizeof(rBuffer->textureRefs[RayBuffer_TextureType_POSITION]), 
			&rBuffer->textureRefs[RayBuffer_TextureType_POSITION]);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: positionTexture");

	err = clSetKernelArg(
			members->aabbReflectionKernel, 
			7, 
			sizeof(rBuffer->textureRefs[RayBuffer_TextureType_NORMAL]), 
			&rBuffer->textureRefs[RayBuffer_TextureType_NORMAL]);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: normalTexture");

	err = clSetKernelArg(
			members->aabbReflectionKernel,
			8,
			sizeof(texture),
			&texture);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: texture");

	err = clSetKernelArg(
			members->aabbReflectionKernel, 
			9, 
			sizeof(*params->aabbs), 
			params->aabbs);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: aabbs");

	err = clSetKernelArg(
			members->aabbReflectionKernel, 
			10, 
			sizeof(*params->gObjects), 
			params->gObjects);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: gObjects");

	err = clSetKernelArg(
			members->aabbReflectionKernel, 
			11, 
			sizeof(*params->materials), 
			params->materials);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: materials");


	err = clSetKernelArg(
			members->aabbReflectionKernel, 
			12, 
			sizeof(*params->cameraPosition), 
			params->cameraPosition);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: cameraPosition");

	err = clSetKernelArg(
			members->aabbReflectionKernel, 
			13, 
			sizeof(cl_float) * numGObjects, 
			NULL);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: reflectionReduce");

	err = clSetKernelArg(
			members->aabbReflectionKernel, 
			14, 
			sizeof(size_t) * (numGObjects), 
			NULL);
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: aabbReflectionKernel :: indexReduce");


	//Execute KErnel
	const size_t numGlobalAABBs[3] = {rBuffer->textureWidth, rBuffer->textureHeight, numGObjects};
	const size_t numLocalAABBs[3] = {1, 1, numGObjects};

	err = clEnqueueNDRangeKernel
	(
		buffer->clQueue,
		members->aabbReflectionKernel,
		3,
		NULL,
		&numGlobalAABBs[0],
		&numLocalAABBs[0],
		numEventsBeforeExecution,
		completeBeforeExecution,
		&execution[0]
	);

	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clEnqueueNDRangeKernel :: aabbReflectionKernel");


}

static void RayTracerReflectionKernelProgram_ExecuteReductionKernel(
		KernelBuffer* buffer, struct RayTracerReflectionKernelProgram_Members* members, 
		struct RayTracerReflectionKernelProgram_ExecutionParameters* params, 
		const size_t numEventsBeforeExecution, cl_event* completeBeforeExecution, cl_event* execution
		)
{
	RayBuffer* destBuffer = params->destBuffer;



	cl_int err = 0;

	//Set the kernel arguments

	//Final textures
	err = clSetKernelArg(
			members->reduceTexturesKernel, 
			0,
			sizeof(destBuffer->textureRefs[RayBuffer_TextureType_POSITION]),
			&destBuffer->textureRefs[RayBuffer_TextureType_POSITION]
			);

	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteReductionKernel :: clSetKernelArg :: reduceTexturesKernel :: sphereReflectionTexture");

	err = clSetKernelArg(
			members->reduceTexturesKernel,
			1,
			sizeof(destBuffer->textureRefs[RayBuffer_TextureType_DIFFUSE]),
			&destBuffer->textureRefs[RayBuffer_TextureType_DIFFUSE]);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteReductionKernel :: clSetKernelArg :: reduceTexturesKernel :: outColor");

	err = clSetKernelArg(
			members->reduceTexturesKernel,
			2,
			sizeof(destBuffer->textureRefs[RayBuffer_TextureType_NORMAL]),
			&destBuffer->textureRefs[RayBuffer_TextureType_NORMAL]);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteReductionKernel :: clSetKernelArg :: reduceTexturesKernel :: outNormal");

	err = clSetKernelArg(
			members->reduceTexturesKernel,
			3,
			sizeof(destBuffer->textureRefs[RayBuffer_TextureType_LOCALMATERIAL]),
			&destBuffer->textureRefs[RayBuffer_TextureType_LOCALMATERIAL]);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteReductionKernel :: clSetKernelArg :: reduceTexturesKernel :: outMaterial");

	err = clSetKernelArg(
			members->reduceTexturesKernel,
			4,
			sizeof(destBuffer->textureRefs[RayBuffer_TextureType_SPECULAR]),
			&destBuffer->textureRefs[RayBuffer_TextureType_SPECULAR]);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteReductionKernel :: clSetKernelArg :: reduceTexturesKernel :: outSpecular");




	//Sphere Textures
	err = clSetKernelArg(
			members->reduceTexturesKernel, 
			5,
			sizeof(*params->sPosition),
			params->sPosition
			);

	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteReductionKernel :: clSetKernelArg :: reduceTexturesKernel :: sphereReflectionTexture");

	err = clSetKernelArg(
			members->reduceTexturesKernel,
			6,
			sizeof(*params->sDiffuse),
			params->sDiffuse
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteReductionKernel :: clSetKernelArg :: reduceTexturesKernel :: outColor");

	err = clSetKernelArg(
			members->reduceTexturesKernel,
			7,
			sizeof(*params->sNormal),
			params->sNormal
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteReductionKernel :: clSetKernelArg :: reduceTexturesKernel :: outNormal");

	err = clSetKernelArg(
			members->reduceTexturesKernel,
			8,
			sizeof(*params->sMaterial),
			params->sMaterial
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteReductionKernel :: clSetKernelArg :: reduceTexturesKernel :: outMaterial");

	err = clSetKernelArg(
			members->reduceTexturesKernel,
			9,
			sizeof(params->sSpecular),
			params->sSpecular
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteReductionKernel :: clSetKernelArg :: reduceTexturesKernel :: outSpecular");

	
	err = clSetKernelArg(
			members->reduceTexturesKernel,
			10,
			sizeof(params->sDepth),
			params->sDepth
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteReductionKernel :: clSetKernelArg :: reduceTexturesKernel :: outDepth");




	//AABB Textures
	err = clSetKernelArg(
			members->reduceTexturesKernel, 
			11,
			sizeof(*params->aPosition), 
			params->aPosition);

	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: reduceTexturesKernel :: aabbReflectionTexture");

	err = clSetKernelArg(
			members->reduceTexturesKernel,
			12,
			sizeof(*params->aDiffuse),
			params->aDiffuse);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: reduceTexturesKernel :: outColor");

	err = clSetKernelArg(
			members->reduceTexturesKernel,
			13,
			sizeof(*params->aNormal),
			params->aNormal);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: reduceTexturesKernel :: outNormal");

	err = clSetKernelArg(
			members->reduceTexturesKernel,
			14,
			sizeof(*params->aMaterial),
			params->aMaterial);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: reduceTexturesKernel :: outMaterial");

	err = clSetKernelArg(
			members->reduceTexturesKernel,
			15,
			sizeof(*params->aSpecular),
			params->aSpecular);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: reduceTexturesKernel :: outSpecular");

	
	err = clSetKernelArg(
			members->reduceTexturesKernel,
			16,
			sizeof(*params->aDepth),
			params->aDepth);
	
	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clSetKernelArg :: reduceTexturesKernel :: outDepth");
	

	//Execute KErnel
	const size_t numGlobalReduction[3] = {destBuffer->textureWidth, destBuffer->textureHeight, 1};
	const size_t numLocalReduction[3] = {1, 1, 1};

	err = clEnqueueNDRangeKernel
	(
		buffer->clQueue,
		members->reduceTexturesKernel,
		3,
		NULL,
		&numGlobalReduction[0],
		&numLocalReduction[0],
		numEventsBeforeExecution,
		completeBeforeExecution,
		&execution[0]
	);

	KernelManager_CheckCLErrors(err, "RayTracerReflectionKernelProgram_ExecuteAABBKernel :: clEnqueueNDRangeKernel :: reduceTexturesKernel");



}

///
//Executes a RayTracerReflectionKernelProgram
//
//Parameters:
//	prog: A pointer to the RayTracerReflectionKernelProgram to execute
//	buffer: A pointer to the active KernelBuffer containing the context and device on which to execute
//	params: A pointer to a struct containing parameters for Reflection KErnal Program execution
static void RayTracerReflectionKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, struct RayTracerReflectionKernelProgram_ExecutionParameters* params)
{
	RayTracerReflectionKernelProgram_Members* members = prog->members;

	RenderingBuffer* renderingBuffer = RenderingManager_GetRenderingBuffer();
	Camera* cam = renderingBuffer->camera;

	Vector cameraPosition;
	Vector_INIT_ON_STACK(cameraPosition, 3);

	//TODO: Write a function for this.. seriously..
	//Get the position of the camera	
	Matrix_SliceColumn(&cameraPosition, cam->translationMatrix, 3, 0, 3);
	Vector_Scale(&cameraPosition, -1.0f);

	float* transformations = collisionBuffer->sphereTransformations->pool->data;

	unsigned int numGObjects = objectBuffer->objectPool->pool->size;

	const size_t numExecution = 2;
	cl_event execution[numExecution];

	RayTracerReflectionKernelProgram_ExecuteSphereKernel(buffer, members, params, numGObjects, 0, NULL, &execution[0]);
	RayTracerReflectionKernelProgram_ExecuteAABBKernel(buffer, members, params, numGObjects, 0, NULL, &execution[1]);

	
	cl_event reduction;
	RayTracerReflectionKernelProgram_ExecuteReductionKernel(buffer, members, params, numExecution, execution, &reduction);

	//Enqueue Sphere execution
	clWaitForEvents(1, &reduction);
	clFinish(buffer->clQueue);

}
