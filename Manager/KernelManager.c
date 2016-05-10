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

const char* KernelManager_GetCLErrorString(cl_int error)
{
	switch(error)
	{
	// run-time and JIT compiler errors
	case 0: return "CL_SUCCESS";
	case -1: return "CL_DEVICE_NOT_FOUND";
	case -2: return "CL_DEVICE_NOT_AVAILABLE";
	case -3: return "CL_COMPILER_NOT_AVAILABLE";
	case -4: return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case -5: return "CL_OUT_OF_RESOURCES";
	case -6: return "CL_OUT_OF_HOST_MEMORY";
	case -7: return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case -8: return "CL_MEM_COPY_OVERLAP";
	case -9: return "CL_IMAGE_FORMAT_MISMATCH";
	case -10: return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case -11: return "CL_BUILD_PROGRAM_FAILURE";
	case -12: return "CL_MAP_FAILURE";
	case -13: return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
	case -14: return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
	case -15: return "CL_COMPILE_PROGRAM_FAILURE";
	case -16: return "CL_LINKER_NOT_AVAILABLE";
	case -17: return "CL_LINK_PROGRAM_FAILURE";
	case -18: return "CL_DEVICE_PARTITION_FAILED";
	case -19: return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
	
	// compile-time errors
	case -30: return "CL_INVALID_VALUE";
	case -31: return "CL_INVALID_DEVICE_TYPE";
	case -32: return "CL_INVALID_PLATFORM";
	case -33: return "CL_INVALID_DEVICE";
	case -34: return "CL_INVALID_CONTEXT";
	case -35: return "CL_INVALID_QUEUE_PROPERTIES";
	case -36: return "CL_INVALID_COMMAND_QUEUE";
	case -37: return "CL_INVALID_HOST_PTR";
	case -38: return "CL_INVALID_MEM_OBJECT";
	case -39: return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case -40: return "CL_INVALID_IMAGE_SIZE";
	case -41: return "CL_INVALID_SAMPLER";
	case -42: return "CL_INVALID_BINARY";
	case -43: return "CL_INVALID_BUILD_OPTIONS";
	case -44: return "CL_INVALID_PROGRAM";
	case -45: return "CL_INVALID_PROGRAM_EXECUTABLE";
	case -46: return "CL_INVALID_KERNEL_NAME";
	case -47: return "CL_INVALID_KERNEL_DEFINITION";
	case -48: return "CL_INVALID_KERNEL";
	case -49: return "CL_INVALID_ARG_INDEX";
	case -50: return "CL_INVALID_ARG_VALUE";
	case -51: return "CL_INVALID_ARG_SIZE";
	case -52: return "CL_INVALID_KERNEL_ARGS";
	case -53: return "CL_INVALID_WORK_DIMENSION";
	case -54: return "CL_INVALID_WORK_GROUP_SIZE";
	case -55: return "CL_INVALID_WORK_ITEM_SIZE";
	case -56: return "CL_INVALID_GLOBAL_OFFSET";
	case -57: return "CL_INVALID_EVENT_WAIT_LIST";
	case -58: return "CL_INVALID_EVENT";
	case -59: return "CL_INVALID_OPERATION";
	case -60: return "CL_INVALID_GL_OBJECT";
	case -61: return "CL_INVALID_BUFFER_SIZE";
	case -62: return "CL_INVALID_MIP_LEVEL";
	case -63: return "CL_INVALID_GLOBAL_WORK_SIZE";
	case -64: return "CL_INVALID_PROPERTY";
	case -65: return "CL_INVALID_IMAGE_DESCRIPTOR";
	case -66: return "CL_INVALID_COMPILER_OPTIONS";
	case -67: return "CL_INVALID_LINKER_OPTIONS";
	case -68: return "CL_INVALID_DEVICE_PARTITION_COUNT";

	// extension errors
	case -1000: return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
	case -1001: return "CL_PLATFORM_NOT_FOUND_KHR";
	case -1002: return "CL_INVALID_D3D10_DEVICE_KHR";
	case -1003: return "CL_INVALID_D3D10_RESOURCE_KHR";
	case -1004: return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
	case -1005: return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
	default: return "Unknown OpenCL error";
	}
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

	const char* errorString = KernelManager_GetCLErrorString(err);
	printf("OpenCL Error Detected:\nError Number:\t%d\nError Name:\n\t%s\nError Description:\n%s\n", err, name, errorString);
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
