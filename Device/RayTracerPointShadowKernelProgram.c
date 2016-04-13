#include "RayTracerPointShadowKernelProgram.h"

#include <stdlib.h>
#include <CL/cl_gl.h>

#include "../Manager/KernelManager.h"
#include "../Manager/RenderingManager.h"
#include "../Manager/CollisionManager.h"

typedef struct RayTracerPointShadowKernelProgram_Members
{
	cl_mem shadowTexture;
	cl_mem positionTexture;
	cl_mem lightPosition;
	cl_mem spheres;

	cl_kernel pointShadowKernel;
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
static void RayTracerPointShadowKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer, RayBuffer* rBuffer);

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
//	rBuffer: A pointer to the RayBuffer containing the textures this kernel will use
void RayTracerPointShadowKernelProgram_Initialize(KernelProgram* prog, struct KernelBuffer* buffer, RayBuffer* rBuffer)
{
	KernelProgram_InitializeWithOptions(prog, "Kernel/RayTracerShadowRayKernelProgram.cl", buffer, "-D LIGHTTYPE=2");

	prog->members = RayTracerPointShadowKernelProgram_AllocateMembers();
	RayTracerPointShadowKernelProgram_InitializeMembers(prog, buffer, rBuffer);

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
static void RayTracerPointShadowKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer, RayBuffer* rBuffer)
{
	RayTracerPointShadowKernelProgram_Members* members = prog->members;

	unsigned char numSpheres;
	numSpheres = collisionBuffer->sphereData->pool->capacity;

	cl_int clError;

	//Shadow Texture
	//TODO: Share this buffer with the RayTracerDirectionalShadowKernelProgram	
	members->shadowTexture = clCreateFromGLTexture
	(
		buffer->clContext, 
		CL_MEM_WRITE_ONLY,
		GL_TEXTURE_2D,
		0,
		rBuffer->textures[RayBuffer_TextureType_SHADOW],
		&clError
	);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_InitializeMembers :: clCreateFromGLTexture :: SHADOW");

	//Position Texture
	//TODO: Share this buffer with the RayTracerDirectionalShadowKernelProgram
	members->positionTexture = clCreateFromGLTexture
	(
		buffer->clContext,
		CL_MEM_READ_ONLY,
		GL_TEXTURE_2D,
		0,
		rBuffer->textures[RayBuffer_TextureType_POSITION],
		&clError
	);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_InitializeMembers :: clCreateFromGLTexture :: POSITION");

	members->lightPosition = clCreateBuffer
	(
		buffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(float) * 3,
		NULL,
		&clError
	);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_InitializeMembers :: clCreateBuffer :: light");

	members->spheres = clCreateBuffer
	(
		buffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(struct ColliderData_Sphere) * numSpheres,
		NULL,
		&clError
	);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_InitializeMembers :: clCreateBuffer :: spheres");


	members->pointShadowKernel = clCreateKernel(prog->clProgram, "ComputeDirectionalSphereShadowTexture", &clError);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_InitializeMembers :: clCreateKernel :: ComputeDirectionalShadowTexture");
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
	clError = clReleaseMemObject(members->positionTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_FreeMembers :: clReleaseMemObject :: positionTexture");


	clError = clReleaseMemObject(members->shadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_FreeMembers :: clReleaseMemObject :: shadowTexture");


	clError = clReleaseMemObject(members->lightPosition);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_FreeMembers :: clReleaseMemObject :: lightDirection");


	clError = clReleaseMemObject(members->spheres);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_FreeMembers :: clReleaseMemObject :: sphere");

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

	struct ColliderData_Sphere* spheres = collisionBuffer->worldSphereData->pool->data;

	unsigned char numSpheres;
	numSpheres = collisionBuffer->worldSphereData->pool->size;


	cl_int clError;

	const unsigned int numEventsBeforeExecution = 4;
	cl_event completeBeforeExecution[numEventsBeforeExecution];

	//Acquire OGL resources
	clError = clEnqueueAcquireGLObjects
	(
		buffer->clQueue, 
		1, 
		&members->shadowTexture, 
		0, 
		NULL, 
		&completeBeforeExecution[0]
	);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_Execute :: clEnqueueAcquireGLObjects");
	
	clError = clEnqueueAcquireGLObjects
	(
		buffer->clQueue, 
		1, 
		&members->positionTexture, 
		0, 
		NULL, 
		&completeBeforeExecution[1]
	);
	KernelManager_CheckCLErrors(clError, "RayTracerPointShadowKernelProgram_Execute :: clEnqueueAcquireGLObjects");

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

	//Set kernel arguments
	clError = clSetKernelArg(members->pointShadowKernel, 0, sizeof(members->shadowTexture), &members->shadowTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: shadowTexture");
	clError = clSetKernelArg(members->pointShadowKernel, 1, sizeof(members->positionTexture), &members->positionTexture);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: positionTexture");

	clError = clSetKernelArg(members->pointShadowKernel, 2, sizeof(members->lightPosition), &members->lightPosition);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: lightDirection");
	clError = clSetKernelArg(members->pointShadowKernel, 3, sizeof(members->spheres), &members->spheres);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: sphere");

	clError = clSetKernelArg(members->pointShadowKernel, 4, sizeof(cl_bool) * numSpheres, NULL);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clSetKernelArg :: shadowReduce");

	//Enqueue Execution
	const size_t numGlobal[3] = { rBuffer->textureWidth, rBuffer->textureHeight, numSpheres };
	const size_t numLocal[3] = { 1, 1, numSpheres };

	cl_event execution;
	
	clError = clEnqueueNDRangeKernel
	(
	 	buffer->clQueue,
		members->pointShadowKernel,
		3,
		NULL,
		&numGlobal[0],
		&numLocal[0],
		numEventsBeforeExecution,
		completeBeforeExecution,
		&execution
	);
	KernelManager_CheckCLErrors(clError, "RayTracerDirectionalShadowKernelProgram_Execute :: clEnqueueNDRangeKernel");

	//Release shared OGL memory
	cl_event cleanup[2];

	clError = clEnqueueReleaseGLObjects(buffer->clQueue, 1, &members->shadowTexture, 1, &execution, &cleanup[0]);
	clError = clEnqueueReleaseGLObjects(buffer->clQueue, 1, &members->positionTexture, 1, &execution, &cleanup[1]);

	clWaitForEvents(2, cleanup);
	clFinish(buffer->clQueue);


}
