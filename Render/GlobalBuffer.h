#ifndef GLOBALBUFFER_H
#define GLOBALBUFFER_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <CL/cl.h>
#include <CL/cl_gl.h>

///
//Defines the types of textures a global buffer can contain
enum GlobalBuffer_TextureType
{
	GlobalBuffer_TextureType_LOCAL,		//RGBA8
	GlobalBuffer_TextureType_REFLECTION,	//RGBA8
	GlobalBuffer_TextureType_TRANSMISSION,	//RGBA8
	GlobalBuffer_TextureType_GLOBALMATERIAL,//RGB32F
	GlobalBuffer_TextureType_TRANSMATERIAL,	//RGB32FF
	GlobalBuffer_TextureType_FINAL,
	GlobalBuffer_TextureType_NUMTEXTURES
};

typedef struct GlobalBuffer
{
	GLuint fbo;
	GLuint textures[GlobalBuffer_TextureType_NUMTEXTURES];
	cl_mem textureRefs[GlobalBuffer_TextureType_NUMTEXTURES];	//Reflection and Transmission will be accessed by kernels
	unsigned int textureWidth, textureHeight;
} GlobalBuffer;

///
//Allocates a new GlobalBuffer
//
//Returns:
//	A pointer to a newly allocated uninitialized Global Buffer
GlobalBuffer* GlobalBuffer_Allocate();

///
//Initializes a Global Buffer
//
//Parameters:
//	gBuffer: A pointer to the global buffer to allocate
//	tWidth: The texture width of the textures in the global buffer
//	tHeight: The texture height of the textures within the globalBuffer
void GlobalBuffer_Initialize(GlobalBuffer* gBuffer, unsigned int tWidth, unsigned int tHeight);

///
//Frees memory used by a globalBuffer
//
//Parameters:
//	gBuffer: A pointer to the global buffer to free
void GlobalBuffer_Free(GlobalBuffer* gBuffer);

///
//Clears a texture attached to the global buffer
//
//Parameters:
//	gBuffer: A pointer to the globalBffer having one of it's textures cleared
//	type: The type of the texture to clear
void GlobalBuffer_ClearTexture(GlobalBuffer* gBuffer, enum GlobalBuffer_TextureType type);

///
//Binds the global buffer to be read/written from/to OpenCL Kernel programs
//
//Parameters:
//	gBuffer: A pointer to the global buffer to bind for kernel use
//	event A pointer to a cl_event which will be filled with an event trackig progress of buffer binding
void GlobalBuffer_BindForKernels(GlobalBuffer* gBuffer, cl_event* event);

///
//Releases the global buffer textures from openCL kernel use
//
//Parameters:
//	gBuffer: A pointer to the global buffer to release from kernel use
//	event: A pointer to an event which will be tracking the progress of buffer release
void GlobalBuffer_ReleaseFromKernels(GlobalBuffer* gBuffer, cl_event* event);

///
//Binds the global buffer for the local illumination pass
//
//Parameters:
//	gBuffer: A pointer to the global buffer to bind for the local illumination pass
void GlobalBuffer_BindForLocalPass(GlobalBuffer* gBuffer, enum GlobalBuffer_TextureType type);

///
//Binds the global buffer for the final pass
//
//Parameters:
//	gBuffer: A pointer to the global buffer to bind for the final pass
void GlobalBuffer_BindForFinalPass(GlobalBuffer* gBuffer);

///
//Binds the globalBuffer for the blitting of the final texture to the screen
//
//Parameters:
//	gBuffer: A pointer to the global buffer to bind for display
void GlobalBuffer_BindForDisplay(GlobalBuffer* gBuffer);

#endif
