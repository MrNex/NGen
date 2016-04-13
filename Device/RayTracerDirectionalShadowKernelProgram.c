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
	cl_mem shadowTexture;
	cl_mem positionTexture;
	cl_mem sphereShadowTexture;
	cl_mem aabbShadowTexture;
	cl_mem lightDirection;

	cl_mem spheres;
	cl_mem aabbs;

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
static void RayTracerDirectionalShadowKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer, RayBuffer* rBuffer);

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
//	rBuffer: A pointer to the ray buffer containing data needed for execution
static void RayTracerDirectionalShadowKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, RayBuffer* rBuffer);

///
//Initializes a RayTracerDirectionalShadow Kernel Program
//
//Parameters:
//	prog: A pointer to an uninitialized Kernel Progral to initialize as a RayTracerDirectionalShadow Kernel Program
//	buffer: A pointer to the KernelBuffer containing the context and device on which to build the kernel
//	rBuffer: A pointer to the RayBuffer cotaining the textures this kernel will use
void RayTracerDirectionalShadowKernelProgram_Initialize(KernelProgram* prog, KernelBuffer* buffer, RayBuffer* rBuffer)
{
	KernelProgram_Initialize(prog, "Kernel/RayTracerShadowRayKernelProgram.cl", buffer);
	
	prog->members = RayTracerDirectionalShadowKernelProgram_AllocateMembers();
	RayTracerDirectionalShadowKernelProgram_InitializeMembers(prog, buffer, rBuffer);

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
static void RayTracerDirectionalShadowKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer, RayBuffer* rBuffer)
{
	RayTracerDirectionalShadowKernelProgram_Members* members = prog->members;

	EnvironmentBuffer* envBuffer = EnvironmentManager_GetEnvironmentBuffer();

	unsigned char numSpheres, numAABBs;
	numSpheres = collisionBuffer->sphereData->pool->capacity;
	numAABBs = collisionBuffer->aabbData->pool->capacity;

	cl_int clError;
	members->shadowTexture = clCreateFromGLTexture
	(
		buffer->clContext, 
		CL_MEM_WRITE_ONLY,
		GL_TEXTURE_2D,
		0,
		rBuffer->textures[RayBuffer_TextureType_SHADOW],
		&clError
	);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_InitializeMembers :: clCreateFromGLTexture :: SHADOW");

	members->positionTexture = clCreateFromGLTexture
	(
		buffer->clContext,
		CL_MEM_READ_ONLY,
		GL_TEXTURE_2D,
		0,
		rBuffer->textures[RayBuffer_TextureType_POSITION],
		&clError
	);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_InitializeMembers :: clCreateFromGLTexture :: POSITION");

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


	

	members->spheres = clCreateBuffer
	(
		buffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(struct ColliderData_Sphere) * numSpheres,
		NULL,
		&clError
	);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_InitializeMembers :: clCreateBuffer :: sphere");

	members->aabbs = clCreateBuffer
	(
		buffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(struct ColliderData_AABB) * numAABBs,
		NULL,
		&clError
	);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKerelProgram_InitializeMembers :: clCreateBuffer :: aabbs");

	
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
	clError = clReleaseMemObject(members->positionTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_FreeMembers :: clReleaseMemObject :: positionTexture");


	clError = clReleaseMemObject(members->shadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_FreeMembers :: clReleaseMemObject :: shadowTexture");

	clError = clReleaseMemObject(members->sphereShadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_FreeMembers :: clReleaseMemObject :: sphereShadowTexture");

	clError = clReleaseMemObject(members->aabbShadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_FreeMembers :: clReleaseMemObject :: aabbShadowTexture");


	clError = clReleaseMemObject(members->lightDirection);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_FreeMembers :: clReleaseMemObject :: lightDirection");


	clError = clReleaseMemObject(members->spheres);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_FreeMembers :: clReleaseMemObject :: spheres");

	clError = clReleaseMemObject(members->aabbs);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_FreeMembers :: clReleaseMemObject :: aabbs");

	free(members);
	prog->members = NULL;
}

///
//Executes a RayTracerDirectionalShadowKernelProgram
//
//Parameters:
//	prog: A pointer to the RayTracerDirectionalShadowKernelProgram to execute
//	buffer: A pointer to the active KernelBuffer containing the context and device on which to execute
//	rBuffer: A pointer to the ray buffer containing data needed for execution
static void RayTracerDirectionalShadowKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, RayBuffer* rBuffer)
{
	struct ColliderData_Sphere* spheres = collisionBuffer->worldSphereData->pool->data;
	struct ColliderData_AABB* aabbs = collisionBuffer->worldAABBData->pool->data;

	static int counter = 0;

	
	if( ++counter > 10)
	{
		Vector_PrintTransposeArray((float*)aabbs, 12);

		counter = 0;
	}

	unsigned char numSpheres, numAABBs;
	numSpheres = collisionBuffer->sphereData->pool->size;
	numAABBs = collisionBuffer->aabbData->pool->size;

	RenderingBuffer* renderingBuffer = RenderingManager_GetRenderingBuffer();
	RayTracerDirectionalShadowKernelProgram_Members* members = prog->members;

	cl_int clError;

	const unsigned int numBeforeExecution = 5;
	cl_event completeBeforeExecution[numBeforeExecution];

	//Acquire OGL resources
	//glActiveTexture(GL_TEXTURE0 + RayBuffer_TextureType_SHADOW);
	//glBindTexture(GL_TEXTURE_2D, rBuffer->textures[RayBuffer_TextureType_SHADOW]);
	clError = clEnqueueAcquireGLObjects
	(
		buffer->clQueue, 
		1, 
		&members->shadowTexture, 
		0, 
		NULL, 
		&completeBeforeExecution[0]
	);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clEnqueueAcquireGLObjects");
	
	clError = clEnqueueAcquireGLObjects
	(
		buffer->clQueue, 
		1, 
		&members->positionTexture, 
		0, 
		NULL, 
		&completeBeforeExecution[1]
	);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clEnqueueAcquireGLObjects");


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
		&completeBeforeExecution[2]
	);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clEnqueueWriteBuffer :: lightDirection");


	clError = clEnqueueWriteBuffer
	(
		buffer->clQueue,
		members->spheres,
		CL_FALSE,
		0,
		sizeof(struct ColliderData_Sphere) * numSpheres,
		spheres,
		0,
		NULL,
		&completeBeforeExecution[3]
	);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clEnqueueWriteBuffer :: sphere");

	

	clError = clEnqueueWriteBuffer
	(
		buffer->clQueue,
		members->aabbs,
		CL_FALSE,
		0,
		sizeof(struct ColliderData_AABB) * numAABBs,
		aabbs,
		0,
		NULL,
		&completeBeforeExecution[4]
	);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clEnqueueWriteBuffer :: aabbs");


	///
	//Execution
	cl_event execution[2];


	///
	//Start AABB Shadow calculations
	//Set kernel arguments
	clError = clSetKernelArg(members->directionalAABBShadowKernel, 0, sizeof(members->aabbShadowTexture), &members->aabbShadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: aabbShadowTexture");

	clError = clSetKernelArg(members->directionalAABBShadowKernel, 1, sizeof(members->positionTexture), &members->positionTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: directionalAABBShadowKernel :: positionTexture");

	clError = clSetKernelArg(members->directionalAABBShadowKernel, 2, sizeof(members->lightDirection), &members->lightDirection);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: directionalAABBShadowKernel :: lightDirection");

	clError = clSetKernelArg(members->directionalAABBShadowKernel, 3, sizeof(members->aabbs), &members->aabbs);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: aabbs");

	clError = clSetKernelArg(members->directionalAABBShadowKernel, 4, sizeof(cl_bool) * numAABBs, NULL);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: directionalAABBShadowKernel :: shadowReduce");
	

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
	clError = clSetKernelArg(members->directionalSphereShadowKernel, 1, sizeof(members->positionTexture), &members->positionTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: positionTexture");

	clError = clSetKernelArg(members->directionalSphereShadowKernel, 2, sizeof(members->lightDirection), &members->lightDirection);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: lightDirection");
	clError = clSetKernelArg(members->directionalSphereShadowKernel, 3, sizeof(members->spheres), &members->spheres);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: sphere");

	clError = clSetKernelArg(members->directionalSphereShadowKernel, 4, sizeof(cl_bool) * numSpheres, NULL);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: shadowReduce");

	const size_t numGlobalSpheres[3] = { rBuffer->textureWidth, rBuffer->textureHeight, numSpheres};
	const size_t numLocalSpheres[3] = { 1, 1, numSpheres};


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


	clError = clSetKernelArg(members->reduceShadowTextures, 0, sizeof(members->shadowTexture), &members->shadowTexture);
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



	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clEnqueueNDRangeKernel");

	cl_event cleanup[2];

	clError = clEnqueueReleaseGLObjects(buffer->clQueue, 1, &members->shadowTexture, 1, &reduction, &cleanup[0]);
	clError = clEnqueueReleaseGLObjects(buffer->clQueue, 1, &members->positionTexture, 1, &reduction, &cleanup[1]);

	clWaitForEvents(2, cleanup);
	clFinish(buffer->clQueue);
}
