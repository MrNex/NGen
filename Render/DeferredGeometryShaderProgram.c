#include "DeferredGeometryShaderProgram.h"

#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"
#include "../Compatibility/ProgramUniform.h"

#include "GeometryBuffer.h"

typedef struct DeferredGeometryShaderProgram_Members
{
	GeometryBuffer* geomBuffer;

	//Uniforms
	GLint modelMatrixLocation;
	GLint modelViewProjectionMatrixLocation;

	GLint textureLocation;
	GLint colorMatrixLocation;
	GLint tileLocation;
} DeferredGeometryShaderProgram_Members;

///
//Static function declarations

///
//Allocates a set of variables needed by a DeferredGeometryShaderProgram
//
//Returns:
//	A (void) pointerto a struct containing the set of uniforms/members this shader program needs
static ShaderProgram_Members DeferredGeometryShaderProgram_AllocateMembers(void);

///
//Initializes the set of members needed by a DeferredGeometryShaderprogram
//
//Parameters:
//	prog: A pointer to the shader program to initialize the members of
static void DeferredGeometryShaderProgram_InitializeMembers(ShaderProgram* prog);

///
//Frees the set of members used by a DeferredGeometryShaderProgram
//
//Parameters:
//	prog: A pointer to the shader program to free the members of
static void DeferredGeometryShaderProgram_FreeMembers(ShaderProgram* prog);

///
//Sets the uniform variables needed by this shader program
//This function only sets the uniforms which remain constant between meshes.
//
//Parameters: 
//	prog: A pointer to the deferred geometry shader program to set the constant uniforms of
//	buffer: A pointer to the rendering buffer to get the uniform values from
static void DeferredGeometryShaderProgram_SetConstantUniforms(ShaderProgram* prog, RenderingBuffer* buffer);

///
//Sets the uniform variables needed by this shader program
//This function only sets the uniforms which vary between meshes
//
//Parameters:
//	prog: A pointer to the deferred geometry shader program to set the variable uniforms of
//	cam: A pointer to the active camera containing information needed to calculate the uniforms
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void DeferredGeometryShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj);


///
//Renders a list of GObjects to the active frame buffer object using a deferred geometry shader program
//
//Parameters:
//	prog: A pointer to the DeferredGeometryShaderProgram which will be rendering the objects
//	buffer: A pointer to the rendering buffer to render with
//	gameObjects: A pointer to a linked list containing GObjects to be rendered
static void DeferredGeometryShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, LinkedList* gameObjects);

///
//Function Definitions
///

///
//Initializes the deferred rendering shader program
//
//Paramters:
//	prog: A pointer to the shader program to initialize as a deferred rendering shader program
void DeferredGeometryShaderProgram_Initialize(ShaderProgram* prog)
{
	ShaderProgram_Initialize(prog, "Shader/DeferredGeometryVertexShader.glsl", "Shader/DeferredGeometryFragmentShader.glsl");

	
}

///
//Allocates a set of variables needed by a DeferredGeometryShaderProgram
//
//Returns:
//	A (void) pointerto a struct containing the set of uniforms/members this shader program needs
static ShaderProgram_Members DeferredGeometryShaderProgram_AllocateMembers(void)
{
	return malloc(sizeof(DeferredGeometryShaderProgram_Members));
}

///
//Initializes the set of members needed by a DeferredGeometryShaderprogram
//
//Parameters:
//	prog: A pointer to the shader program to initialize the members of
static void DeferredGeometryShaderProgram_InitializeMembers(ShaderProgram* prog)
{
	DeferredGeometryShaderProgram_Members* members = prog->members;

	glUseProgram(prog->shaderProgramID);

	members->geomBuffer = GeometryBuffer_Allocate();

	//TODO: Initialize geometry buffer


	members->modelMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelMatrix");
	members->modelViewProjectionMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelViewProjectionMatrix");

	members->textureLocation = glGetUniformLocation(prog->shaderProgramID, "textureDiffuse");
	members->colorMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "colorMatrix");
	members->tileLocation = glGetUniformLocation(prog->shaderProgramID, "tileVector");

	glUseProgram(0);
}

///
//Frees the set of members used by a DeferredGeometryShaderProgram
//
//Parameters:
//	prog: A pointer to the shader program to free the members of
static void DeferredGeometryShaderProgram_FreeMembers(ShaderProgram* prog);

///
//Sets the uniform variables needed by this shader program
//This function only sets the uniforms which remain constant between meshes.
//
//Parameters: 
//	prog: A pointer to the deferred geometry shader program to set the constant uniforms of
//	buffer: A pointer to the rendering buffer to get the uniform values from
static void DeferredGeometryShaderProgram_SetConstantUniforms(ShaderProgram* prog, RenderingBuffer* buffer);

///
//Sets the uniform variables needed by this shader program
//This function only sets the uniforms which vary between meshes
//
//Parameters:
//	prog: A pointer to the deferred geometry shader program to set the variable uniforms of
//	cam: A pointer to the active camera containing information needed to calculate the uniforms
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void DeferredGeometryShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj);


///
//Renders a list of GObjects to the active frame buffer object using a deferred geometry shader program
//
//Parameters:
//	prog: A pointer to the DeferredGeometryShaderProgram which will be rendering the objects
//	buffer: A pointer to the rendering buffer to render with
//	gameObjects: A pointer to a linked list containing GObjects to be rendered
static void DeferredGeometryShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, LinkedList* gameObjects);
