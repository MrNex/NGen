#include "RayTracerKernelProgram.h"

#include <stdlib.h>
#include <CL/cl_gl.h>

#include "../Manager/KernelManager.h"
#include "../Manager/RenderingManager.h"
#include "../Manager/CollisionManager.h"
#include "../Manager/AssetManager.h"

typedef struct RayTracerKernelProgram_Members
{
	cl_mem lightColor;
	cl_mem lightDirection;
	cl_mem lightIntensity;

	cl_kernel rayTracerKernel;
} RayTracerKernelProgram_Members;

///
//Allocates a set of internal members for thekernel
//
//Returns:
//	Void pointer to struct containing uninitialized members
static RayTracerKernelProgram_Members* RayTracerKernelProgram_AllocateMembers();

///
//Initializes the members of a kernel
//
//Parameters:
//	prog: A pointer to the raytracerKernelProgram to initialize the members of
//	buffer: A pointer to the active kernel buffer containing the target context
static void RayTracerKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer);

///
//Frees the internal members of the kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to free the internal members of
static void RayTracerKernelProgram_FreeMembers(KernelProgram* prog);

///
//Executes the  kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to execute
//	buffer: A pointer to the kernel buffer containing the target context
//	params: The execution parameters specifying the memory with which to execute the kernel
static void RayTracerKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, struct RayTracerKernelProgram_ExecutionParameters* params);

///
//Initializes a RayTracerKernelProgram
//
//Parameters:
//	prog: A pointer to an uninitialized kernel program to initialize as a RayTracerKernelProgram
//	buffer: A pointer to the kernel buffer continaing the context and device on which to build the kernel
void RayTracerKernelProgram_Initialize(KernelProgram* prog, struct KernelBuffer* buffer)
{
	KernelProgram_Initialize(prog, "Kernel/RayTracerKernelProgram.cl", buffer);

	prog->members = RayTracerKernelProgram_AllocateMembers();
	RayTracerKernelProgram_InitializeMembers(prog, buffer);

	prog->FreeMembers = RayTracerKernelProgram_FreeMembers;
	prog->Execute = (KernelProgram_ExecuteFunc)RayTracerKernelProgram_Execute;
}

///
//Allocates a set of internal members for thekernel
//
//Returns:
//	Void pointer to struct containing uninitialized members
static RayTracerKernelProgram_Members* RayTracerKernelProgram_AllocateMembers()
{
	return malloc(sizeof(RayTracerKernelProgram_Members));
}

///
//Initializes the members of a kernel
//
//Parameters:
//	prog: A pointer to the raytracerKernelProgram to initialize the members of
//	buffer: A pointer to the active kernel buffer containing the target context
static void RayTracerKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer)
{
	RayTracerKernelProgram_Members* members = (RayTracerKernelProgram_Members*)prog->members;
	
	cl_int err = 0;
	members->lightColor = clCreateBuffer
	(
		buffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(float) * 3,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_InitializeMembers :: lightColor");

	members->lightDirection = clCreateBuffer
	(
		buffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(float) * 3,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_InitializeMembers :: lightDirection");

	members->lightIntensity = clCreateBuffer
	(
		buffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(float) * 2,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_InitializeMembers :: lightIntensity");

	members->rayTracerKernel = clCreateKernel(prog->clProgram, "RayTrace", &err);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_InitializeMembers :: rayTracerKernel");
}

///
//Frees the internal members of the kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to free the internal members of
static void RayTracerKernelProgram_FreeMembers(KernelProgram* prog)
{
	
	RayTracerKernelProgram_Members* members = (RayTracerKernelProgram_Members*)prog->members;
	
	cl_int err = 0;
	err = clReleaseMemObject(members->lightColor);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_FreeMembers :: lightColor");
	err = clReleaseMemObject(members->lightDirection);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_FreeMembers :: lightDirection");
	err = clReleaseMemObject(members->lightIntensity);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_FreeMembers :: lightIntensity");

	free(members);
	prog->members = NULL;

}

static void RayTracerKernelProgram_UpdateMembers(KernelProgram* prog, KernelBuffer* buffer, RenderingBuffer* rBuffer)
{
	RayTracerKernelProgram_Members* members = (RayTracerKernelProgram_Members*)prog->members;

	DirectionalLight* light = rBuffer->directionalLight;

	cl_int err = 0;

	cl_event updating[3];

	err = clEnqueueWriteBuffer
	(
		buffer->clQueue,
		members->lightColor,
		CL_FALSE,
		0,
		sizeof(float) * 3,
		light->base->color->components,
		0,
		NULL,
		&updating[0]
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_UpdateMembers :: lightColor");

	err = clEnqueueWriteBuffer
	(
		buffer->clQueue,
		members->lightDirection,
		CL_FALSE,
		0,
		sizeof(float) * 3,
		light->direction->components,
		0,
		NULL,
		&updating[1]
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_UpdateMembers :: lightDirection");

	float lightIntensities[2] = {light->base->ambientIntensity, light->base->diffuseIntensity};

	err = clEnqueueWriteBuffer
	(
		buffer->clQueue,
		members->lightIntensity,
		CL_FALSE,
		0,
		sizeof(float) * 2,
		lightIntensities,
		0,
		NULL,
		&updating[2]
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_UpdateMembers :: lightColor");

	clWaitForEvents(3, &updating[0]);
}

static void RayTracerKernelProgram_SetArguments(KernelProgram* prog, struct RayTracerKernelProgram_ExecutionParameters* params, unsigned int numGObjects)
{
	RayTracerKernelProgram_Members* members = (RayTracerKernelProgram_Members*)prog->members;
	RayBuffer* sBuf = params->srcBuffer;
	GlobalBuffer* dBuf = params->destBuffer;
	cl_mem textures = assetBuffer->textureArray;	


	//Destination textures
	cl_int err = 0;
	err = clSetKernelArg
	(
		members->rayTracerKernel,
		0,
		sizeof(dBuf->textureRefs[GlobalBuffer_TextureType_LOCAL]),
		&dBuf->textureRefs[GlobalBuffer_TextureType_LOCAL]
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: localTexture");

	err = clSetKernelArg
	(
		members->rayTracerKernel,
		1,
		sizeof(dBuf->textureRefs[GlobalBuffer_TextureType_REFLECTION]),
		&dBuf->textureRefs[GlobalBuffer_TextureType_REFLECTION]
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: reflectionTexture");

	err = clSetKernelArg
	(
		members->rayTracerKernel,
		2,
		sizeof(dBuf->textureRefs[GlobalBuffer_TextureType_TRANSMISSION]),
		&dBuf->textureRefs[GlobalBuffer_TextureType_TRANSMISSION]
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: transimissionTexture");

	err = clSetKernelArg
	(
		members->rayTracerKernel,
		3,
		sizeof(dBuf->textureRefs[GlobalBuffer_TextureType_GLOBALMATERIAL]),
		&dBuf->textureRefs[GlobalBuffer_TextureType_GLOBALMATERIAL]
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: globalMaterialTexture");

	//Source textures
	err = clSetKernelArg
	(
		members->rayTracerKernel,
		4,
		sizeof(sBuf->textureRefs[RayBuffer_TextureType_POSITION]),
		&sBuf->textureRefs[RayBuffer_TextureType_POSITION]
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: positionTexture");

	err = clSetKernelArg
	(
		members->rayTracerKernel,
		5,
		sizeof(sBuf->textureRefs[RayBuffer_TextureType_DIFFUSE]),
		&sBuf->textureRefs[RayBuffer_TextureType_DIFFUSE]
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: diffuseTexture");

	err = clSetKernelArg
	(
		members->rayTracerKernel,
		6,
		sizeof(sBuf->textureRefs[RayBuffer_TextureType_NORMAL]),
		&sBuf->textureRefs[RayBuffer_TextureType_NORMAL]
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: normalTexture");

	err = clSetKernelArg
	(
		members->rayTracerKernel,
		7,
		sizeof(sBuf->textureRefs[RayBuffer_TextureType_LOCALMATERIAL]),
		&sBuf->textureRefs[RayBuffer_TextureType_LOCALMATERIAL]
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: localMaterialTexture");


	err = clSetKernelArg
	(
		members->rayTracerKernel,
		8,
		sizeof(textures),
		&textures
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: textures");

	//Execution parameters

	err = clSetKernelArg
	(
		members->rayTracerKernel,
		9,
		sizeof(*params->spheres),
		params->spheres
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: spheres");



	err = clSetKernelArg
	(
		members->rayTracerKernel,
		10,
		sizeof(*params->aabbs),
		params->aabbs
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: aabbs");


	err = clSetKernelArg
	(
		members->rayTracerKernel,
		11,
		sizeof(*params->gObjects),
		params->gObjects
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: gObjects");


	err = clSetKernelArg
	(
		members->rayTracerKernel,
		12,
		sizeof(*params->materials),
		params->materials
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: materials");


	err = clSetKernelArg
	(
		members->rayTracerKernel,
		13,
		sizeof(*params->cameraPosition),
		params->cameraPosition
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: cameraPosition");


	err = clSetKernelArg
	(
		members->rayTracerKernel,
		14,
		sizeof(*params->sphereTransformations),
		params->sphereTransformations
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: sphereTransformations");

	//Members

	err = clSetKernelArg
	(
		members->rayTracerKernel,
		15,
		sizeof(members->lightColor),
		&members->lightColor
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: lightColor");


	err = clSetKernelArg
	(
		members->rayTracerKernel,
		16,
		sizeof(members->lightDirection),
		&members->lightDirection
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: lightDirection");


	err = clSetKernelArg
	(
		members->rayTracerKernel,
		17,
		sizeof(members->lightIntensity),
		&members->lightIntensity
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: lightIntensity");

	//Free space
	
	err = clSetKernelArg
	(
		members->rayTracerKernel,
		18,
		sizeof(cl_float) * numGObjects * 3 * 64,
		NULL
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: valReduce");


	err = clSetKernelArg
	(
		members->rayTracerKernel,
		19,
		sizeof(size_t) * numGObjects * 3 * 64,
		NULL
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_SetArguments :: indexReduce");

}

///
//Executes the  kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to execute
//	buffer: A pointer to the kernel buffer containing the target context
//	params: The execution parameters specifying the memory with which to execute the kernel
static void RayTracerKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, struct RayTracerKernelProgram_ExecutionParameters* params)
{

	RayTracerKernelProgram_Members* members = (RayTracerKernelProgram_Members*)prog->members;

	RenderingBuffer* rBuffer = RenderingManager_GetRenderingBuffer();
	RayTracerKernelProgram_UpdateMembers(prog, buffer, rBuffer);


	unsigned int numGObjects = objectBuffer->objectPool->pool->size;
	RayTracerKernelProgram_SetArguments(prog, params, numGObjects);


	const size_t numGlobal[3] = {params->srcBuffer->textureWidth, params->srcBuffer->textureHeight, numGObjects};
	const size_t numLocal[3] = {8, 8, numGObjects};

	
	cl_event execution;

	cl_int err = 0;

	err = clEnqueueNDRangeKernel
	(
		buffer->clQueue,
		members->rayTracerKernel,
		3,
		NULL,
		&numGlobal[0],
		&numLocal[0],
		0,
		NULL,
		&execution
	);
	KernelManager_CheckCLErrors(err, "RayTracerKernelProgram_Execute :: clEnqueueNDRangeKernel :: rayTracerKernel");

	clWaitForEvents(1, &execution);
}



