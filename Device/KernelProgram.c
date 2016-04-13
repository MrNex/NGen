#include "KernelProgram.h"

#include <stdlib.h>

#include "../Manager/KernelManager.h"

#include "../Load/Loader.h"

///
//Allocates a new Kernel Program
//
//Returns:
//	A pointer to a newly allocated uninitialized kernel program
KernelProgram* KernelProgram_Allocate(void)
{
	return malloc(sizeof(KernelProgram));
}///
//Get a reference to the Kernel Manager's internal Kernel Buffer
//
//Returns:
//	A pointer to the active Kernel Buffer
KernelBuffer* KernelManager_GetKernelBuffer(void);

///
//Initializes a kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to initialize
//	source: The filepath to the file containing the source code for the kernel program
//	buffer: The active kernel buffer containing the context and device in/on which to create the program
void KernelProgram_Initialize(KernelProgram* prog, const char* sourcePath, KernelBuffer* buffer)
{

	KernelProgram_InitializeWithOptions(prog, sourcePath, buffer, "\0");	
}

void KernelProgram_InitializeWithOptions(KernelProgram* prog, const char* sourcePath, KernelBuffer* buffer, const char* options)
{
	char* source = Loader_LoadTextFile(sourcePath);
	size_t size = strlen(source);
	cl_int clError = 0;
	prog->clProgram = clCreateProgramWithSource
	(
		buffer->clContext,
		1,
		(const char**)&source,
		&size,
		&clError
	);

	KernelManager_CheckCLErrors(clError, "KernelProgram_Initialize :: clCreateProgramWithSource");

	free(source);
	
	clError = clBuildProgram
	(
		prog->clProgram,
		1,
		&buffer->clDevice,
		options,
		NULL,
		NULL
	);

	if(KernelManager_CheckCLErrors(clError, "KernelProgram_Initialize :: clBuildProgram"))
	{
		cl_build_status status;
		size_t logSize;
		clGetProgramBuildInfo
		(
			prog->clProgram,
			buffer->clDevice,
			CL_PROGRAM_BUILD_STATUS,
			sizeof(cl_build_status),
			&status,
			NULL
		);

		clGetProgramBuildInfo
		(
			prog->clProgram,
			buffer->clDevice,
			CL_PROGRAM_BUILD_LOG,
			0,
			NULL,
			&logSize
		);

		char* progLog = calloc(sizeof(char) ,logSize + 1);

		clGetProgramBuildInfo
		(
			prog->clProgram,
			buffer->clDevice,
			CL_PROGRAM_BUILD_LOG,
			sizeof(char) * logSize,
			progLog,
			NULL
		);

		printf("Kernel Program Build Failed!\nSource Path:\t%s\nERROR=\t%d\tSTATUS=\t%d\nProgram Log:\n%s\n\n", sourcePath, clError, status, progLog);

		free(progLog);

	}

	prog->FreeMembers = NULL;
	prog->Execute = NULL;
}

///
//Frees the memory consumed by a kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to free
void KernelProgram_Free(KernelProgram* prog)
{
	prog->FreeMembers(prog);

	cl_int clError = clReleaseProgram(prog->clProgram);
	KernelManager_CheckCLErrors(clError, "KernelProgram_Free :: clReleaseProgram");
}

