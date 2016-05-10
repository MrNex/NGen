#include "RayTracerPointShadowKernelProgram.h"

#include <stdlib.h>
#include <CL/cl_gl.h>

#include "../Manager/KernelManager.h"
#include "../Manager/RenderingManager.h"
#include "../Manager/CollisionManager.h"
#include "../Manager/EnvironmentManager.h"

typedef struct RayTracerPointShadowKernelProgram_Members
{
	cl_mem sphereShadowTexture;
	cl_mem aabbShadowTexture;
	cl_mem lightPosition;

	cl_kernel pointSphereShadowKernel;
	cl_kernel pointAABBShadowKernel;

	cl_kernel reduceShadowTextures;
} RayTracerPointShadowKernelProgram_Members;

///
//Allocates memory for the members of this kernel program
//
//Returns:
//	An uninitialized pointer to memory for RayTracerPointShadowKernelProgram members
static RayTracerPointShadowKernelProgram_Members* RayTracerPointShadowKernelProgram_AllocateMembers(void);

///
//Initializes the kernel program's members
//
//Parameters:
//	prog: A pointer to the kernel program to initialize the members of
//	buffer: A pointer to The currently active Kernel buffer
static void RayTracerPointShadowKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer);

///
//Frees the members of a RayTracerPointShadow kernel program
//
//Parameters:
//	prog: A pointer to the RayTracerPointShadow KernelProgram having it's members freed
static void RayTracerPointShadowKernelProgram_FreeMembers(KernelProgram* prog);

///
//Executes a RayTracerPointShadowKernelProgram
//
//Parameters:
//	prog: A pointer to the RayTracerPointShadowKernelProgram to execute
//	buffer: A pointer to the active KernelBuffer containing the context and device on which to execute
//	params: A pointer to a struct containing pointers to:
//		The ray buffer containing data needed for execution
//		The point light to calculate shadows for
static void RayTracerPointShadowKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, struct RayTracerPointShadowKernelProgram_ExecutionParameters* params);

///
//Initializes a rayTracerPointShadowKernelProgram
//
//Parameters:
//	prog: A pointer to an uninitialized  kernel program to initialize as a RayTracerPointShadowRayKernelProgram
//	buffer: A pointer to the kernel buffer containing the context and device on which to build this kernel
void RayTracerPointShadowKernelProgram_Initialize(KernelProgram* prog, struct KernelBuffer* buffer)
{
	KernelProgram_InitializeWithOptions(prog, "Kernel/RayTracerShadowRayKernelProgram.cl", buffer, "-D LIGHTTYPE=2");

	prog->members = RayTracerPointShadowKernelProgram_AllocateMembers();
	RayTracerPointShadowKernelProgram_InitializeMembers(prog, buffer);

	prog->FreeMembers = RayTracerPointShadowKernelProgram_FreeMembers;
	prog->Execute = (KernelProgram_ExecuteFunc)RayTracerPointShadowKernelProgram_Execute;
}

///
//Allocates memory for the members of this kernel program
//
//Returns:
//	An uninitialized pointer to memory for RayTracerPointShadowKernelProgram members
static RayTracerPointShadowKernelProgram_Members* RayTracerPointShadowKernelProgram_AllocateMembers(void)
{
	return malloc(sizeof(RayTracerPointShadowKernelProgram_Members));
}

///
//Initializes the kernel program's members
//
//Parameters:
//	prog: A pointer to the kernel program to initialize the members of
//	buffer: A pointer to The currently active Kernel buffer
static void RayTracerPointShadowKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer)
{
	RayTracerPointShadowKernelProgram_Members* members = prog->members;

	EnvironmentBuffer* envBuffer = EnvironmentManager_GetEnvironmentBuffer();

	cl_int clError;

	cl_image_format format;
	format.image_channel_order = CL_R;
	format.image_channel_data_type = CL_SNORM_INT8;

	cl_image_desc properties;
	properties.image_type = CL_MEM_OBJECT_IMAGE2D;
	properties.image_width= envBuffer->windowWidth;
	properties.image_height = envBuffer->windowHeight;
	properties.image_depth = 0;
	properties.image_array_size = 0;
	properties.image_row_pitch = 0;
	properties.image_slice_pitch = 0;
	properties.num_mip_levels = 0;
	properties.num_samples = 0;
	properties.mem_object = NULL;

	members->sphereShadowTexture = clCreateImage
	(
		buffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&clError
	);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_InitializeMembers :: clCreateImage :: sphereShadowTexture");

	members->aabbShadowTexture = clCreateImage
	(
		buffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&clError
	);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_InitializeMembers :: clCreateImage :: aabbShadowTexture");


	members->lightPosition = clCreateBuffer
	(
		buffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(float) * 3,
		NULL,
		&clError
	);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_InitializeMembers :: clCreateBuffer :: light");

	members->pointSphereShadowKernel = clCreateKernel(prog->clProgram, "ComputeDirectionalSphereShadowTexture", &clError);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_InitializeMembers :: clCreateKernel :: ComputeDirectionalSphereShadowTexture");

	members->pointAABBShadowKernel = clCreateKernel(prog->clProgram, "ComputeDirectionalAABBShadowTexture", &clError);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_InitializeMembers :: clCreateKernel :: ComputeDirectionalAABBShadowTexture");

	//Todo:Share kernel with directional kernel program
	members->reduceShadowTextures = clCreateKernel(prog->clProgram, "ReduceShadowTextures", &clError);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_InitializeMembers :: clCreateKernel :: ReduceShadowTextures");
}

///
//Frees the members of a RayTracerPointShadow kernel program
//
//Parameters:
//	prog: A pointer to the RayTracerPointShadow KernelProgram having it's members freed
static void RayTracerPointShadowKernelProgram_FreeMembers(KernelProgram* prog)
{
	RayTracerPointShadowKernelProgram_Members* members = prog->members;

	cl_int clError;
	clError = clReleaseMemObject(members->sphereShadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_FreeMembers :: clReleaseMemObject :: sphereShadowTexture");

	clError = clReleaseMemObject(members->aabbShadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_FreeMembers :: clReleaseMemObject :: aabbShadowTexture");

	clError = clReleaseMemObject(members->lightPosition);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_FreeMembers :: clReleaseMemObject :: lightDirection");

	free(members);
	prog->members = NULL;
}

///
//Executes a RayTracerPointShadowKernelProgram
//
//Parameters:
//	prog: A pointer to the RayTracerPointShadowKernelProgram to execute
//	buffer: A pointer to the active KernelBuffer containing the context and device on which to execute
//	params: A pointer to a struct containing pointers to:
//		The ray buffer containing data needed for execution
//		The point light to calculate shadows for
static void RayTracerPointShadowKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, struct RayTracerPointShadowKernelProgram_ExecutionParameters* params)
{
	//Gather necessary data
	RayTracerPointShadowKernelProgram_Members* members = prog->members;

	GObject* light = params->light;
	RayBuffer* rBuffer = params->buffer;

	Vector lightPosition;
	Vector_INIT_ON_STACK(lightPosition, 3);

	FrameOfReference_GetTransformedVector(&lightPosition, light->frameOfReference, light->light->position);

	unsigned char numSpheres, numAABBs;
	numSpheres = collisionBuffer->worldSphereData->pool->size;
	numAABBs = collisionBuffer->worldAABBData->pool->size;

	cl_int clError;

	const unsigned int numEventsBeforeExecution = 1;
	cl_event completeBeforeExecution[numEventsBeforeExecution];

	//Update non OGL device memory
	clError = clEnqueueWriteBuffer
	(
		buffer->clQueue,
		members->lightPosition,
		CL_FALSE,
		0,
		sizeof(float) * 3,
		lightPosition.components,
		0,
		NULL,
		&completeBeforeExecution[0]
	);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_Execute :: clEnqueueWriteBuffer :: lightDirection");

	//AABBs
	//Set kernel arguments
	clError = clSetKernelArg(
			members->pointAABBShadowKernel, 
			0, 
			sizeof(members->aabbShadowTexture), 
			&members->aabbShadowTexture);

	KernelManager_CheckCLErrors(clError, "RayTracerPointAABBShadowKernelProgram_Execute :: clSetKernelArg :: pointAABBShadowKernel :: shadowTexture");

	clError = clSetKernelArg(
			members->pointAABBShadowKernel, 
			1, 
			sizeof(rBuffer->textureRefs[RayBuffer_TextureType_POSITION]), 
			&rBuffer->textureRefs[RayBuffer_TextureType_POSITION]);

	KernelManager_CheckCLErrors(clError, "RayTracerPointAABBShadowKernelProgram_Execute :: clSetKernelArg :: pointAABBShadowKernel :: positionTexture");

	clError = clSetKernelArg(
			members->pointAABBShadowKernel, 
			2, 
			sizeof(members->lightPosition), 
			&members->lightPosition);

	KernelManager_CheckCLErrors(clError, "RayTracerPointAABBShadowKernelProgram_Execute :: clSetKernelArg :: pointAABBShadowKernel :: lightDirection");

	clError = clSetKernelArg(
			members->pointAABBShadowKernel, 
			3, 
			sizeof(*params->aabbs), 
			params->aabbs);

	KernelManager_CheckCLErrors(clError, "RayTracerPointAABBShadowKernelProgram_Execute :: clSetKernelArg :: pointAABBShadowKernel :: aabbs");

	clError = clSetKernelArg(
			members->pointAABBShadowKernel, 
			4, 
			sizeof(cl_bool) * numAABBs, 
			NULL);

	KernelManager_CheckCLErrors(clError, "RayTracerPointAABBShadowKernelProgram_Execute :: clSetKernelArg :: pointAABBShadowKernel :: shadowReduce");

	//Enqueue AABB execution
	const size_t numGlobalAABBs[3] = { rBuffer->textureWidth, rBuffer->textureHeight, numAABBs };
	const size_t numLocalAABBs[3] = { 1, 1, numAABBs };

	const size_t numExecution = 2;
	cl_event execution[numExecution];


	clError = clEnqueueNDRangeKernel
	(
		buffer->clQueue,
		members->pointAABBShadowKernel,
		3,
		NULL,
		&numGlobalAABBs[0],
		&numLocalAABBs[0],
		numEventsBeforeExecution,
		completeBeforeExecution,
		&execution[0]

	);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_Execute :: clEnqueueNDRangeKernel :: pointAABBShadowKernel");

	//Spheres
	//Set kernel arguments
	clError = clSetKernelArg(
			members->pointSphereShadowKernel, 
			0, 
			sizeof(members->sphereShadowTexture), 
			&members->sphereShadowTexture);

	KernelManager_CheckCLErrors(clError, "RayTracerPointSphereShadowKernelProgram_Execute :: clSetKernelArg :: pointSphereShadowKernel :: shadowTexture");

	clError = clSetKernelArg(
			members->pointSphereShadowKernel, 
			1, 
			sizeof(rBuffer->textureRefs[RayBuffer_TextureType_POSITION]), 
			&rBuffer->textureRefs[RayBuffer_TextureType_POSITION]);

	KernelManager_CheckCLErrors(clError, "RayTracerPointSphereShadowKernelProgram_Execute :: clSetKernelArg :: pointSphereShadowKernel :: positionTexture");

	clError = clSetKernelArg(
			members->pointSphereShadowKernel, 
			2, 
			sizeof(members->lightPosition), 
			&members->lightPosition);

	KernelManager_CheckCLErrors(clError, "RayTracerPointSphereShadowKernelProgram_Execute :: clSetKernelArg :: pointSphereShadowKernel :: lightDirection");

	clError = clSetKernelArg(
			members->pointSphereShadowKernel, 
			3, 
			sizeof(*params->spheres), 
			params->spheres);

	KernelManager_CheckCLErrors(clError, "RayTracerPointSphereShadowKernelProgram_Execute :: clSetKernelArg :: pointSphereShadowKernel :: sphere");

	clError = clSetKernelArg(
			members->pointSphereShadowKernel, 
			4, 
			sizeof(cl_bool) * numSpheres, 
			NULL);

	KernelManager_CheckCLErrors(clError, "RayTracerPointSphereShadowKernelProgram_Execute :: clSetKernelArg :: pointSphereShadowKernel :: shadowReduce");

	//Enqueue Execution
	const size_t numGlobalSpheres[3] = { rBuffer->textureWidth, rBuffer->textureHeight, numSpheres };
	const size_t numLocalSpheres[3] = { 1, 1, numSpheres };

	
	clError = clEnqueueNDRangeKernel
	(
	 	buffer->clQueue,
		members->pointSphereShadowKernel,
		3,
		NULL,
		&numGlobalSpheres[0],
		&numLocalSpheres[0],
		numEventsBeforeExecution,
		completeBeforeExecution,
		&execution[1]
	);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_Execute :: clEnqueueNDRangeKernel :: ComputeDirectionalSphereShadowTexture");

	//Start reduction
	
	clError = clSetKernelArg(members->reduceShadowTextures, 0, sizeof(rBuffer->textureRefs[RayBuffer_TextureType_SHADOW]), &rBuffer->textureRefs[RayBuffer_TextureType_SHADOW]);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_Execute :: clSetKernelArg :: reduceShadowTextures :: shadowTexture");
	
	clError = clSetKernelArg(members->reduceShadowTextures, 1, sizeof(members->sphereShadowTexture), &members->sphereShadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_Execute :: clSetKernelArg :: reduceShadowTextures :: sphereShadowTexture");
	
	clError = clSetKernelArg(members->reduceShadowTextures, 2, sizeof(members->aabbShadowTexture), &members->aabbShadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_Execute :: clSetKernelArg :: reduceShadowTextures :: aabbShadowTexture");

	const size_t numGlobalReduction[3] = { rBuffer->textureWidth, rBuffer->textureHeight, 1 };
	const size_t numLocalReduction[3] = { 1, 1, 1 };

	cl_event reduction;

	//Execute kernel
	clError = clEnqueueNDRangeKernel
	(
		buffer->clQueue,
		members->reduceShadowTextures,
		3,
		NULL,
		&numGlobalReduction[0],
		&numLocalReduction[0],
		numExecution,
		execution,
		&reduction
	);

	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_Execute :: clEnqueueNDRangeKernel :: reduceShadowTextures");

	clWaitForEvents(1, &reduction);
	clFinish(buffer->clQueue);


}
