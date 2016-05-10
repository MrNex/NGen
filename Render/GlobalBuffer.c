#include "GlobalBuffer.h"

#include <stdlib.h>
#include <stdio.h>

#include "../Manager/KernelManager.h"

static const char* GlobalBuffer_TextureTypeStrings[GlobalBuffer_TextureType_NUMTEXTURES] =
{
	"Local", "Reflection", "Transmission", "GlobalMaterial", "TransMaterial", "Final"
};

///
//Allocates a new GlobalBuffer
//
//Returns:
//	A pointer to a newly allocated uninitialized Global Buffer
GlobalBuffer* GlobalBuffer_Allocate()
{
	return malloc(sizeof(GlobalBuffer));
}

///
//Initializes a Global Buffer
//
//Parameters:
//	gBuffer: A pointer to the global buffer to allocate
//	tWidth: The texture width of the textures in the global buffer
//	tHeight: The texture height of the textures within the globalBuffer
void GlobalBuffer_Initialize(GlobalBuffer* gBuffer, unsigned int tWidth, unsigned int tHeight)
{
	gBuffer->textureWidth = tWidth;
	gBuffer->textureHeight = tHeight;

	//Create a new frame buffer object
	glGenFramebuffers(1, &gBuffer->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->fbo);

	//Create geometry buffer textures
	glGenTextures(GlobalBuffer_TextureType_NUMTEXTURES, gBuffer->textures);

	//Initialize local texture
	glBindTexture(GL_TEXTURE_2D, gBuffer->textures[GlobalBuffer_TextureType_LOCAL]);
	glTexImage2D
	(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		gBuffer->textureWidth,
		gBuffer->textureHeight,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		NULL
	);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D
	(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_LOCAL,
		GL_TEXTURE_2D,
		gBuffer->textures[GlobalBuffer_TextureType_LOCAL],
		0
	);

	//Initialize Reflection Texture
	glBindTexture(GL_TEXTURE_2D, gBuffer->textures[GlobalBuffer_TextureType_REFLECTION]);
	glTexImage2D
	(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		gBuffer->textureWidth,
		gBuffer->textureHeight,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		NULL
	);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D
	(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_REFLECTION,
		GL_TEXTURE_2D,
		gBuffer->textures[GlobalBuffer_TextureType_REFLECTION],
		0
	);

	//Initialize transmission texture
	glBindTexture(GL_TEXTURE_2D, gBuffer->textures[GlobalBuffer_TextureType_TRANSMISSION]);
	glTexImage2D
	(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		gBuffer->textureWidth,
		gBuffer->textureHeight,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		NULL
	);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D
	(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_TRANSMISSION,
		GL_TEXTURE_2D,
		gBuffer->textures[GlobalBuffer_TextureType_TRANSMISSION],
		0
	);

	//GlobalMaterial Texture initialization
	glBindTexture(GL_TEXTURE_2D, gBuffer->textures[GlobalBuffer_TextureType_GLOBALMATERIAL]);
	glTexImage2D
	(
		GL_TEXTURE_2D,
		0,
		GL_RGB32F,
		gBuffer->textureWidth,
		gBuffer->textureHeight,
		0,
		GL_RGB,
		GL_FLOAT,
		NULL
	);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D
	(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_GLOBALMATERIAL,
		GL_TEXTURE_2D,
		gBuffer->textures[GlobalBuffer_TextureType_GLOBALMATERIAL],
		0
	);

	//TransMaterial Texture initialization
	glBindTexture(GL_TEXTURE_2D, gBuffer->textures[GlobalBuffer_TextureType_TRANSMATERIAL]);
	glTexImage2D
	(
		GL_TEXTURE_2D,
		0,
		GL_RGB32F,
		gBuffer->textureWidth,
		gBuffer->textureHeight,
		0,
		GL_RGB,
		GL_FLOAT,
		NULL
	);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D
	(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_TRANSMATERIAL,
		GL_TEXTURE_2D,
		gBuffer->textures[GlobalBuffer_TextureType_TRANSMATERIAL],
		0
	);
	//Final texture initialization
	glBindTexture(GL_TEXTURE_2D, gBuffer->textures[GlobalBuffer_TextureType_FINAL]);
	glTexImage2D
	(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		gBuffer->textureWidth,
		gBuffer->textureHeight,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		NULL
	);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D
	(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_FINAL,
		GL_TEXTURE_2D,
		gBuffer->textures[GlobalBuffer_TextureType_FINAL],
		0
	);

	KernelBuffer* kBuf = KernelManager_GetKernelBuffer();

	cl_int err = 0;
	for(unsigned int i = 0; i < GlobalBuffer_TextureType_NUMTEXTURES; i++)
	{
		gBuffer->textureRefs[i] = clCreateFromGLTexture
		(
			kBuf->clContext,
			CL_MEM_WRITE_ONLY,
			GL_TEXTURE_2D,
			0,
			gBuffer->textures[i],
			&err
		);
		KernelManager_CheckCLErrors(err, "GlobalBuffer_Initialize :: clCreateFromGLTexture :: i");
	}
	/*

	gBuffer->textureRefs[1] = clCreateFromGLTexture
	(
		kBuf->clContext,
		CL_MEM_WRITE_ONLY,
		GL_TEXTURE_2D,
		0,
		gBuffer->textures[GlobalBuffer_TextureType_TRANSMISSION],
		&err
	);
	KernelManager_CheckCLErrors(err, "GlobalBuffer_Initialize :: clCreateFromGLTexture :: transmission");
	*/
}

///
//Frees memory used by a globalBuffer
//
//Parameters:
//	gBuffer: A pointer to the global buffer to free
void GlobalBuffer_Free(GlobalBuffer* gBuffer)
{
	cl_int err = 0;
	for(unsigned int i = 0; i < GlobalBuffer_TextureType_NUMTEXTURES; i++)
	{
		err = clReleaseMemObject(gBuffer->textureRefs[i]);
		KernelManager_CheckCLErrors(err, "GlobalBuffer_Free :: clReleaseMemObject :: i");
	}

/*	
	err = clReleaseMemObject(gBuffer->textureRefs[1]);
	KernelManager_CheckCLErrors(err, "GlobalBuffer_Free :: clReleaseMemObject :: 1 - transmission");

	*/

	glDeleteTextures(GlobalBuffer_TextureType_NUMTEXTURES, gBuffer->textures);
	glDeleteFramebuffers(1, &gBuffer->fbo);
	free(gBuffer);
}

///
//Clears a texture attached to the global buffer
//
//Parameters:
//	gBuffer: A pointer to the globalBffer having one of it's textures cleared
//	type: The type of the texture to clear
void GlobalBuffer_ClearTexture(GlobalBuffer* gBuffer, enum GlobalBuffer_TextureType type)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + type);
	glClear(GL_COLOR_BUFFER_BIT);
}

///
//Binds the global buffer to be read/written from/to OpenCL Kernel programs
//
//Parameters:
//	gBuffer: A pointer to the global buffer to bind for kernel use
//	event A pointer to a cl_event which will be filled with an event trackig progress of buffer binding
void GlobalBuffer_BindForKernels(GlobalBuffer* gBuffer, cl_event* event)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	KernelBuffer* kBuf = KernelManager_GetKernelBuffer();

	cl_int err = 0;
	err = clEnqueueAcquireGLObjects
	(
		kBuf->clQueue,
		GlobalBuffer_TextureType_NUMTEXTURES,
		&gBuffer->textureRefs[0],
		0,
		NULL,
		event
	);
	KernelManager_CheckCLErrors(err, "GlobalBuffer_BindForKernels :: clEnqueueAcquireGLObjects");
}

///
//Releases the global buffer textures from openCL kernel use
//
//Parameters:
//	gBuffer: A pointer to the global buffer to release from kernel use
//	event: A pointer to an event which will be tracking the progress of buffer release
void GlobalBuffer_ReleaseFromKernels(GlobalBuffer* gBuffer, cl_event* event)
{
	KernelBuffer* kBuf = KernelManager_GetKernelBuffer();

	cl_int err = 0;
	err = clEnqueueReleaseGLObjects
	(
		kBuf->clQueue, 
		GlobalBuffer_TextureType_NUMTEXTURES, 
		&gBuffer->textureRefs[0], 
		0, 
		NULL, 
		event
	);
	KernelManager_CheckCLErrors(err, "GlobalBuffer_ReleaseFromKernels :: clEnqueueReleaseGLObjects");
}

///
//Binds the global buffer for the local illumination pass
//
//Parameters:
//	gBuffer: A pointer to the global buffer to bind for the local illumination pass
void GlobalBuffer_BindForLocalPass(GlobalBuffer* gBuffer, enum GlobalBuffer_TextureType type)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->fbo);

	GLenum drawBuffers[] =
	{
		GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_LOCAL,
		GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_REFLECTION,
		GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_TRANSMISSION,
		GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_GLOBALMATERIAL,
		GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_TRANSMATERIAL
	};

	glDrawBuffers(5, drawBuffers);
}

///
//Binds the global buffer for the final pass
//
//Parameters:
//	gBuffer: A pointer to the global buffer to bind for the final pass
void GlobalBuffer_BindForFinalPass(GlobalBuffer* gBuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer->fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_FINAL);

	for(unsigned int i = 0; i < GlobalBuffer_TextureType_NUMTEXTURES; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gBuffer->textures[i]);
	}
}

///
//Binds the globalBuffer for the blitting of the final texture to the screen
//
//Parameters:
//	gBuffer: A pointer to the global buffer to bind for display
void GlobalBuffer_BindForDisplay(GlobalBuffer* gBuffer)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + GlobalBuffer_TextureType_FINAL);
}
