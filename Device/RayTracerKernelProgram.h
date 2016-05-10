#ifndef RAYTRACERKERNELPROGRAM_H
#define RAYTRACERKERNELPROGRAM_H

#include "KernelProgram.h"
#include "../Render/RayBuffer.h"
#include "../Render/GlobalBuffer.h"

struct RayTracerKernelProgram_ExecutionParameters
{
	RayBuffer* srcBuffer;
	GlobalBuffer* destBuffer;

	cl_mem* aabbs;
	cl_mem* spheres;

	cl_mem* cameraPosition;
	cl_mem* gObjects;
	cl_mem* materials;
	cl_mem* sphereTransformations;
	
};

///
//Initializes a RayTracerKernelProgram
//
//Parameters:
//	prog: A pointer to an uninitialized kernel program to initialize as a RayTracerKernelProgram
//	buffer: A pointer to the kernel buffer continaing the context and device on which to build the kernel
void RayTracerKernelProgram_Initialize(KernelProgram* prog, struct KernelBuffer* buffer);

#endif
