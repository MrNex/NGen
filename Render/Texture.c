#include "Texture.h"
#include "Image.h"

#include <stdio.h>
#include <string.h>

#include <CL/cl.h>
#include <CL/cl_gl.h>

///
//Allocates memory for a texture
//
//Returns:
//	A pointer to a newly allocated texture in memory
Texture* Texture_Allocate(void)
{
	Texture* t = (Texture *)malloc(sizeof(Texture));
	return t;
}

///
//Initializes a texture
//
//Parameters:
//	t: The texture to initialize
//	i: The image being made into a texture
void Texture_Initialize(Texture* t, struct Image* i)
{
	t->image = i;
	glGenTextures(1, &t->textureID);
	glBindTexture(GL_TEXTURE_2D, t->textureID);

	glTexImage2D(
		GL_TEXTURE_2D,		//What kind of texture
		0,			//Mipmaping level (Increases detail)
		GL_RGBA8,		//Internal format of image data (RGBA each taking 8 bits)
		t->image->width,	//Width of image
		t->image->height,	//Height of image
		0,			//Border
		GL_RGBA,		//Format of data to output into texture
		GL_UNSIGNED_BYTE,	//What isthe type of each element in array
		t->image->bitmap	//Image data 
		);

	//Perform a linear interpolation of texture coordinates when scaling the texture both up and down
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//CLamp texture to the edge when it wraps around side or top
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Tile texture instead of clamping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	//Check for error codes
	int errorCode = 0;
	do
	{
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			printf("Shit: %d\n", errorCode);

		}
	} while (errorCode != GL_NO_ERROR);

}

///
//Initializes a texture as a single GLuint, rather than a full texture struct
//
//Parameters:
//	image: A pointer to the image data to upload to this texture unit
GLuint Texture_InitializeGLuint(struct Image* image)
{
	GLuint texture; 
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexImage2D(
		GL_TEXTURE_2D,		//What kind of texture
		0,			//Mipmaping level (Increases detail)
		GL_RGBA8,		//Internal format of image data (RGBA each taking 8 bits)
		image->width,		//Width of image
		image->height,		//Height of image
		0,			//Border
		GL_RGBA,		//Format of data to output into texture
		GL_UNSIGNED_BYTE,	//What isthe type of each element in array
		image->bitmap		//Image data 
		);

	//Perform a linear interpolation of texture coordinates when scaling the texture both up and down
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//CLamp texture to the edge when it wraps around side or top
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	//Tile texture instead of clamping
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


	//Check for error codes
	int errorCode = 0;
	do
	{
		errorCode = glGetError();
		if (errorCode != GL_NO_ERROR)
		{
			printf("Shit: %d\n", errorCode);

		}
	} while (errorCode != GL_NO_ERROR);

	return texture;
}

///
//Creates an openCL reference to an openGL texture
//
//Parameters:
//	glRef: A OpenGL texture unit ID to create an openCL reference to
//	buf: A pointer to the kernel buffer containing the cl context and device to create this reference on
//
//returns:
//	cl memory reference to specified openGL texture
cl_mem Texture_InitializeCLReference(GLuint glRef, KernelBuffer* buf)
{
	cl_int err = 0;
	cl_mem clRef = 0;
	clRef = clCreateFromGLTexture
	(
		buf->clContext,
		CL_MEM_READ_WRITE,
		GL_TEXTURE_2D,
		0,
		glRef,
		&err
	);

	const size_t len = strlen("Texture_InitializeCLReference :: clCreateFromGLTexture :: ###");

	char finalString[len];
	sprintf(finalString, "Texture_InitializeCLReference :: clCreateFromGLTexture :: %d", glRef);

	KernelManager_CheckCLErrors(err, finalString);
	
	return clRef;
}

///
//Frees a texture
//
//PArameters:
//	t: Pointer to texture being deleted
void Texture_Free(Texture* t)
{
	glDeleteTextures(1, &(t->textureID));
	Image_Free(t->image);
	free(t);
}

///
//Frees a texture in the form of a texture unit ID
//
//Parameters:
//	textureID: the ID of the texture unit to free
void Texture_FreeGLuint(GLuint textureID)
{
	glDeleteTextures(1, &textureID);
}

///
//Frees an OpenCL reference to an OpenGL texture
//
//Parameters:
//	the openCL memory reference to free
void Texture_FreeCLReference(cl_mem clRef)
{
	cl_int err = 0;
	err = clReleaseMemObject(clRef);
	KernelManager_CheckCLErrors(err, "Texture_FreeCLReference :: clReleaseMemObject");
}
