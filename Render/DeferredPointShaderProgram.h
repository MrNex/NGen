#ifndef DEFERREDPOINTSHADERPROGRAM_H
#define DEFERREDPOINTSHADERPROGRAM_H

#include "ShaderProgram.h"

#include "GeometryBuffer.h"


///
//Initializes the deferred point light shader program
//
//Parameters:
//	prog: A pointer to the shader program to initialize as a deferred point shader program
void DeferredPointShaderProgram_Initialize(ShaderProgram* prog);

#endif
