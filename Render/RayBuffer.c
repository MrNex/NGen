#include "RayBuffer.h"

#include <stdlib.h>

#include <stdio.h>

///
//Allocates memory for a RayBuffer
//
//Returns:
//	A pointer to an uninitialized RayBuffer
RayBuffer* RayBuffer_Allocate(void)
{
	RayBuffer* rBuffer = malloc(sizeof(RayBuffer));
	return rBuffer;
}

///
//Initializes a RayBuffer
//
//Parameters:
//	gbuffer: Pointer to the RayBuffer being initialized
//	textureWidth: The width of the RayBuffer's textures
//	textureHeight: The height of the RayBuffer's textures
void RayBuffer_Initialize(RayBuffer* rBuffer, unsigned int textureWidth, unsigned int textureHeight)
{
	rBuffer->textureWidth = textureWidth;
	rBuffer->textureHeight = textureHeight;

	//Create a new frame buffer object
	glGenFramebuffers(1, &rBuffer->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, rBuffer->fbo);

	//Create geometry buffer textures
	glGenTextures(RayBuffer_TextureType_NUMTEXTURES, rBuffer->textures);

	///
	//Position Texture
	
	//Initialize position texture
	glBindTexture(GL_TEXTURE_2D, rBuffer->textures[RayBuffer_TextureType_POSITION]);
	glTexImage2D
	(
		GL_TEXTURE_2D,	//Type
		0,		//Mipmapping level
		GL_RGB32F,	//Internal format of image data (3 components each a 32 bit single floating point precision)
		textureWidth,	//Width of texture
		textureHeight,	//Height of texture
		0,		//Border
		GL_RGB,		//Format of data to output into texture
		GL_FLOAT,	//Data type of each element in texture
		NULL		//Texture data (We will be generating the texture
	);

	//Because the textures will be a 1-1 mapping with the pixels in the window
	//we want to make sure openGL does not attempt an unnecessary interpolation 
	//when shading. 	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Attach position texture to frame buffer
	glFramebufferTexture2D
	(
		GL_FRAMEBUFFER, 						//Target framebuffer type
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_POSITION,	//Attachment point to attach this buffer to
		GL_TEXTURE_2D,							//Target attachment type
		rBuffer->textures[RayBuffer_TextureType_POSITION],		//Texture to attach
		0								//Mipmapping level
	);
	
	///
	//Diffuse Texture

	//Initialize diffuse texture
	glBindTexture(GL_TEXTURE_2D, rBuffer->textures[RayBuffer_TextureType_DIFFUSE]);
	glTexImage2D
	(
		GL_TEXTURE_2D,		//Type
		0,			//Mipmapping level
		GL_RGBA8,		//Internal format of image data (4 components each a 8 bits)
		textureWidth,		//Width of texture
		textureHeight,		//Height of texture
		0,			//Border
		GL_RGBA,			//Format of data to output into texture
		GL_UNSIGNED_BYTE,	//Data type of each element in texture
		NULL			//Texture data (We will be generating the texture
	);

	//Because the textures will be a 1-1 mapping with the pixels in the window
	//we want to make sure openGL does not attempt an unnecessary interpolation 
	//when shading. 	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Attach diffuse texture to frame buffer
	glFramebufferTexture2D
	(
		GL_FRAMEBUFFER, 						//Target framebuffer type
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_DIFFUSE,	//Attachment point to attach this buffer to
		GL_TEXTURE_2D,							//Target attachment type
		rBuffer->textures[RayBuffer_TextureType_DIFFUSE],		//Texture to attach
		0								//Mipmapping level
	);

	///
	//Normal Texture

	//Initialize Normal texture
	glBindTexture(GL_TEXTURE_2D, rBuffer->textures[RayBuffer_TextureType_NORMAL]);
	glTexImage2D
	(
		GL_TEXTURE_2D,	//Type
		0,		//Mipmapping level
		GL_RGB32F,	//Internal format of image data (3 components each a 32 bit single floating point precision)
		textureWidth,	//Width of texture
		textureHeight,	//Height of texture
		0,		//Border
		GL_RGB,		//Format of data to output into texture
		GL_FLOAT,	//Data type of each element in texture
		NULL		//Texture data (We will be generating the texture
	);


	//Because the textures will be a 1-1 mapping with the pixels in the window
	//we want to make sure openGL does not attempt an unnecessary interpolation 
	//when shading. 	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Attach Normal texture to frame buffer
	glFramebufferTexture2D
	(
		GL_FRAMEBUFFER, 						//Target framebuffer type
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_NORMAL,	//Attachment point to attach this buffer to
		GL_TEXTURE_2D,							//Target attachment type
		rBuffer->textures[RayBuffer_TextureType_NORMAL],		//Texture to attach
		0								//Mipmapping level
	);

	///
	//Material Texture
	glBindTexture(GL_TEXTURE_2D, rBuffer->textures[RayBuffer_TextureType_MATERIAL]);
	glTexImage2D
	(
		GL_TEXTURE_2D,
		0,
		GL_RGBA32F,
		textureWidth,
		textureHeight,
		0,
		GL_RGBA,
		GL_FLOAT,
		NULL
	);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D
	(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_MATERIAL,
		GL_TEXTURE_2D,
		rBuffer->textures[RayBuffer_TextureType_MATERIAL],
		0
	);

	///
	//Specular Texture
	glBindTexture(GL_TEXTURE_2D, rBuffer->textures[RayBuffer_TextureType_SPECULAR]);
	glTexImage2D
	(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		textureWidth,
		textureHeight,
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
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_SPECULAR,
		GL_TEXTURE_2D,
		rBuffer->textures[RayBuffer_TextureType_SPECULAR],
		0
	);


	///
	//Shadow Texture
	glBindTexture(GL_TEXTURE_2D, rBuffer->textures[RayBuffer_TextureType_SHADOW]);
	glTexImage2D
	(
		GL_TEXTURE_2D,		//Type
		0,			//Mipmapping level
		GL_R8,			//Internal Format
		textureWidth,
		textureHeight,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		NULL
	);


	//Because the textures will be a 1-1 mapping with the pixels in the window
	//we want to make sure openGL does not attempt an unnecessary interpolation 
	//when shading. 	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Attach shadow texture to frame buffer
	glFramebufferTexture2D
	(
		GL_FRAMEBUFFER,					//Target framebuffer type
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_SHADOW,
		GL_TEXTURE_2D,
		rBuffer->textures[RayBuffer_TextureType_SHADOW],
		0
	);

	///
	//Final texture

	//Initialize final texture
	glBindTexture(GL_TEXTURE_2D, rBuffer->textures[RayBuffer_TextureType_FINAL]);
	glTexImage2D
	(
		GL_TEXTURE_2D,		//Type
		0,			//Mipmapping level
		GL_RGB8,		//Internal format
		textureWidth,
		textureHeight,
		0,			//Border width
		GL_RGB,
		GL_FLOAT,
		NULL
	);

	//Because the textures will be a 1-1 mapping with the pixels in the window
	//we want to make sure openGL does not attempt an unnecessary interpolation 
	//when shading. 	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glFramebufferTexture2D
	(
		GL_FRAMEBUFFER,							//Target framebuffer type
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_FINAL,		//Attachment point
		GL_TEXTURE_2D,								//Attachment type
		rBuffer->textures[RayBuffer_TextureType_FINAL],			//Texture to attach
		0									//mipmapping level
	);


	///
	//Depth Texturencil

	//Initialize Depth/ste texture
	glBindTexture(GL_TEXTURE_2D, rBuffer->textures[RayBuffer_TextureType_DEPTH]);
	glTexImage2D
	(
		GL_TEXTURE_2D,		//Type
		0,			//Mipmapping level
		GL_DEPTH32F_STENCIL8,	//Internal format of image data (2 components each a 32 bit single floating point precision)
		textureWidth,					//Width of texture
		textureHeight,					//Height of texture
		0,						//Border
		GL_DEPTH_STENCIL,				//Format of data to output into texture
		GL_FLOAT_32_UNSIGNED_INT_24_8_REV,		//Data type of each element in texture
		NULL						//Texture data (We will be generating the texture
	);
	//Attach Depth texture to frame buffer
	glFramebufferTexture2D
	(
		GL_FRAMEBUFFER, 					//Target framebuffer type
		GL_DEPTH_STENCIL_ATTACHMENT,				//Attachment point to attach this buffer to
		GL_TEXTURE_2D,						//Target attachment type
		rBuffer->textures[RayBuffer_TextureType_DEPTH],	//Texture to attach
		0							//Mipmapping level
	);

	//Draw buffers
	GLenum buffersToDraw[RayBuffer_TextureType_DEPTH];
	for(int i = 0; i < RayBuffer_TextureType_DEPTH; i++)
	{
		buffersToDraw[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	glDrawBuffers(RayBuffer_TextureType_DEPTH, buffersToDraw);

	//Get status of framebuffer
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error occurred creating RayBuffer: 0x%x\nBuffer was not created!!\n", status);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
}

///
//Frees memory allocated by a geometry buffer
//
//Parameters:
//	rBuffer: A pointer to the geometry buffer being freed
void RayBuffer_Free(RayBuffer* rBuffer)
{
	glDeleteTextures(RayBuffer_TextureType_NUMTEXTURES, rBuffer->textures);
	glDeleteFramebuffers(1, &rBuffer->fbo);
	free(rBuffer);
}

///
//Clears the final texture of the geometry buffer
//
//Parameters:
//	rBuffer: A pointer to the geometry buffer to clear the final texture of
//	type: The type of the texture to clear
void RayBuffer_ClearTexture(RayBuffer* rBuffer, enum RayBuffer_TextureType type)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rBuffer->fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + type);
	glClear(GL_COLOR_BUFFER_BIT);
}

///
//Binds the Frame Buffer Object of the Ray Buffer to bind it's textures to be written to from the PixelProjectionGeometryShaderProgram.
//
//Parameters:
//	rBuffer: A pointer to the geometry buffer being bound for writing
void RayBuffer_BindForGeometryPass(RayBuffer* rBuffer)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rBuffer->fbo);

	GLenum drawBuffers[] = 
	{
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_POSITION,
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_DIFFUSE,
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_NORMAL,
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_MATERIAL,
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_SPECULAR
	};

	//Specify where the fragment shader will write
	glDrawBuffers(5, drawBuffers);
}

///
//Binds the Ray Buffer to be read/written from/to for the shadow pass by the PixelProjectionShadowShaderProgram
//
//Parameters:
//	rBuffer: A pointer to the ray buffer being bound for the shadow pass
void RayBuffer_BindForShadowPass(RayBuffer* rBuffer)
{
	glBindFramebuffer(GL_FRAMEBUFFER, rBuffer->fbo);

	glDrawBuffer(GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_SHADOW);



	glActiveTexture(GL_TEXTURE0 + RayBuffer_TextureType_POSITION);
	glBindTexture(GL_TEXTURE_2D, rBuffer->textures[RayBuffer_TextureType_POSITION]);

	//glActiveTexture(GL_TEXTURE0 + RayBuffer_TextureType_SHADOW);
	//glBindTexture(GL_TEXTURE_2D, rBuffer->textures[RayBuffer_TextureType_SHADOW]);
}

///
//Binds the ray buffer for use with the RayTracerShadowKernelProgram
void RayBuffer_BindForShadowKernel()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

///
//Binds the ray buffer to have it's stencil texture written to from the DeferredStencilShaderProgram
//
//Parameters:
//	rBuffer: A pointer to the ray buffer being bound for the stencil pass
void RayBuffer_BindForStencilPass(RayBuffer* rBuffer)
{
	//Just in case
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rBuffer->fbo);

	//We will use a null fragment shader for the stencil pass.
	glDrawBuffer(GL_NONE);
}


///
//Binds the Frame Buffer Object of the geometry buffer to bind it's textures to be read from the PixelProjection lighting shaders
// 
//Parameters:
//	rBuffer: A pointer to the geometry buffer being bound for reading
void RayBuffer_BindForLightPass(RayBuffer* rBuffer)
{
	//Just in case
	glBindFramebuffer(GL_FRAMEBUFFER, rBuffer->fbo);

	//Draw to the final buffer!
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_FINAL);

	//Bind Position - Specular
	for(unsigned int i = 0; i < RayBuffer_TextureType_FINAL; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, rBuffer->textures[i]);
	}
}

///
//Binds the FrameBufferObject of the geometry buffer to be read for the final pass
//
//Parameters:
//	rBuffer: A pointer to the geometry buffer being bound for the final pass
void RayBuffer_BindForFinalPass(RayBuffer* rBuffer)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);	//Bind default frame buffer for writing
	glBindFramebuffer(GL_READ_FRAMEBUFFER, rBuffer->fbo);	//Bind rBuffer's final texture for blitting
	glReadBuffer(GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_FINAL);
}

///
//Allocates the necessary memory to hold the data contained in a texture image.
//
//Parameters:
//	buffer: A pointer to the ray buffer which the target texture belongs to.
//	type: The target texture type
//
//Returns:
//	A pointer to uninitialized data of length necessary to hold the desired texture.
//	WARNING: Remember to free this
void* RayBuffer_AllocateTextureData(RayBuffer* buffer, enum RayBuffer_TextureType type)
{
	//Get number of components and depth of each component in bytes
	int components = 0, depth = 0;
	switch(type)
	{
	case RayBuffer_TextureType_POSITION:
	case RayBuffer_TextureType_NORMAL:
		components = 3;
		depth = 4;
		break;
	case RayBuffer_TextureType_DIFFUSE:
	case RayBuffer_TextureType_SPECULAR:
	case RayBuffer_TextureType_FINAL:
		components = 4;
		depth = 1;
		break;
	case RayBuffer_TextureType_SHADOW:
		components = 1;
		depth = 1;
		break;
	default:
		components = 0;
		depth = 0;
		break;	
	}

	int size = buffer->textureWidth * buffer->textureHeight * components * depth;

	return size > 0 ?//If
		malloc(size) :	//True
		NULL;		//False
}

static GLenum RayBuffer_DetermineTextureFormat(enum RayBuffer_TextureType type)
{
	switch(type)
	{
	case RayBuffer_TextureType_POSITION:
	case RayBuffer_TextureType_NORMAL:
		return GL_RGB;
		break;
	case RayBuffer_TextureType_DIFFUSE:
	case RayBuffer_TextureType_SPECULAR:
	case RayBuffer_TextureType_FINAL:
		return GL_RGBA;
		break;
	case RayBuffer_TextureType_SHADOW:
		return GL_RED;
		break;
	default:
		return 0;
		break;	
	}
}

static GLenum RayBuffer_DetermineTexturePixelType(enum RayBuffer_TextureType type)
{
	switch(type)
	{
	case RayBuffer_TextureType_POSITION:
	case RayBuffer_TextureType_NORMAL:
		return GL_FLOAT;
		break;
	case RayBuffer_TextureType_DIFFUSE:
	case RayBuffer_TextureType_SPECULAR:
	case RayBuffer_TextureType_FINAL:
		return GL_FLOAT;
		break;
	case RayBuffer_TextureType_SHADOW:
		return GL_UNSIGNED_BYTE;
		break;
	default:
		return 0;
		break;	
	}
}

///
//Reads the data from the bound texture storing it in the specified destination.
//NOTE: Texture must be bound first
//
//Parameters:
//	dest: a pointer to the contiguous memory in which to store the texture data
//	type: The type of the texture to get 
void RayBuffer_GetTextureData(void* dest, enum RayBuffer_TextureType type)
{
	GLenum format = RayBuffer_DetermineTextureFormat(type);
	GLenum returnType = RayBuffer_DetermineTexturePixelType(type);;
	
	glGetTexImage(GL_TEXTURE_2D, 0, format, returnType, dest);
	
}

///
//Writes data to the bound texture
//NOTE: Texture specified by type must be bound before calling this function
//
//Parameters:
//	buffer: A pointer to the ray buffer which contains the texture being written to
//	type: The type of texture being written to
//	data: A pointer to the data to write
void RayBuffer_WriteTextureData(RayBuffer* buffer, enum RayBuffer_TextureType type, void* data)
{
	
	GLenum format = RayBuffer_DetermineTextureFormat(type);
	GLenum pixelType = RayBuffer_DetermineTexturePixelType(type);;
	
	glTexSubImage2D
	(
		GL_TEXTURE_2D,
		0,
		0,
		0,
		buffer->textureWidth,
		buffer->textureHeight,
		format,
		pixelType,
		data
	);
}

