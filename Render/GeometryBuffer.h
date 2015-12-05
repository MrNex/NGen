#ifndef GEOMETRYBUFFER_H
#define GEOMETRYBUFFER_H

#include <GL/glew.h>
#include <GL/freeglut.h>

///
//Defines the types of textures a geometry buffer can contain
enum GeometryBuffer_TextureType
{
	GeometryBuffer_TextureType_POSITION,		//RGB32F Internal Format
	GeometryBuffer_TextureType_DIFFUSE,		//RGBA8 Internal Format
	GeometryBuffer_TextureType_NORMAL,		//RGB32F InternalFormat
	GeometryBuffer_TextureType_TEXTURECOORDINATE,	//RG32F Internal Format
	GeometryBuffer_TextureType_DEPTH,		//DEPTH32F Internal format
	GeometryBuffer_TextureType_NUMTEXTURES		//The number of texture types
};

typedef struct GeometryBuffer
{
	GLuint fbo;						//Frame buffer object
	GLuint textures[GeometryBuffer_TextureType_NUMTEXTURES];
} GeometryBuffer;

///
//Allocates memory for a GeometryBuffer
//
//Returns:
//	A pointer to an uninitialized GeometryBuffer
GeometryBuffer* GeometryBuffer_Allocate(void);

///
//Initializes a GeometryBuffer
//
//Parameters:
//	gbuffer: Pointer to the GeometryBuffer being initialized
//	textureWidth: The width of the GeometryBuffer's textures
//	textureHeight: The height of the GeometryBuffer's textures
void GeometryBuffer_Initialize(GeometryBuffer* gBuffer, unsigned int textureWidth, unsigned int textureHeight);

///
//Frees memory allocated by a geometry buffer
//
//Parameters:
//	gBuffer: A pointer to the geometry buffer being freed
void GeometryBuffer_Free(GeometryBuffer* gBuffer);

#endif
