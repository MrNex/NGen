#include "RayBuffer.h"

#include <stdlib.h>

#include <stdio.h>

#include "../Manager/AssetManager.h"
#include "../Manager/KernelManager.h"
#include "../Manager/CollisionManager.h"

///
//Initializes the references to the raybuffer textures from OpenCL kernel memory
//
//Parameters:
//	rBuffer: A pointer to the ray buffer to initialize the references of
static void RayBuffer_InitializeCLReferences(RayBuffer* rBuffer);

static const char* RayBuffer_TextureTypeStrings[RayBuffer_TextureType_NUMTEXTURES] = 
{
	"Position", "Diffuse", "Normal", "MaterialLocal", "Specular", "Shadow", "GlobalMaterial", "Depth"
};

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
	rBuffer->passType[0] = rBuffer->passType[1] = rBuffer->passType[2] = 0;
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
	//Local Material Texture
	glBindTexture(GL_TEXTURE_2D, rBuffer->textures[RayBuffer_TextureType_LOCALMATERIAL]);
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
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_LOCALMATERIAL,
		GL_TEXTURE_2D,
		rBuffer->textures[RayBuffer_TextureType_LOCALMATERIAL],
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
	//Global Material Texture
	glBindTexture(GL_TEXTURE_2D, rBuffer->textures[RayBuffer_TextureType_GLOBALMATERIAL]);
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
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_GLOBALMATERIAL,
		GL_TEXTURE_2D,
		rBuffer->textures[RayBuffer_TextureType_GLOBALMATERIAL],
		0
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


	//Create references to textures from CL memory
	RayBuffer_InitializeCLReferences(rBuffer);

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

///
//Helper function to initialize a CL ray buffer texture from gl ray buffer texture
//
//Parameters:
//	kBuf: Pointer to the Kernel Buffer containing the context and device on which to store this memory
//	rBuffer: A pointer to the ray buffer to initialize the CL reference of
//	type: The type of texture to initialize a reference to
static void RayBuffer_InitializeCLReference(KernelBuffer* kBuf, RayBuffer* rBuffer, const enum RayBuffer_TextureType type)
{
	cl_int err = 0;
	rBuffer->textureRefs[type] = clCreateFromGLTexture
	(
		kBuf->clContext,
		CL_MEM_READ_WRITE,
		GL_TEXTURE_2D,
		0,
		rBuffer->textures[type],
		&err
	);
	const size_t len1 = strlen("RayBuffer_InitializeCLReference :: ");
	const size_t len2 = strlen(RayBuffer_TextureTypeStrings[type]);
	const size_t len = len1 + len2 + 1;

	char finalString[len];
       	sprintf(finalString, "RayBuffer_InitializeCLReference :: %s", RayBuffer_TextureTypeStrings[type]);
	KernelManager_CheckCLErrors(err, finalString);
}

//////
//Initializes the references to the raybuffer textures from OpenCL kernel memory
//
//Parameters:
//	rBuffer: A pointer to the ray buffer to initialize the references of
static void RayBuffer_InitializeCLReferences(RayBuffer* rBuffer)
{
	KernelBuffer* kBuf = KernelManager_GetKernelBuffer();

	for(int i = 0; i < RayBuffer_TextureType_DEPTH; i++)
	{
		RayBuffer_InitializeCLReference(kBuf, rBuffer, i);
	};
}

///
//Converts a texture type enum value to a string
//
//Parameters:
//	type: The type to conver tot a string
//
//Returns:
//	A pointer to a null terminated constant character array containing the string
const char* RayBuffer_TextureTypeToString(enum RayBuffer_TextureType type)
{
	return RayBuffer_TextureTypeStrings[type];
}

//Frees memory allocated by a geometry buffer
//
//Parameters:
//	rBuffer: A pointer to the geometry buffer being freed
void RayBuffer_Free(RayBuffer* rBuffer)
{
	cl_int err = 0;
	//const size_t len = (sizeof("RayBuffer_Free :: clReleaseMemObject :: ###") + 1)/sizeof(char);
	char errorString[(sizeof("RayBuffer_Free :: clReleaseMemObject :: ###") + 1)/sizeof(char)] = { '0' };
	for(int i = 0; i < RayBuffer_TextureType_DEPTH; i++)
	{
		err = clReleaseMemObject(rBuffer->textureRefs[i]);
		sprintf(errorString, "RayBuffer_Free :: clReleaseMemObject :: %d", i);
		KernelManager_CheckCLErrors(err, errorString);
	}
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
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_LOCALMATERIAL,
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_SPECULAR,
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_SHADOW,
		GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_GLOBALMATERIAL
	};

	//Specify where the fragment shader will write
	glDrawBuffers(7, drawBuffers);
}


///
//Binds the ray buffer for use with KernelPrograms
//
//Parameters:
//	rBuffer: A pointer to the ray buffer to bind for kernel use
void RayBuffer_BindForKernels(RayBuffer* rBuffer, cl_event* event)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	
	KernelBuffer* kBuf = KernelManager_GetKernelBuffer();
	
	cl_int err = 0;

	
	err = clEnqueueAcquireGLObjects
	(
		kBuf->clQueue,
		RayBuffer_TextureType_DEPTH,
		&rBuffer->textureRefs[0],
		0,
		NULL,
		event
	);
	KernelManager_CheckCLErrors(err, "RayBuffer_BindForKernels :: clEnqueueAcquireGLObjects");
	
	
}

///
//Releases the ray buffer textures from openCL kernel use
//
//Parameters:
//	rBuffer: A pointer to the ray buffer to release from kernel use
//	event: A pointer to an event which will be tracking the progress of buffer release
void RayBuffer_ReleaseFromKernels(RayBuffer* rBuffer, cl_event* event)
{
	
	KernelBuffer* kBuf = KernelManager_GetKernelBuffer();
	
	cl_int err = 0;
	err = clEnqueueReleaseGLObjects(kBuf->clQueue, RayBuffer_TextureType_DEPTH, &rBuffer->textureRefs[0], 0, NULL, event);

	KernelManager_CheckCLErrors(err, "RayBuffer_ReleaseFromKernels :: clEnqueueReleaseGLObjects");
	
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
	//glDrawBuffer(GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_FINAL);

	//Bind Position - Specular
	for(unsigned int i = 0; i < RayBuffer_TextureType_GLOBALMATERIAL; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, rBuffer->textures[i]);
	}

}

///
//Binds the ray buffer for the local illumination pass
//
//Parameters:
//	rBuffer: A ointer to the ray buffer to bind
void RayBuffer_BindForLocalPass(RayBuffer* rBuffer)
{
	glBindFramebuffer(GL_READ_FRAMEBUFFER, rBuffer->fbo);

	for(unsigned int i = 0; i < RayBuffer_TextureType_DEPTH; i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, rBuffer->textures[i]);
	}
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
	//case RayBuffer_TextureType_FINAL:
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
	//case RayBuffer_TextureType_FINAL:
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
	//case RayBuffer_TextureType_FINAL:
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

