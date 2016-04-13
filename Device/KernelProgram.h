#ifndef KERNELPROGRAM_H
#define KERNELPROGRAM_H

#include <CL/cl.h>

struct KernelBuffer;

///
//Define the set of members needed by this Kernel Program
	
typedef void* KernelProgram_Members;

typedef struct KernelProgram
{
	cl_program clProgram;
	KernelProgram_Members members;

	void (*FreeMembers)(struct KernelProgram* prog);
	void (*Execute)(struct KernelProgram* prog, struct KernelBuffer* buffer, void* args);
} KernelProgram;

typedef void (*KernelProgram_ExecuteFunc)(struct KernelProgram* prog, struct KernelBuffer* buffer, void* args);

///
//Allocates a new Kernel Program
//
//Returns:
//	A pointer to a newly allocated uninitialized kernel program
KernelProgram* KernelProgram_Allocate(void);

///
//Initializes a kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to initialize
//	source: The filepath to the file containing the source code for the kernel program
//	buffer: The active kernel buffer containing the context and device in/on which to create the program
void KernelProgram_Initialize(KernelProgram* prog, const char* source, struct KernelBuffer* buffer);

///
//Initializes a kernel program with build options
//
//Parameters:
//	prog: A pointer to the kernel program to initialize
//	source: The filepath to the file containing the source code for the kernel program
//	buffer: The active kernel buffer containing the context and device in/on which to create the program
//	options: A null terminated string containing the build options to use
void KernelProgram_InitializeWithOptions(KernelProgram* prog, const char* sourcePath, struct KernelBuffer* buffer, const char* options);


///
//Frees the memory consumed by a kernel program
//
//Parameters:
//	prog: A pointer to the kernel program to free
void KernelProgram_Free(KernelProgram* prog);

#endif
