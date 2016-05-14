#include "ToneReproductionKernelProgram.h"

#include <stdlib.h>
#include <CL/cl_gl.h>

#include "../Manager/KernelManager.h"
#include "../Manager/EnvironmentManager.h"

typedef struct ToneReproductionKernelProgram_Members
{
	cl_mem luminence;
	cl_mem globalReduce;
	
	float xThreads;
	float yThreads;

	cl_kernel toneKernel;
	cl_kernel tone2Kernel;
	cl_kernel reductionKernel;
} ToneReproductionKernelProgram_Members;

///
//Allocates a set of internal members for thekernel
//
//Returns:
//	Void pointer to struct containing uninitialized members
static ToneReproductionKernelProgram_Members* ToneReproductionKernelProgram_AllocateMembers();

///
//Initializes the members of a kernel
//
//Parameters:
//	prog: A pointer to the tonereproductionKernelProgram to initialize the members of
//	buffer: A pointer to the active kernel buffer containing the target context
static void ToneReproductionKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer);

///
//Frees the internal members of the kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to free the internal members of
static void ToneReproductionKernelProgram_FreeMembers(KernelProgram* prog);

///
//Executes the  kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to execute
//	buffer: A pointer to the kernel buffer containing the target context
//	gBuffer: The global buffer to apply tone reproduction to
static void ToneReproductionKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, GlobalBuffer* gBuffer);


///
//Initializes a ToneReproductionKernelProgram
//
//Parameters:
//	prog: A pointer to an uninitialized kernel program to initialize as a ToneReproductionKernelProgram
//	buffer: A pointer to the kernel buffer continaing the context and device on which to build the kernel
void ToneReproductionKernelProgram_Initialize(KernelProgram* prog, struct KernelBuffer* buffer)
{
	KernelProgram_Initialize(prog, "Kernel/ToneReproductionKernelProgram.cl", buffer);
	prog->members = ToneReproductionKernelProgram_AllocateMembers();
	ToneReproductionKernelProgram_InitializeMembers(prog, buffer);

	prog->FreeMembers = ToneReproductionKernelProgram_FreeMembers;
	prog->Execute = (KernelProgram_ExecuteFunc)ToneReproductionKernelProgram_Execute;
}

///
//Allocates a set of internal members for thekernel
//
//Returns:
//	Void pointer to struct containing uninitialized members
static ToneReproductionKernelProgram_Members* ToneReproductionKernelProgram_AllocateMembers()
{
	return malloc(sizeof(ToneReproductionKernelProgram_Members));
}

///
//Initializes the members of a kernel
//
//Parameters:
//	prog: A pointer to the tonereproductionKernelProgram to initialize the members of
//	buffer: A pointer to the active kernel buffer containing the target context
static void ToneReproductionKernelProgram_InitializeMembers(KernelProgram* prog, KernelBuffer* buffer)
{
	ToneReproductionKernelProgram_Members* members = prog->members;
	
	EnvironmentBuffer* eBuffer = EnvironmentManager_GetEnvironmentBuffer();

	members->xThreads = members->yThreads = 10;

	cl_int err = 0;
	members->luminence = clCreateBuffer
	(
		buffer->clContext,
		CL_MEM_READ_ONLY,
		sizeof(float) * 4,
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_InitializeMembers :: luminence");

	

	members->globalReduce = clCreateBuffer
	(
		buffer->clContext,
		CL_MEM_READ_WRITE,
		sizeof(float) * (eBuffer->windowWidth / members->xThreads) * (eBuffer->windowHeight / members->yThreads),
		NULL,
		&err
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_InitializeMembers :: globalReduce");

	members->toneKernel = clCreateKernel(prog->clProgram, "ToneReproduction", &err);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_InitializeMembers :: toneKernel");
	
	members->tone2Kernel = clCreateKernel(prog->clProgram, "ToneReproductionSecond", &err);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_InitializeMembers :: tone2Kernel");
	
	members->reductionKernel = clCreateKernel(prog->clProgram, "ReductionKernel", &err);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_InitializeMembers :: reductionKernel");
}

///
//Frees the internal members of the kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to free the internal members of
static void ToneReproductionKernelProgram_FreeMembers(KernelProgram* prog)
{
	ToneReproductionKernelProgram_Members* members = (ToneReproductionKernelProgram_Members*)prog->members;
	
	cl_int err = 0;
	err = clReleaseMemObject(members->luminence);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_FreeMembers :: luminence");

	err = clReleaseMemObject(members->globalReduce);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_FreeMembers :: globalReduce");
}

///
//Executes the  kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to execute
//	buffer: A pointer to the kernel buffer containing the target context
//	gBuffer: The global buffer to apply tone reproduction to
static void ToneReproductionKernelProgram_Execute(KernelProgram* prog, KernelBuffer* buffer, GlobalBuffer* gBuffer)
{
	ToneReproductionKernelProgram_Members* members = prog->members;

	cl_int err = 0;
	cl_event upload;

	//Upload luminance values
	float luminence[4] =
	{
		500.0f,		//lDisplayMax
		1000.0f,		//lMax
		0.0f,		//unused
		0.0f,		//Unused
	};

	err = clEnqueueWriteBuffer
	(
		buffer->clQueue,
		members->luminence,
		CL_FALSE,
		0,
		sizeof(float) * 4,
		&luminence[0],
		0,
		NULL,
		&upload
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clEnqueueWriteBuffer :: luminence");

	//Set kernel arguments

	/*	
	err = clSetKernelArg
	(
		members->toneKernel,
		0,
		sizeof(gBuffer->textureRefs[GlobalBuffer_TextureType_FINAL]),
		&gBuffer->textureRefs[GlobalBuffer_TextureType_FINAL]
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: finalTexture");
	*/
	
	err = clSetKernelArg
	(
		members->toneKernel,
		0,
		sizeof(gBuffer->textureRefs[GlobalBuffer_TextureType_LOCAL]),
		&gBuffer->textureRefs[GlobalBuffer_TextureType_LOCAL]
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: localTexture");
	
	err = clSetKernelArg
	(
		members->toneKernel,
		1,
		sizeof(gBuffer->textureRefs[GlobalBuffer_TextureType_REFLECTION]),
		&gBuffer->textureRefs[GlobalBuffer_TextureType_REFLECTION]
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: reflectionTexture");

	err = clSetKernelArg
	(
		members->toneKernel,
		2,
		sizeof(gBuffer->textureRefs[GlobalBuffer_TextureType_TRANSMISSION]),
		&gBuffer->textureRefs[GlobalBuffer_TextureType_TRANSMISSION]
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: transmissionTexture");

	err = clSetKernelArg
	(
		members->toneKernel,
		3,
		sizeof(gBuffer->textureRefs[GlobalBuffer_TextureType_GLOBALMATERIAL]),
		&gBuffer->textureRefs[GlobalBuffer_TextureType_GLOBALMATERIAL]
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: globalMaterialTexture");

	
	err = clSetKernelArg
	(
		members->toneKernel,
		4,
		sizeof(members->luminence),
		&members->luminence
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: luminence");


	err = clSetKernelArg
	(
		members->toneKernel,
		5,
		sizeof(members->globalReduce),
		&members->globalReduce
	);

	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: globalReduce");

	err = clSetKernelArg
	(
		members->toneKernel,
		6,
		sizeof(float) * members->xThreads * members->yThreads,
		NULL
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: valReduce");



	//Execution
	const size_t numGlobal[2] = {gBuffer->textureWidth, gBuffer->textureHeight};
	const size_t numLocal[2] = {members->xThreads, members->yThreads};

	cl_event firstExecution;
	err = clEnqueueNDRangeKernel
	(
		buffer->clQueue,
		members->toneKernel,
		2,
		NULL,
		&numGlobal[0],
		&numLocal[0],
		1,
		&upload,
		&firstExecution
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clEnqueueNDRangeKernel");

	//Reduction set kernel args
	err = clSetKernelArg
	(
		members->reductionKernel,
		0,
		sizeof(members->globalReduce),
		&members->globalReduce
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: globalReduce");
	err = clSetKernelArg
	(
		members->reductionKernel,
		1,
		sizeof(float) * members->xThreads * members->yThreads,
		NULL
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: valReduce");

	const size_t numR1Global[2] = {8 * members->xThreads, 6 * members->yThreads};
	const size_t numR1Local[2] = {members->xThreads, members->yThreads};


	cl_event r1Execution;
	err = clEnqueueNDRangeKernel
	(
		buffer->clQueue,
		members->reductionKernel,
		2,
		NULL,
		&numR1Global[0],
		&numR1Local[0],
		1,
		&firstExecution,
		&r1Execution
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clEnqueueNDRangeKernel");

	const size_t numR2Global[2] = {8, 6};
	const size_t numR2Local[2] = {8, 6};


	cl_event r2Execution;
	err = clEnqueueNDRangeKernel
	(
		buffer->clQueue,
		members->reductionKernel,
		2,
		NULL,
		&numR2Global[0],
		&numR2Local[0],
		1,
		&r1Execution,
		&r2Execution
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clEnqueueNDRangeKernel");



	//Set Second pass kernel args
	err = clSetKernelArg
	(
		members->tone2Kernel,
		0,
		sizeof(gBuffer->textureRefs[GlobalBuffer_TextureType_FINAL]),
		&gBuffer->textureRefs[GlobalBuffer_TextureType_FINAL]
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: finalTexture");

	
	err = clSetKernelArg
	(
		members->tone2Kernel,
		1,
		sizeof(gBuffer->textureRefs[GlobalBuffer_TextureType_LOCAL]),
		&gBuffer->textureRefs[GlobalBuffer_TextureType_LOCAL]
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: localTexture");
	
	err = clSetKernelArg
	(
		members->tone2Kernel,
		2,
		sizeof(gBuffer->textureRefs[GlobalBuffer_TextureType_REFLECTION]),
		&gBuffer->textureRefs[GlobalBuffer_TextureType_REFLECTION]
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: reflectionTexture");

	err = clSetKernelArg
	(
		members->tone2Kernel,
		3,
		sizeof(gBuffer->textureRefs[GlobalBuffer_TextureType_TRANSMISSION]),
		&gBuffer->textureRefs[GlobalBuffer_TextureType_TRANSMISSION]
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: transmissionTexture");

	err = clSetKernelArg
	(
		members->tone2Kernel,
		4,
		sizeof(gBuffer->textureRefs[GlobalBuffer_TextureType_GLOBALMATERIAL]),
		&gBuffer->textureRefs[GlobalBuffer_TextureType_GLOBALMATERIAL]
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: globalMaterialTexture");

	
	err = clSetKernelArg
	(
		members->tone2Kernel,
		5,
		sizeof(members->luminence),
		&members->luminence
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: luminence");


	err = clSetKernelArg
	(
		members->tone2Kernel,
		6,
		sizeof(members->globalReduce),
		&members->globalReduce
	);

	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clSetKernelArg :: globalReduce");

	//Second execution
	//Execution
	cl_event secondExecution;
	err = clEnqueueNDRangeKernel
	(
		buffer->clQueue,
		members->tone2Kernel,
		2,
		NULL,
		&numGlobal[0],
		&numLocal[0],
		1,
		&r2Execution,
		&secondExecution
	);
	KernelManager_CheckCLErrors(err, "ToneReproductionKernelProgram_Execute :: clEnqueueNDRangeKernel");

	clWaitForEvents(1, &secondExecution);

}
