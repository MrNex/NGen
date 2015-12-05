#include "GeometryBuffer.h"

#include <stdlib.h>

#include <stdio.h>

///
//Allocates memory for a GeometryBuffer
//
//Returns:
//	A pointer to an uninitialized GeometryBuffer
GeometryBuffer* GeometryBuffer_Allocate(void)
{
	GeometryBuffer* gBuffer = malloc(sizeof(GeometryBuffer));
	return gBuffer;
}

///
//Initializes a GeometryBuffer
//
//Parameters:
//	gbuffer: Pointer to the GeometryBuffer being initialized
//	textureWidth: The width of the GeometryBuffer's textures
//	textureHeight: The height of the GeometryBuffer's textures
void GeometryBuffer_Initialize(GeometryBuffer* gBuffer, unsigned int textureWidth, unsigned int textureHeight)
{
	//Create a new frame buffer object
	glGenFramebuffers(1, &gBuffer->fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->fbo);

	//Create geometry buffer textures
	glGenTextures(GeometryBuffer_TextureType_NUMTEXTURES, gBuffer->textures);

	///
	//Position Texture
	
	//Initialize position texture
	glBindTexture(GL_TEXTURE_2D, gBuffer->textures[GeometryBuffer_TextureType_POSITION]);
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
	//Attach position texture to frame buffer
	glFramebufferTexture2D
	(
		GL_DRAW_FRAMEBUFFER, 						//Target framebuffer type
		GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_POSITION,	//Attachment point to attach this buffer to
		GL_TEXTURE_2D,							//Target attachment type
		gBuffer->textures[GeometryBuffer_TextureType_POSITION],		//Texture to attach
		0								//Mipmapping level
	);
	
	///
	//Diffuse Texture

	//Initialize diffuse texture
	glBindTexture(GL_TEXTURE_2D, gBuffer->textures[GeometryBuffer_TextureType_DIFFUSE]);
	glTexImage2D
	(
		GL_TEXTURE_2D,		//Type
		0,			//Mipmapping level
		GL_RGB8,		//Internal format of image data (3 components each a 8 bits)
		textureWidth,		//Width of texture
		textureHeight,		//Height of texture
		0,			//Border
		GL_RGB,			//Format of data to output into texture
		GL_UNSIGNED_BYTE,	//Data type of each element in texture
		NULL			//Texture data (We will be generating the texture
	);
	//Attach diffuse texture to frame buffer
	glFramebufferTexture2D
	(
		GL_DRAW_FRAMEBUFFER, 						//Target framebuffer type
		GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_DIFFUSE,	//Attachment point to attach this buffer to
		GL_TEXTURE_2D,							//Target attachment type
		gBuffer->textures[GeometryBuffer_TextureType_DIFFUSE],		//Texture to attach
		0								//Mipmapping level
	);

	///
	//Normal Texture

	//Initialize Normal texture
	glBindTexture(GL_TEXTURE_2D, gBuffer->textures[GeometryBuffer_TextureType_NORMAL]);
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
	//Attach Normal texture to frame buffer
	glFramebufferTexture2D
	(
		GL_DRAW_FRAMEBUFFER, 						//Target framebuffer type
		GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_NORMAL,	//Attachment point to attach this buffer to
		GL_TEXTURE_2D,							//Target attachment type
		gBuffer->textures[GeometryBuffer_TextureType_NORMAL],		//Texture to attach
		0								//Mipmapping level
	);

	///
	//Texture Coordinate Texture

	//Initialize Texture Coordinate texture
	glBindTexture(GL_TEXTURE_2D, gBuffer->textures[GeometryBuffer_TextureType_TEXTURECOORDINATE]);
	glTexImage2D
	(
		GL_TEXTURE_2D,	//Type
		0,		//Mipmapping level
		GL_RG32F,	//Internal format of image data (2 components each a 32 bit single floating point precision)
		textureWidth,	//Width of texture
		textureHeight,	//Height of texture
		0,		//Border
		GL_RG,		//Format of data to output into texture
		GL_FLOAT,	//Data type of each element in texture
		NULL		//Texture data (We will be generating the texture
	);
	//Attach Texture Coordinate texture to frame buffer
	glFramebufferTexture2D
	(
		GL_DRAW_FRAMEBUFFER, 							//Target framebuffer type
		GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_TEXTURECOORDINATE,	//Attachment point to attach this buffer to
		GL_TEXTURE_2D,								//Target attachment type
		gBuffer->textures[GeometryBuffer_TextureType_TEXTURECOORDINATE],	//Texture to attach
		0									//Mipmapping level
	);

	///
	//Depth Texture

	//Initialize Depth texture
	glBindTexture(GL_TEXTURE_2D, gBuffer->textures[GeometryBuffer_TextureType_DEPTH]);
	glTexImage2D
	(
		GL_TEXTURE_2D,		//Type
		0,			//Mipmapping level
		GL_DEPTH_COMPONENT32F,	//Internal format of image data (2 components each a 32 bit single floating point precision)
		textureWidth,		//Width of texture
		textureHeight,		//Height of texture
		0,			//Border
		GL_DEPTH_COMPONENT,	//Format of data to output into texture
		GL_FLOAT,		//Data type of each element in texture
		NULL			//Texture data (We will be generating the texture
	);
	//Attach Depth texture to frame buffer
	glFramebufferTexture2D
	(
		GL_DRAW_FRAMEBUFFER, 					//Target framebuffer type
		GL_DEPTH_ATTACHMENT,					//Attachment point to attach this buffer to
		GL_TEXTURE_2D,						//Target attachment type
		gBuffer->textures[GeometryBuffer_TextureType_DEPTH],	//Texture to attach
		0							//Mipmapping level
	);

	//Draw buffers
	GLenum buffersToDraw[GeometryBuffer_TextureType_NUMTEXTURES];
	for(int i = 0; i < GeometryBuffer_TextureType_NUMTEXTURES; i++)
	{
		buffersToDraw[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	glDrawBuffers(GeometryBuffer_TextureType_NUMTEXTURES, buffersToDraw);

	//Get status of framebuffer
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE)
	{
		printf("Error occurred creating GeometryBuffer: 0x%x\nBuffer was not created!!\n", status);
	}

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	
}

///
//Frees memory allocated by a geometry buffer
//
//Parameters:
//	gBuffer: A pointer to the geometry buffer being freed
void GeometryBuffer_Free(GeometryBuffer* gBuffer)
{
	glDeleteTextures(GeometryBuffer_TextureType_NUMTEXTURES, gBuffer->textures);
	glDeleteFramebuffers(1, &gBuffer->fbo);
	free(gBuffer);
}
