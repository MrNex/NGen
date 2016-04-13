#ifndef RAYTRACERSHADOWSHADERPROGRAM_H
#define RAYTRACERSHADOWSHADERPROGRAM_H

#include "../Manager/RenderingManager.h"

///
//Computes the shadow texture for the RayTracerRenderPipeline
//
//Parameters:
//	shadowDest: A pointer to an array of bytes in which to store the shadow texture
//	buffer: A pointer to the active rendering buffer
//	positionData: A pointer to the memory containing the position of the surfaces at every pixel on the screen
//	textureWidth: The width of the shadow and position textures
//	textureHeight: The height of the shadow and position textures
//	gObjects: A pointer to a linked list containing the GObjects in the scene
void RayTracerShadowShaderProgram_ComputeShadowTexture(unsigned char* shadowDest, struct RenderingBuffer* buffer, float* positionData, int textureWidth, int textureHeight, LinkedList* gObjects);

#endif
