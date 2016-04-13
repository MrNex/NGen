#ifndef KERNELMANAGER_H

#define KERNELMANAGER_H

#include <CL/cl.h>

enum KernelManager_KernelPrograms
{
	KernelManager_KernelPrograms_RAYTRACERSHADOW,
	KernelManager_KernelPrograms_NUMPROGRAMS
};

typedef struct KernelBuffer
{
	cl_context clContext;
	cl_device_id clDevice;

	cl_command_queue clQueue;

	//KernelProgram* programs;
} KernelBuffer;

///
//Initializes the kernel manager
void KernelManager_Initialize(void);

///
//Frees the kernel manager
void KernelManager_Free(void);

///
//Get a reference to the Kernel Manager's internal Kernel Buffer
//
//Returns:
//	A pointer to the active Kernel Buffer
KernelBuffer* KernelManager_GetKernelBuffer(void);

///
//Checks for a CL error code and prints an error if one is present
//
//Parameters:
//	err: The openCL return status
//	name: A string containing some indication of where this possible error may have occurred.
//
//Returns:
//	0 if everything is okay.
int KernelManager_CheckCLErrors(cl_int err, const char* name);





#endif
