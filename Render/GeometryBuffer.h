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
	GeometryBuffer_TextureType_DEPTH,		//DEPTH32F_STENCIL8 Internal format
	GeometryBuffer_TextureType_FINAL,		//RGBA internal format
	GeometryBuffer_TextureType_NUMTEXTURES		//The number of texture types
};

typedef struct GeometryBuffer
{
	GLuint fbo;						//Frame buffer object
	GLuint textures[GeometryBuffer_TextureType_NUMTEXTURES];
	int textureWidth, textureHeight;
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

///
//Binds the Frame Buffer Object of the Geometry Buffer to bind it's textures to be written to from the DeferredGeometryShaderProgram.
//
//Parameters:
//	gBuffer: A pointer to the geometry buffer being bound for writing
void GeometryBuffer_BindForGeometryPass(GeometryBuffer* gBuffer);

///
//Binds the geometry buffer to have it's stencil texture written to from the DeferredStencilShaderProgram
//
//Parameters:
//	gBuffer: A pointer to the geometry buffer being bound for the stencil pass
void GeometryBuffer_BindForStencilPass(GeometryBuffer* gBuffer);

///
//Binds the Frame Buffer Object of the geometry buffer to bind it's textures to be read from the Deferred lighting shaders
// 
//Parameters:
//	gBuffer: A pointer to the geometry buffer being bound for reading
void GeometryBuffer_BindForLightPass(GeometryBuffer* gBuffer);

///
//Binds the FrameBufferObject of the geometry buffer to be read for the final pass
//
//Parameters:
//	gBuffer: A pointer to the geometry buffer being bound for the final pass
void GeometryBuffer_BindForFinalPass(GeometryBuffer* gBuffer);

#endif
