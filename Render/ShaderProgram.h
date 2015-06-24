#include <GL/glew.h>
#include <GL/freeglut.h>

typedef struct ShaderProgram
{
	//Base members
	GLuint shaderProgramID;
	GLuint vertexShaderID;
	GLuint fragmentShaderID;

	//Program uniforms
	GLint modelMatrixLocation;
	GLint viewMatrixLocation;
	GLint projectionMatrixLocation;
	GLint modelViewProjectionMatrixLocation;
	

	GLint colorMatrixLocation;
	GLint tileLocation;

	GLint directionalLightVectorLocation;

	GLint textureLocation;

} ShaderProgram;

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
