#include "RayTracerDirectionalShadowKernelProgram.h"

#include <stdlib.h>
#include <CL/cl_gl.h>

#include "../Manager/KernelManager.h"
#include "../Manager/RenderingManager.h"
#include "../Manager/CollisionManager.h"
#include "../Manager/EnvironmentManager.h"

#include "../Math/Vector.h"

typedef struct RayTracerDirectionalShadowKernelProgram_Members
{
	cl_mem sphereShadowTexture;
	cl_mem aabbShadowTexture;
	cl_mem lightDirection;

	cl_kernel directionalSphereShadowKernel;
	cl_kernel directionalAABBShadowKernel;

	cl_kernel reduceShadowTextures;
} RayTracerDirectionalShadowKernelProgram_Members;

///
//Allocates memory for the members of this kernel program
//
//Returns:
//	An uninitialized pointer to memory for RayTracerDirectionalShadowKernelProgram members
static RayTracerDirectionalShadowKernelProgram_Members* RayTracerDirectionalShadowKernelProgram_AllocateMembers(void);

///
//Initializes the kernel program's members
//
//Parameters:
//	prog: A pointer to the kernel program to initialize the members of
//	buffer: A pointer to The currently active Kernel buffer
static void RayTracerDirectionalShadowKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer);

///
//Frees the members of a RayTracerDirectionalShadow kernel program
//
//Parameters:
//	prog: A pointer to the RayTracerDirectionalShadow KernelProgram having it's members freed
static void RayTracerDirectionalShadowKernelProgram_FreeMembers(KernelProgram* prog);

///
//Executes a RayTracerDirectionalShadowKernelProgram
//
//Parameters:
//	prog: A pointer to the RayTracerDirectionalShadowKernelProgram to execute
//	buffer: A pointer to the active KernelBuffer containing the context and device on which to execute
//	params: A pointer to the struct containing data needed for execution
static void RayTracerDirectionalShadowKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, struct RayTracerDirectionalShadowKernelProgram_ExecutionParameters* params);

///
//Initializes a RayTracerDirectionalShadow Kernel Program
//
//Parameters:
//	prog: A pointer to an uninitialized Kernel Progral to initialize as a RayTracerDirectionalShadow Kernel Program
//	buffer: A pointer to the KernelBuffer containing the context and device on which to build the kernel
void RayTracerDirectionalShadowKernelProgram_Initialize(KernelProgram* prog, KernelBuffer* buffer)
{
	KernelProgram_Initialize(prog, "Kernel/RayTracerShadowRayKernelProgram.cl", buffer);
	
	prog->members = RayTracerDirectionalShadowKernelProgram_AllocateMembers();
	RayTracerDirectionalShadowKernelProgram_InitializeMembers(prog, buffer);

	prog->FreeMembers = RayTracerDirectionalShadowKernelProgram_FreeMembers;
	prog->Execute = (KernelProgram_ExecuteFunc)RayTracerDirectionalShadowKernelProgram_Execute;
}

///
//Allocates memory for the members of this kernel program
//
//Returns:
//	An uninitialized pointer to memory for RayTracerDirectionalShadowKernelProgram members
static RayTracerDirectionalShadowKernelProgram_Members* RayTracerDirectionalShadowKernelProgram_AllocateMembers(void)
{
	return malloc(sizeof(RayTracerDirectionalShadowKernelProgram_Members));
}

///
//Initializes the kernel program's members
//
//Parameters:
//	prog: A pointer to the kernel program to initialize the members of
//	buffer: A pointer to The currently active Kernel buffer
static void RayTracerDirectionalShadowKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer)
{
	RayTracerDirectionalShadowKernelProgram_Members* members = prog->members;

	EnvironmentBuffer* envBuffer = EnvironmentManager_GetEnvironmentBuffer();

	cl_int clError;

	cl_image_format format;
	format.image_channel_order = CL_R;
	format.image_channel_data_type = CL_SNORM_INT8;

	cl_image_desc properties;
	properties.image_type = CL_MEM_OBJECT_IMAGE2D;
	properties.image_width = envBuffer->windowWidth;
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
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_InitializeMembers :: clCreateImage :: sphereShadowTexture");

	members->aabbShadowTexture = clCreateImage
	(
		buffer->clContext,
		CL_MEM_READ_WRITE | CL_MEM_HOST_NO_ACCESS,
		&format,
		&properties,
		NULL,
		&clError
	);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_InitializeMembers :: clCreateImage :: aabbShadowTexture");

	members->lightDirection = clCreateBuffer
	(
		buffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(float) * 3,
		NULL,
		&clError
	);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_InitializeMembers :: clCreateBuffer :: light");


	members->directionalSphereShadowKernel = clCreateKernel(prog->clProgram, "ComputeDirectionalSphereShadowTexture", &clError);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_InitializeMembers :: clCreateKernel :: ComputeDirectionalSphereShadowTexture");


	members->directionalAABBShadowKernel = clCreateKernel(prog->clProgram, "ComputeDirectionalAABBShadowTexture", &clError);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_InitializeMembers :: clCreateKernel :: computeDirectionalAABBShadowTexture");

	members->reduceShadowTextures = clCreateKernel(prog->clProgram, "ReduceShadowTextures", &clError);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_InitializeMembers :: clCreateKernel :: ReduceShadowTextures");

}

///
//Frees the members of a RayTracerDirectionalShadow kernel program
//
//Parameters:
//	prog: A pointer to the RayTracerDirectionalShadow KernelProgram having it's members freed
static void RayTracerDirectionalShadowKernelProgram_FreeMembers(KernelProgram* prog)
{

	RayTracerDirectionalShadowKernelProgram_Members* members = prog->members;

	cl_int clError;

	clError = clReleaseMemObject(members->sphereShadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_FreeMembers :: clReleaseMemObject :: sphereShadowTexture");

	clError = clReleaseMemObject(members->aabbShadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_FreeMembers :: clReleaseMemObject :: aabbShadowTexture");


	clError = clReleaseMemObject(members->lightDirection);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_FreeMembers :: clReleaseMemObject :: lightDirection");

	free(members);
	prog->members = NULL;
}

///
//Executes a RayTracerDirectionalShadowKernelProgram
//
//Parameters:
//	prog: A pointer to the RayTracerDirectionalShadowKernelProgram to execute
//	buffer: A pointer to the active KernelBuffer containing the context and device on which to execute
//	params: A pointer to the struct containing data needed for execution
static void RayTracerDirectionalShadowKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, struct RayTracerDirectionalShadowKernelProgram_ExecutionParameters* params)
{
	RayBuffer* rBuffer = params->srcBuffer;

	unsigned int numGObjects = objectBuffer->objectPool->pool->size;

	unsigned char numSpheres, numAABBs;
	numSpheres = collisionBuffer->worldSphereData->pool->size;
	numAABBs = collisionBuffer->worldAABBData->pool->size;

	RenderingBuffer* renderingBuffer = RenderingManager_GetRenderingBuffer();
	RayTracerDirectionalShadowKernelProgram_Members* members = prog->members;

	cl_int clError;

	const unsigned int numBeforeExecution = 1;
	cl_event completeBeforeExecution[numBeforeExecution];

	//Update non-ogl cl device memory
	clError = clEnqueueWriteBuffer
	(
		buffer->clQueue,
		members->lightDirection,
		CL_FALSE,
		0,
		sizeof(float) * 3,
		renderingBuffer->directionalLight->direction->components,
		0,
		NULL,
		&completeBeforeExecution[0]
	);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clEnqueueWriteBuffer :: lightDirection");

	///
	//Execution
	cl_event execution[2];


	///
	//Start AABB Shadow calculations
	//Set kernel arguments
	clError = clSetKernelArg(members->directionalAABBShadowKernel, 0, sizeof(members->aabbShadowTexture), &members->aabbShadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: aabbShadowTexture");

	clError = clSetKernelArg(members->directionalAABBShadowKernel, 1, sizeof(rBuffer->textureRefs[RayBuffer_TextureType_POSITION]), &rBuffer->textureRefs[RayBuffer_TextureType_POSITION]);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: directionalAABBShadowKernel :: positionTexture");

	clError = clSetKernelArg(members->directionalAABBShadowKernel, 2, sizeof(members->lightDirection), &members->lightDirection);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: directionalAABBShadowKernel :: lightDirection");

	clError = clSetKernelArg(members->directionalAABBShadowKernel, 3, sizeof(*params->gObjects), params->gObjects);		
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKErnelArg :: directionalAABBShadowKernel :: gObjects");
	
	clError = clSetKernelArg(members->directionalAABBShadowKernel, 4, sizeof(*params->materials), params->materials);		
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKErnelArg :: directionalAABBShadowKernel :: materials");
	

	clError = clSetKernelArg(members->directionalAABBShadowKernel, 5, sizeof(*params->aabbs), params->aabbs);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: aabbs");

	clError = clSetKernelArg(members->directionalAABBShadowKernel, 6, sizeof(cl_bool) * numAABBs, NULL);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: directionalAABBShadowKernel :: shadowReduce");

	clError = clSetKernelArg(members->directionalAABBShadowKernel, 7, sizeof(size_t) * numAABBs, NULL);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Exeture :: clSetKernelArg :: directionalAABBShadowKernel :: indexReduce");	

	const size_t numGlobalAABB[3] = { rBuffer->textureWidth, rBuffer->textureHeight, numAABBs };
	const size_t numLocalAABB[3] = { 1, 1, numAABBs };

	clError = clEnqueueNDRangeKernel
	(
		buffer->clQueue,
		members->directionalAABBShadowKernel,
		3,
		NULL,
		&numGlobalAABB[0],
		&numLocalAABB[0],
		numBeforeExecution,
		completeBeforeExecution,
		&execution[0]
	);

	///
	//Start Sphere Shadow Calculations
	//Set Kernel Arguments
	clError = clSetKernelArg(members->directionalSphereShadowKernel, 0, sizeof(members->sphereShadowTexture), &members->sphereShadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: shadowTexture");
	clError = clSetKernelArg(members->directionalSphereShadowKernel, 1, sizeof(rBuffer->textureRefs[RayBuffer_TextureType_POSITION]), &rBuffer->textureRefs[RayBuffer_TextureType_POSITION]);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: positionTexture");

	clError = clSetKernelArg(members->directionalSphereShadowKernel, 2, sizeof(members->lightDirection), &members->lightDirection);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: lightDirection");

	clError = clSetKernelArg(members->directionalSphereShadowKernel, 3, sizeof(*params->gObjects), params->gObjects);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: gObjects");

	clError = clSetKernelArg(members->directionalSphereShadowKernel, 4, sizeof(*params->materials), params->materials);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: materials");




	clError = clSetKernelArg(members->directionalSphereShadowKernel, 5, sizeof(*params->spheres), params->spheres);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: sphere");

	clError = clSetKernelArg(members->directionalSphereShadowKernel, 6, sizeof(cl_bool) * numGObjects, NULL);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: shadowReduce");

	clError = clSetKernelArg(members->directionalSphereShadowKernel, 7, sizeof(float) * numGObjects, NULL);

	const size_t numGlobalSpheres[3] = { rBuffer->textureWidth, rBuffer->textureHeight, numGObjects};
	const size_t numLocalSpheres[3] = { 1, 1, numGObjects};


	//Execute kernel
	clError = clEnqueueNDRangeKernel
	(
		buffer->clQueue,
		members->directionalSphereShadowKernel,
		3,
		NULL,
		&numGlobalSpheres[0],
		&numLocalSpheres[0],
		numBeforeExecution,
		completeBeforeExecution,
		&execution[1]
	);


	clError = clSetKernelArg(members->reduceShadowTextures, 0, sizeof(rBuffer->textureRefs[RayBuffer_TextureType_SHADOW]), &rBuffer->textureRefs[RayBuffer_TextureType_SHADOW]);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: reduceShadowTextures :: shadowTexture");
	
	clError = clSetKernelArg(members->reduceShadowTextures, 1, sizeof(members->sphereShadowTexture), &members->sphereShadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: reduceShadowTextures :: sphereShadowTexture");
	
	clError = clSetKernelArg(members->reduceShadowTextures, 2, sizeof(members->aabbShadowTexture), &members->aabbShadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: reduceShadowTextures :: aabbShadowTexture");
	
	const size_t numGlobalReduction[3] = { rBuffer->textureWidth, rBuffer->textureHeight, 1};
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
		2,
		execution,
		&reduction
	);



	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clEnqueueNDRangeKernel :: reduceShadowTextures");

	clWaitForEvents(1, &reduction);
	clFinish(buffer->clQueue);
}
