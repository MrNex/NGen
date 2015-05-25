#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>
#include <GL/freeglut.h>

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
//Frees a texture
//
//PArameters:
//	t: Pointer to texture being deleted
void Texture_Free(Texture* t);

#endif
