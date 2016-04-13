#ifndef RAYTRACERPOINTSHADOWKERNELPROGRAM_H
#define RAYTRACERPOINTSHADOWKERNELPROGRAM_H

#include "KernelProgram.h"

#include "../Render/RayBuffer.h"
#include "../GObject/GObject.h"

struct RayTracerPointShadowKernelProgram_ExecutionParameters
{
	RayBuffer* buffer;
	GObject* light;
};

///
//Initializes a rayTracerPointShadowKernelProgram
//
//Parameters:
//	prog: A pointer to an uninitialized  kernel program to initialize as a RayTracerPointShadowRayKernelProgram
//	buffer: A pointer to the kernel buffer containing the context and device on which to build this kernel
//	rBuffer: A pointer to the RayBuffer containing the textures this kernel will use
void RayTracerPointShadowKernelProgram_Initialize(KernelProgram* prog, struct KernelBuffer* buffer, RayBuffer* rBuffer);

#endif
