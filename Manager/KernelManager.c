#include "KernelManager.h"

#include <CL/cl_gl.h>

#include "EnvironmentManager.h"

#include <stdio.h>
#include <stdlib.h>

#include "../Device/KernelProgram.h"
#include "../Device/RayTracerDirectionalShadowKernelProgram.h"


//Internals
static KernelBuffer* kernelBuffer = NULL;

///
//Initializes the kernel manager
void KernelManager_Initialize(void)
{
	printf("Initializing Kernel Manager...\n");
	kernelBuffer = malloc(sizeof(KernelBuffer));
	
	cl_int clError = 0;
	cl_uint numQuery = 0;
	cl_platform_id platform;

	clError = clGetPlatformIDs(1, &platform, &numQuery);
	KernelManager_CheckCLErrors(clError, "KernelManager_Initialize :: clGetPlatformIDs");

	printf("%d platforms identified. \nSelecting platform...\n", numQuery);

	size_t vendorSize;
	clError = clGetPlatformInfo
	(
		platform,
		CL_PLATFORM_VENDOR,
		0,
		NULL,
		&vendorSize
	);
	KernelManager_CheckCLErrors(clError, "KernelManager_Initialize :: clGetPlatformInfo ( CL_PLATFORM_VENDOR size )");

	char* vendor = malloc(sizeof(char) * vendorSize);

	clError = clGetPlatformInfo
	(
		platform,
		CL_PLATFORM_VENDOR,
		vendorSize,
		vendor,
		NULL
	);
	KernelManager_CheckCLErrors(clError, "KernelManager_Initialize :: clGetPlatformInfo ( CL_PLATFORM_VENDOR )");

	printf("Platform by %s ( ID: %p )\n", vendor, (void*)platform);
	free(vendor);

	EnvironmentBuffer* eBuffer = EnvironmentManager_GetEnvironmentBuffer();

	//Define properties of our context
	cl_context_properties clProperties[7]; 
	clProperties[0] = CL_GL_CONTEXT_KHR;
	clProperties[1] = (cl_context_properties)eBuffer->glContextHandle;
	if(eBuffer->operatingSystem == EnvironmentManager_OS_LINUX)
	{
		clProperties[2] = CL_GLX_DISPLAY_KHR;
		clProperties[3] = (cl_context_properties)eBuffer->glDeviceHandle;
	}
	else if(eBuffer->operatingSystem == EnvironmentManager_OS_WINDOWS)
	{

		clProperties[2] = CL_WGL_HDC_KHR;
		clProperties[3] = (cl_context_properties)eBuffer->glDeviceHandle;
	}
	else
	{
		printf("Error: OGL/Kernel memory sharing is not defined on your platform.\n");
	}

	clProperties[4] = CL_CONTEXT_PLATFORM;
	clProperties[5] = (cl_context_properties)platform;
	clProperties[6] = 0;

	//Create context using properties
	kernelBuffer->clContext = clCreateContextFromType
	(
		clProperties,
		CL_DEVICE_TYPE_GPU,
		NULL,
		NULL,
		&clError
	);

	KernelManager_CheckCLErrors(clError, "KernelManager_Initialize :: clCreateContextFromType");

	printf("Identifying Devices...\n");

	clError = clGetContextInfo
	(
		kernelBuffer->clContext,
		CL_CONTEXT_NUM_DEVICES,
		sizeof(numQuery),
		&numQuery,
		NULL
	);
	KernelManager_CheckCLErrors(clError, "KernelManager_Initialize :: clGetContextInfo ( CL_CONTEXT_NUM_DEVICES )");
	printf("%d Devices were identified.\nSelecting Device...\n", numQuery);

	clError = clGetContextInfo
	(
		kernelBuffer->clContext,
		CL_CONTEXT_DEVICES,
		sizeof(kernelBuffer->clDevice),
		&kernelBuffer->clDevice,
		NULL
	);

	KernelManager_CheckCLErrors(clError, "KernelManager_Initialize :: clGetContextInfo ( CL_CONTEXT_DEVICES )");

	printf("Device selected:\t%p\n", (void*)kernelBuffer->clDevice);

	size_t extensionSize = 0;

	clGetDeviceInfo
	(
		kernelBuffer->clDevice,
		CL_DEVICE_EXTENSIONS,
		0,
		NULL,
		&extensionSize
	);
	KernelManager_CheckCLErrors(clError, "KernelManager_Initialize :: clGetDeviceInfo ( CL_DEVICE_EXTENSIONS size )");

	char* extensions = malloc(sizeof(char) * extensionSize);


	clGetDeviceInfo
	(
		kernelBuffer->clDevice,
		CL_DEVICE_EXTENSIONS,
		extensionSize,
		extensions,
		NULL
	);
	KernelManager_CheckCLErrors(clError, "KernelManager_Initialize :: clGetDeviceInfo ( CL_DEVICE_EXTENSIONS )");

	printf("Device supports the following extensions:\n%s\n", extensions);
	free(extensions);

	cl_bool imageSupport;
	clError = clGetDeviceInfo
	(
		kernelBuffer->clDevice,
		CL_DEVICE_IMAGE_SUPPORT,
		sizeof(imageSupport),
		&imageSupport,
		NULL
	);
	KernelManager_CheckCLErrors(clError, "KernelManager_Initialize :: clGetDeviceInfo :: CL_DEVICE_IMAGE_SUPPORT");
	printf("Image support:\t%d\n", imageSupport);

	printf("Creating Command Queue...\n");
	/*
	   clCreateCommandQueueWithProperties keeps failing, removing this until I figure out why.
	cl_queue_properties queueProperties[] = 
	{
		CL_QUEUE_PROPERTIES,
			CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE |
			CL_QUEUE_ON_DEVICE,
		
		0	
	};
	*/

	kernelBuffer->clQueue = clCreateCommandQueue
	(
		kernelBuffer->clContext, 
		kernelBuffer->clDevice,
		0,
		&clError
	);

	KernelManager_CheckCLErrors(clError, "KernelManager_Initialize :: clCreateCommandQueueWithProperties");

	//kernelBuffer->programs = malloc(sizeof(KernelProgram) * KernelManager_KernelPrograms_NUMPROGRAMS);


	printf("Kernel Manager Initialization Sequence Executed.\n");
}

///
//Frees the kernel manager
void KernelManager_Free(void)
{
	cl_int clError;
	clError = clReleaseCommandQueue(kernelBuffer->clQueue);

	KernelManager_CheckCLErrors(clError, "KernelManager_Free :: clReleaseCommandQueue");


	clError = clReleaseDevice(kernelBuffer->clDevice);//Not sure if needed, but won't hurt according to spec
	KernelManager_CheckCLErrors(clError, "KernelManager_Free :: clReleaseDevice");

	clError = clReleaseContext(kernelBuffer->clContext);

	KernelManager_CheckCLErrors(clError, "KernelManager_Free :: clReleaseContext");
}

///
//Checks for a CL error code and prints an error if one is present
//
//Parameters:
//	err: The openCL return status
//	name: A string containing some indication of where this possible error may have occurred.
//
//Returns:
//	0 if everything is okay.
int KernelManager_CheckCLErrors(cl_int err, const char* name)
{
	if(err == CL_SUCCESS)
		return 0;
	
	printf("OpenCL Error Detected:\nError Number:\t%d\nError Name:\n\t%s\n", err, name);
	return -1;
}

///
//Get a reference to the Kernel Manager's internal Kernel Buffer
//
//Returns:
//	A pointer to the active Kernel Buffer
KernelBuffer* KernelManager_GetKernelBuffer(void)
{
	return kernelBuffer;
}
