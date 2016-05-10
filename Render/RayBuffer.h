#ifndef RAYBUFFER_H
#define RAYBUFFER_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <CL/cl.h>
#include <CL/cl_gl.h>

///
//Defines the types of textures a ray buffer can contain
enum RayBuffer_TextureType
{
	RayBuffer_TextureType_POSITION,		//RGB32F Internal Format
	RayBuffer_TextureType_DIFFUSE,		//RGBA8 Internal Format
	RayBuffer_TextureType_NORMAL,		//RGB32F Internal Format
	RayBuffer_TextureType_LOCALMATERIAL,	//RGBA32F Internal Format
	RayBuffer_TextureType_SPECULAR,		//RGBA8
	RayBuffer_TextureType_SHADOW,		//R8 Internal Format
	RayBuffer_TextureType_GLOBALMATERIAL,	//RGBA32F Internal format
	RayBuffer_TextureType_DEPTH,		//DEPTH32F_STENCIL8 Internal format
	RayBuffer_TextureType_NUMTEXTURES	//The number of texture types
};



typedef struct RayBuffer
{
	GLuint fbo;						//Frame buffer object
	GLuint textures[RayBuffer_TextureType_NUMTEXTURES];
	cl_mem textureRefs[RayBuffer_TextureType_DEPTH];
	int textureWidth, textureHeight;

	unsigned int passType[3];	//Determines if GlobalMaterial information gets written in the LocalPass

} RayBuffer;

///
//Allocates memory for a RayBuffer
//
//Returns:
//	A pointer to an uninitialized RayBuffer
RayBuffer* RayBuffer_Allocate(void);

///
//Initializes a RayBuffer
//
//Parameters:
//	gbuffer: Pointer to the RayBuffer being initialized
//	textureWidth: The width of the RayBuffer's textures
//	textureHeight: The height of the RayBuffer's textures
void RayBuffer_Initialize(RayBuffer* rBuffer, unsigned int textureWidth, unsigned int textureHeight);

///
//Frees memory allocated by a ray buffer
//
//Parameters:
//	rBuffer: A pointer to the ray buffer being freed
void RayBuffer_Free(RayBuffer* rBuffer);

///
//Converts a texture type enum value to a string
//
//Parameters:
//	type: The type to conver tot a string
//
//Returns:
//	A pointer to a null terminated constant character array containing the string
const char* RayBuffer_TextureTypeToString(enum RayBuffer_TextureType type);

///
//Clears the final texture of the ray buffer
//
//Parameters:
//	rBuffer: A pointer to the ray buffer to clear the final texture of
void RayBuffer_ClearTexture(RayBuffer* rBuffer, enum RayBuffer_TextureType type);

///
//Binds the Frame Buffer Object of the Ray Buffer to bind it's textures to be written to from the PixelProjectionGeometryShaderProgram.
//
//Parameters:
//	rBuffer: A pointer to the ray buffer being bound for writing
void RayBuffer_BindForGeometryPass(RayBuffer* rBuffer);

///
//Binds the ray buffer to be written to by the ray tracer shadow kernel program
//
//Parameters:
//	rBuffer: A pointer to the ray buffer to bind for kernel use
//	event: A pointer to a cl_event which will be filled with an event tracking the progress of buffer binding
void RayBuffer_BindForKernels(RayBuffer* rBuffer, cl_event* event);

///
//Releases the ray buffer textures from openCL kernel use
//
//Parameters:
//	rBuffer: A pointer to the ray buffer to release from kernel use
//	event: A pointer to an event which will be tracking the progress of buffer release
void RayBuffer_ReleaseFromKernels(RayBuffer* rBuffer, cl_event* event);

///
//Binds the ray buffer to have it's stencil texture written to from the DeferredStencilShaderProgram
//
//Parameters:
//	rBuffer: A pointer to the ray buffer being bound for the stencil pass
void RayBuffer_BindForStencilPass(RayBuffer* rBuffer);

///
//Binds the Frame Buffer Object of the ray buffer to bind it's textures to be read from the Deferred lighting shaders
// 
//Parameters:
//	rBuffer: A pointer to the ray buffer being bound for reading
void RayBuffer_BindForLightPass(RayBuffer* rBuffer);

///
//Binds the ray buffer for the local illumination pass
//
//Parameters:
//	rBuffer: A ointer to the ray buffer to bind
void RayBuffer_BindForLocalPass(RayBuffer* rBuffer);

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
void* RayBuffer_AllocateTextureData(RayBuffer* buffer, enum RayBuffer_TextureType type);

///
//Reads the data from a texture storing it in the specified destination.
//NOTE: Texture must be bound first
//
//Parameters:
//	dest: a pointer to the contiguous memory in which to store the texture data
//	type: The type of the texture to get 
void RayBuffer_GetTextureData(void* dest, enum RayBuffer_TextureType type);

///
//Writes data to the bound texture
//NOTE: Texture specified by type must be bound before calling this function
//
//Parameters:
//	buffer: A pointer to the ray buffer which contains the texture being written to
//	type: The type of texture being written to
//	data: A pointer to the data to write
void RayBuffer_WriteTextureData(RayBuffer* buffer, enum RayBuffer_TextureType type, void* data);

#endif
