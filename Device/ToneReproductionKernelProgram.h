#ifndef TONEREPRODUCTIONKERNELPROGRAM_H
#define TONEREPRODUCTIONKERNELPROGRAM_H

#include "KernelProgram.h"
#include "../Render/GlobalBuffer.h"

///
//Initializes a ToneReproductionKernelProgram
//
//Parameters:
//	prog: A pointer to an uninitialized kernel program to initialize as a ToneReproductionKernelProgram
//	buffer: A pointer to the kernel buffer continaing the context and device on which to build the kernel
void ToneReproductionKernelProgram_Initialize(KernelProgram* prog, struct KernelBuffer* buffer);


#endif
