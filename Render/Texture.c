#include "Texture.h"
#include "Image.h"

#include <stdio.h>

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
		0,					//Mipmaping level (Increases detail)
		GL_RGBA8,			//Internal format of image data (RGBA each taking 8 bits)
		t->image->width,	//Width of image
		t->image->height,	//Height of image
		0,					//Border
		GL_RGBA,			//Format of data to output into texture
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
