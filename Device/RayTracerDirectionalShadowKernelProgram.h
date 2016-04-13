#ifndef RAYTRACERDIRECTIONALSHADOWKERNELPROGRAM_H
#define RAYTRACERDIRECTIONALSHADOWKERNELPROGRAM_H
#include "KernelProgram.h"

#include "../Render/RayBuffer.h"

///
//Initializes a RayTracerDirectionalShadow Kernel Program
//
//Parameters:
//	prog: A pointer to an uninitialized Kernel Progral to initialize as a RayTracerDirectionalShadow Kernel Program
//	buffer: A pointer to the KernelBuffer containing the context and device on which to build the kernel
//	rBuffer: A pointer to the RayBuffer cotaining the textures this kernel will use
void RayTracerDirectionalShadowKernelProgram_Initialize(KernelProgram* prog, struct KernelBuffer* buffer, RayBuffer* rBuffer);

#endif
