#ifndef RAYTRACERTRANSMISSIONKERNELPROGRAM_H
#define RAYTRACERTRANSMISSIONKERNELPROGRAM_H

#include "KernelProgram.h"
#include "../Render/RayBuffer.h"

struct RayTracerTransmissionKernelProgram_ExecutionParameters
{

	RayBuffer* srcBuffer;
	RayBuffer* dstBuffer;
	
	cl_mem* aabbs;
	cl_mem* spheres;

	cl_mem* sPosition;
	cl_mem* aPosition;
	cl_mem* sDiffuse;
	cl_mem* aDiffuse;
	cl_mem* sNormal;
	cl_mem* aNormal;
	cl_mem* sMaterial;
	cl_mem* aMaterial;
	cl_mem* sSpecular;
	cl_mem* aSpecular;
	cl_mem* sDepth;
	cl_mem* aDepth;

	cl_mem* cameraPosition;
	cl_mem* gObjects;
	cl_mem* materials;
	cl_mem* sphereTransformations;
};

///
//Initializes a RayTracerTransmissionKernelProgram
//
//Parameters:
//	prog: A pointer to an uninitialized kernel program to initialize as a ray tracer transmission kernel program
//	buffer: A pointer to the kernel buffer containing the context and device on which to build the kernel
void RayTracerTransmissionKernelProgram_Initialize(KernelProgram* prog, struct KernelBuffer* buffer);

#endif
