#include "RayTracerTransmissionKernelProgram.h"

#include <stdlib.h>
#include <CL/cl_gl.h>

#include "../Manager/KernelManager.h"
#include "../Manager/RenderingManager.h"
#include "../Manager/CollisionManager.h"
#include "../Manager/AssetManager.h"

typedef struct RayTracerTransmissionKernelProgram_Members
{
	cl_kernel sphereTransmissionKernel;
	cl_kernel aabbTransmissionKernel;
	cl_kernel reduceTexturesKernel;
} RayTracerTransmissionKernelProgram_Members;

///
//Allocates a set of internal members for the transmission kernel
//
//Returns:
//	Void pointer to struct containing uninitialized members
static RayTracerTransmissionKernelProgram_Members* RayTracerTransmissionKernelProgram_AllocateMembers();

///
//Initializes the members of a transmission kernel
//
//Parameters:
//	prog: A pointer to the raytracerTransmissionKernelProgram to initialize the members of
//	buffer: A pointer to the active kernel buffer containing the target context
static void RayTracerTransmissionKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer);

///
//Frees the internal members of the transmission kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to free the internal members of
static void RayTracerTransmissionKernelProgram_FreeMembers(KernelProgram* prog);

///
//Executes the transmission kernel program
//
//Parameters:
//	prog: A pointer to the transmission kernel program to execute
//	buffer: A pointer to the kernel buffer containing the target context
//	params: The execution parameters specifying the memory with which to execute the kernel
static void RayTracerTransmissionKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, struct RayTracerTransmissionKernelProgram_ExecutionParameters* params);

///
//Initializes a RayTracerTransmissionKernelProgram
//
//Parameters:
//	prog: A pointer to an uninitialized kernel program to initialize as a ray tracer transmission kernel program
//	buffer: A pointer to the kernel buffer containing the context and device on which to build the kernel
void RayTracerTransmissionKernelProgram_Initialize(KernelProgram* prog, struct KernelBuffer* buffer)
{
	KernelProgram_Initialize(prog, "Kernel/RayTracerTransmissionRayKernelProgram.cl", buffer);

	prog->members = RayTracerTransmissionKernelProgram_AllocateMembers();
	RayTracerTransmissionKernelProgram_InitializeMembers(prog, buffer);

	prog->FreeMembers = RayTracerTransmissionKernelProgram_FreeMembers;
	prog->Execute = (KernelProgram_ExecuteFunc)RayTracerTransmissionKernelProgram_Execute;
}

///
//Allocates a set of internal members for the transmission kernel
//
//Returns:
//	Void pointer to struct containing uninitialized members
static RayTracerTransmissionKernelProgram_Members* RayTracerTransmissionKernelProgram_AllocateMembers()
{
	return malloc(sizeof(RayTracerTransmissionKernelProgram_Members));
}

///
//Initializes the members of a transmission kernel
//
//Parameters:
//	prog: A pointer to the raytracerTransmissionKernelProgram to initialize the members of
//	buffer: A pointer to the active kernel buffer containing the target context
static void RayTracerTransmissionKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer)
{
	RayTracerTransmissionKernelProgram_Members* members = prog->members;

	cl_int err;

	
	members->sphereTransmissionKernel = clCreateKernel(prog->clProgram, "ComputeSphereTransmissionTextures", &err);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_InitializeMembers :: clCreateKernel :: sphereTransmissionKernel");
	//members->aabbTransmissionKernel = clCreateKernel(prog->clProgram, "ComputeAABBTransmissionTextures", &err);
	//KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_InitializeMembers :: clCreateKernel :: aabbTransmissionKernel");
	//members->reduceTexturesKernel = clCreateKernel(prog->clProgram, "ReduceTransmissionTextures", &err);
	//KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_InitializeMembers :: clCreateKernel :: reduceTransmissionTexturesKernel");
}

///
//Frees the internal members of the transmission kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to free the internal members of
static void RayTracerTransmissionKernelProgram_FreeMembers(KernelProgram* prog)
{
	RayTracerTransmissionKernelProgram_Members* members = prog->members;
	free(members);
	prog->members = NULL;
}

static void RayTracerTransmissionKernelProgram_ExecuteSphereKernel(
		KernelBuffer* buffer, struct RayTracerTransmissionKernelProgram_Members* members,
		struct RayTracerTransmissionKernelProgram_ExecutionParameters* params, unsigned int numGObjects,
		const size_t numEventsBeforeExecution, cl_event* completeBeforeExecution, cl_event* execution)
{
	RayBuffer* rBuffer = params->srcBuffer;

	//cl_mem texture = AssetManager_LookupTextureCLReference("Test");
	cl_mem texture = assetBuffer->textureArray;

	cl_int err = 0;
	
	//Set kernel arguments
	/*
	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			0,
			sizeof(*params->sPosition),
			params->sPosition
			);

	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTransmissionKernel :: sPosition");
	

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			1,
			sizeof(*params->sDiffuse),
			params->sDiffuse
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTranmissionKernel :: sColor");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			2,
			sizeof(*params->sNormal),
			params->sNormal
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTransmissionKernel :: sNormal");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			3,
			sizeof(*params->sMaterial),
			params->sMaterial
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTransmissionKernel :: sMaterial");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			4,
			sizeof(*params->sSpecular),
			params->sSpecular
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTransmissionKernel :: sSpecular");

	
	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			5,
			sizeof(*params->sDepth),
			params->sDepth
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTransmissionKernel :: sDepth");
	

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			6, 
			sizeof(rBuffer->textureRefs[RayBuffer_TextureType_POSITION]), 
			&rBuffer->textureRefs[RayBuffer_TextureType_POSITION]);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTranmissionKernel :: positionTexture");

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			7, 
			sizeof(rBuffer->textureRefs[RayBuffer_TextureType_NORMAL]), 
			&rBuffer->textureRefs[RayBuffer_TextureType_NORMAL]);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTransmissionKernel :: normalTexture");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			8,
			sizeof(texture),
			&texture);
	
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: texture");

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			9, 
			sizeof(*params->spheres), 
			params->spheres);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: spheres");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			10,
			sizeof(*params->aabbs),
			params->aabbs);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: aabbs");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			11,
			sizeof(*params->gObjects),
			params->gObjects);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelPRogram_ExecuteSphereKernel :: clSetKErnelArg :: sphereReflectionKernel :: gObjects");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			12,
			sizeof(*params->materials),
			params->materials);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: materials");

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			13, 
			sizeof(*params->cameraPosition), 
			params->cameraPosition);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: cameraPosition");

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			14, 
			sizeof(*params->sphereTransformations), 
			params->sphereTransformations);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: sphereTransformations");

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			15, 
			sizeof(cl_float) * numGObjects, 
			NULL);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: reflectionReduce");

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			16, 
			sizeof(size_t) * (numGObjects), 
			NULL);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: indexReduce");
	*/

	RayBuffer* destBuffer = params->dstBuffer;
	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			0,
			sizeof(destBuffer->textureRefs[RayBuffer_TextureType_POSITION]),
			&destBuffer->textureRefs[RayBuffer_TextureType_POSITION]
			);

	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTransmissionKernel :: sPosition");
	

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			1,
			sizeof(destBuffer->textureRefs[RayBuffer_TextureType_DIFFUSE]),
			&destBuffer->textureRefs[RayBuffer_TextureType_DIFFUSE]
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTranmissionKernel :: sColor");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			2,
			sizeof(destBuffer->textureRefs[RayBuffer_TextureType_NORMAL]),
			&destBuffer->textureRefs[RayBuffer_TextureType_NORMAL]);

	
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTransmissionKernel :: sNormal");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			3,
			sizeof(destBuffer->textureRefs[RayBuffer_TextureType_LOCALMATERIAL]),
			&destBuffer->textureRefs[RayBuffer_TextureType_LOCALMATERIAL]);
		
	
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTransmissionKernel :: sMaterial");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			4,
			sizeof(destBuffer->textureRefs[RayBuffer_TextureType_SPECULAR]),
			&destBuffer->textureRefs[RayBuffer_TextureType_SPECULAR]);
	
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTransmissionKernel :: sSpecular");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			5,
			sizeof(destBuffer->textureRefs[RayBuffer_TextureType_GLOBALMATERIAL]),
			&destBuffer->textureRefs[RayBuffer_TextureType_GLOBALMATERIAL]
			);

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			6,
			sizeof(*params->sDepth),
			params->sDepth
			);
	
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTransmissionKernel :: sDepth");
	

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			7, 
			sizeof(rBuffer->textureRefs[RayBuffer_TextureType_POSITION]), 
			&rBuffer->textureRefs[RayBuffer_TextureType_POSITION]);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTranmissionKernel :: positionTexture");

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			8, 
			sizeof(rBuffer->textureRefs[RayBuffer_TextureType_NORMAL]), 
			&rBuffer->textureRefs[RayBuffer_TextureType_NORMAL]);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereTransmissionKernel :: normalTexture");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			9,
			sizeof(texture),
			&texture);
	
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: texture");

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			10, 
			sizeof(*params->spheres), 
			params->spheres);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: spheres");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			11,
			sizeof(*params->aabbs),
			params->aabbs);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: aabbs");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			12,
			sizeof(*params->gObjects),
			params->gObjects);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelPRogram_ExecuteSphereKernel :: clSetKErnelArg :: sphereReflectionKernel :: gObjects");

	err = clSetKernelArg(
			members->sphereTransmissionKernel,
			13,
			sizeof(*params->materials),
			params->materials);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: materials");

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			14, 
			sizeof(*params->cameraPosition), 
			params->cameraPosition);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: cameraPosition");

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			15, 
			sizeof(*params->sphereTransformations), 
			params->sphereTransformations);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: sphereTransformations");

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			16, 
			sizeof(cl_float) * numGObjects, 
			NULL);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: reflectionReduce");

	err = clSetKernelArg(
			members->sphereTransmissionKernel, 
			17, 
			sizeof(size_t) * (numGObjects), 
			NULL);
	KernelManager_CheckCLErrors(err, "RayTracerTransmissionKernelProgram_ExecuteSphereKernel :: clSetKernelArg :: sphereReflectionKernel :: indexReduce");

	//Execute KErnel
	const size_t numGlobalSpheres[3] = {rBuffer->textureWidth, rBuffer->textureHeight, numGObjects};
	const size_t numLocalSpheres[3] = {1, 1, numGObjects};

	err = clEnqueueNDRangeKernel
	(
		buffer->clQueue,
		members->sphereTransmissionKernel,
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

static void RayTracerTransmissionKernelProgram_ExecuteAABBKernel(
		KernelBuffer* buffer, struct RayTracerTransmissionKernelProgram_Members* members,
		struct RayTracerTransmissionKernelProgram_ExecutionParameters* params, unsigned int numGObjects,
		const size_t numEventsBeforeExecution, cl_event* completeBeforeExecution, cl_event* execution)
{
	//TODO
}

static void RayTracerTransmissionKernelProgram_ExecuteReductionKernel(
		KernelBuffer* buffer, struct RayTracerTransmissionKernelProgram_Members* members,
		struct RayTracerTransmissionKernelProgram_ExecutionParameters* params, unsigned int numGObjects,
		const size_t numEventsBeforeExecution, cl_event* completeBeforeExecution, cl_event* execution)
{
	//TODO
}

///
//Executes the transmission kernel program
//
//Parameters:
//	prog: A pointer to the transmission kernel program to execute
//	buffer: A pointer to the kernel buffer containing the target context
//	params: The execution parameters specifying the memory with which to execute the kernel
static void RayTracerTransmissionKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, struct RayTracerTransmissionKernelProgram_ExecutionParameters* params)
{
	RayTracerTransmissionKernelProgram_Members* members = prog->members;

	RenderingBuffer* renderBuffer = RenderingManager_GetRenderingBuffer();


	unsigned int numGObjects = objectBuffer->objectPool->pool->size;
	
	const size_t numExecution = 2;
	cl_event execution[numExecution];

	RayTracerTransmissionKernelProgram_ExecuteSphereKernel(buffer, members, params, numGObjects, 0, NULL, &execution[0]);
	//RayTracerTransmissionKernelProgram_ExecuteAABBKernel(buffer, members, params, numGObjects, 0, NULL, &execution[1]);

	cl_event reduction;
	RayTracerTransmissionKernelProgram_ExecuteReductionKernel(buffer, members, params, numGObjects, 0, NULL, &reduction);

	//cl_event cleanup;
	clWaitForEvents(1, &reduction);
	clFinish(buffer->clQueue);
}
