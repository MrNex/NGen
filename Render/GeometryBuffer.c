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
	gBuffer->textureWidth = textureWidth;
	gBuffer->textureHeight = textureHeight;

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

	//Because the textures will be a 1-1 mapping with the pixels in the window
	//we want to make sure openGL does not attempt an unnecessary interpolation 
	//when shading. 	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

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


	//Because the textures will be a 1-1 mapping with the pixels in the window
	//we want to make sure openGL does not attempt an unnecessary interpolation 
	//when shading. 	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Attach Normal texture to frame buffer
	glFramebufferTexture2D
	(
		GL_DRAW_FRAMEBUFFER, 						//Target framebuffer type
		GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_NORMAL,	//Attachment point to attach this buffer to
		GL_TEXTURE_2D,							//Target attachment type
		gBuffer->textures[GeometryBuffer_TextureType_NORMAL],		//Texture to attach
		0								//Mipmapping level
	);

	//Not really needed because of 1 to 1 mapping between window size and texture size
	//Consider for split screen applications
	///
	//Texture Coordinate Texture
	/*
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

	//Because the textures will be a 1-1 mapping with the pixels in the window
	//we want to make sure openGL does not attempt an unnecessary interpolation 
	//when shading. 	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//Attach Texture Coordinate texture to frame buffer
	glFramebufferTexture2D
	(
		GL_DRAW_FRAMEBUFFER, 							//Target framebuffer type
		GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_TEXTURECOORDINATE,	//Attachment point to attach this buffer to
		GL_TEXTURE_2D,								//Target attachment type
		gBuffer->textures[GeometryBuffer_TextureType_TEXTURECOORDINATE],	//Texture to attach
		0									//Mipmapping level
	);*/


	///
	//Final texture

	//Initialize final texture
	glBindTexture(GL_TEXTURE_2D, gBuffer->textures[GeometryBuffer_TextureType_FINAL]);
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
		GL_DRAW_FRAMEBUFFER,							//Target framebuffer type
		GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_FINAL,		//Attachment point
		GL_TEXTURE_2D,								//Attachment type
		gBuffer->textures[GeometryBuffer_TextureType_FINAL],			//Texture to attach
		0									//mipmapping level
	);


	///
	//Depth Texturencil

	//Initialize Depth/ste texture
	glBindTexture(GL_TEXTURE_2D, gBuffer->textures[GeometryBuffer_TextureType_DEPTH]);
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
		GL_DRAW_FRAMEBUFFER, 					//Target framebuffer type
		GL_DEPTH_STENCIL_ATTACHMENT,				//Attachment point to attach this buffer to
		GL_TEXTURE_2D,						//Target attachment type
		gBuffer->textures[GeometryBuffer_TextureType_DEPTH],	//Texture to attach
		0							//Mipmapping level
	);

	//Draw buffers
	GLenum buffersToDraw[GeometryBuffer_TextureType_DEPTH];
	for(int i = 0; i < GeometryBuffer_TextureType_DEPTH; i++)
	{
		buffersToDraw[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	glDrawBuffers(GeometryBuffer_TextureType_DEPTH, buffersToDraw);

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

///
//Clears the final texture of the geometry buffer
//
//Parameters:
//	gBuffer: A pointer to the geometry buffer to clear the final texture of
void GeometryBuffer_ClearFinalTexture(GeometryBuffer* gBuffer)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->fbo);
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_FINAL);
	glClear(GL_COLOR_BUFFER_BIT);
}

///
//Binds the Frame Buffer Object of the geometry buffer to bind it's textures to be read from the shaders
//
//Parameters:
//	gBuffer: A pointer to the geometry buffer being bound for reading
void GeometryBuffer_BindForReading(GeometryBuffer* gBuffer)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	//Never bind the depth texture
	for(unsigned int i = 0; i < GeometryBuffer_TextureType_DEPTH; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gBuffer->textures[i]);
	}
	
}

///
//Binds the Frame Buffer Object of the Geometry Buffer to bind it's textures to be written to from the shaders.
//
//Parameters:
//	gBuffer: A pointer to the geometry buffer being bound for writing
void GeometryBuffer_BindForWriting(GeometryBuffer* gBuffer)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->fbo);
	
}

///
//Binds the Frame Buffer Object of the Geometry Buffer to bind it's textures to be written to from the DeferredGeometryShaderProgram.
//
//Parameters:
//	gBuffer: A pointer to the geometry buffer being bound for writing
void GeometryBuffer_BindForGeometryPass(GeometryBuffer* gBuffer)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->fbo);

	GLenum drawBuffers[] = 
	{
		GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_POSITION,
		GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_DIFFUSE,
		GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_NORMAL
	};

	//Specify where the fragment shader will write
	glDrawBuffers(3, drawBuffers);
}

///
//Binds the geometry buffer to have it's stencil texture written to from the DeferredStencilShaderProgram
//
//Parameters:
//	gBuffer: A pointer to the geometry buffer being bound for the stencil pass
void GeometryBuffer_BindForStencilPass(GeometryBuffer* gBuffer)
{
	//Just in case
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->fbo);

	//We will use a null fragment shader for the stencil pass.
	glDrawBuffer(GL_NONE);
}

///
//Binds the Frame Buffer Object of the geometry buffer to bind it's textures to be read from the Deferred lighting shaders
// 
//Parameters:
//	gBuffer: A pointer to the geometry buffer being bound for reading
void GeometryBuffer_BindForLightPass(GeometryBuffer* gBuffer)
{
	//Just in case
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, gBuffer->fbo);

	//Draw to the final buffer!
	glDrawBuffer(GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_FINAL);

	//Bind Position - Normal
	for(unsigned int i = 0; i < GeometryBuffer_TextureType_FINAL; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, gBuffer->textures[i]);
	}
}

///
//Binds the FrameBufferObject of the geometry buffer to be read for the final pass
//
//Parameters:
//	gBuffer: A pointer to the geometry buffer being bound for the final pass
void GeometryBuffer_BindForFinalPass(GeometryBuffer* gBuffer)
{
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);	//Bind default frame buffer for writing
	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer->fbo);	//Bind gBuffer's final texture for blitting
	glReadBuffer(GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_FINAL);
}


