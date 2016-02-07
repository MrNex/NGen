#ifndef DEFERREDPOINTSHADERPROGRAM_H
#define DEFERREDPOINTSHADERPROGRAM_H

#include "ShaderProgram.h"

///
//Initializes the deferred point light shader program
//
//Parameters:
//	prog: A pointer to the shader program to initialize as a deferred point shader program
void DeferredPointShaderProgram_Initialize(ShaderProgram* prog);

///
//Sets the uniform variables needed by this shader program
//which do not change between meshes
//
//Parameters:
//	prog: A pointer to the deferred directional shader program to set the constant uniforms of
//	buffer: A pointer to the rendering buffer to get the uniform values of
//	gBuffer: A pointer to the geometry buffer containing the results from the geometry pass.
void DeferredPointShaderProgram_SetConstantUniforms(ShaderProgram* prog);



#endif
