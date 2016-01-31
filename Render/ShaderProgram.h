#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "../Data/LinkedList.h"

///
//Forward declaration of RenderingBuffer struct
struct RenderingBuffer;

///
//Defines the set of uniforms/members needed by this shader program
typedef void* ShaderProgram_Members;

typedef struct ShaderProgram
{
	//Base members
	GLuint shaderProgramID;

	//Program uniforms
	ShaderProgram_Members members;
	
	//Function to begin rendering
	void (*Render)(struct ShaderProgram* prog, struct RenderingBuffer* buffer, void* collectionToRender);

	//Function to free shader members
	void (*FreeMembers)(struct ShaderProgram* prog);
	
} ShaderProgram;

typedef void (*ShaderProgram_RenderFunc)(struct ShaderProgram* prog, struct RenderingBuffer* buffer, void* collectionToRender);

///
//Allocates a new shader program returning a pointer to it in memory
//
//Returns:
//	A pointer to a newly allocated shader program
ShaderProgram* ShaderProgram_Allocate();

///
//Initializes a shader program
//
//PArameters:
//	prog: The shader program to initialize
//	vPath: filepath to vertex shader .glsl file
//	fPath: filepath to fragment shader .glsl file
void ShaderProgram_Initialize(ShaderProgram* prog, const char* vPath, const char* fPath);

///
//Frees the memory being taken up by a shader program
//
//Parameters:
//	prog: The shader program to free
void ShaderProgram_Free(ShaderProgram* prog);
#endif
