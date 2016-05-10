#ifndef RAYTRACERDIRECTIONALSHADOWKERNELPROGRAM_H
#define RAYTRACERDIRECTIONALSHADOWKERNELPROGRAM_H
#include "KernelProgram.h"

#include "../Render/RayBuffer.h"

struct RayTracerDirectionalShadowKernelProgram_ExecutionParameters
{
	RayBuffer* srcBuffer;

	cl_mem* aabbs;
	cl_mem* spheres;

	cl_mem* gObjects;
	cl_mem* materials;
};

///
//Initializes a RayTracerDirectionalShadow Kernel Program
//
//Parameters:
//	prog: A pointer to an uninitialized Kernel Progral to initialize as a RayTracerDirectionalShadow Kernel Program
//	buffer: A pointer to the KernelBuffer containing the context and device on which to build the kernel
void RayTracerDirectionalShadowKernelProgram_Initialize(KernelProgram* prog, struct KernelBuffer* buffer);

#endif
