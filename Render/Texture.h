#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "../Manager/KernelManager.h"

typedef struct Texture
{
	GLuint textureID;
	struct Image* image;
} Texture;

///
//Allocates memory for a texture
//
//Returns:
//	A pointer to a newly allocated texture in memory
Texture* Texture_Allocate(void);

///
//Initializes a texture
//
//Parameters:
//	t: The texture to initialize
//	i: The image being made into a texture
void Texture_Initialize(Texture* t, struct Image* i);

///
//Initializes a texture as a single GLuint, rather than a full texture struct
//
//Parameters:
//	image: A pointer to the image data to upload to this texture unit
GLuint Texture_InitializeGLuint(struct Image* image);

///
//Creates an openCL reference to an openGL texture
//
//Parameters:
//	glRef: A OpenGL texture unit ID to create an openCL reference to
//	buf: A pointer to the kernel buffer containing the cl context and device to create this reference on
//
//returns:
//	cl memory reference to specified openGL texture
cl_mem Texture_InitializeCLReference(GLuint glRef, KernelBuffer* buf);

///
//Frees a texture
//
//PArameters:
//	t: Pointer to texture being deleted
void Texture_Free(Texture* t);

///
//Frees a texture in the form of a texture unit ID
//
//Parameters:
//	textureID: the ID of the texture unit to free
void Texture_FreeGLuint(GLuint textureID);

///
//Frees an OpenCL reference to an OpenGL texture
//
//Parameters:
//	the openCL memory reference to free
void Texture_FreeCLReference(cl_mem clRef);

#endif
