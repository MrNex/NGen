#include "DeferredGeometryShaderProgram.h"

#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"
#include "../Compatibility/ProgramUniform.h"

#include "GeometryBuffer.h"

typedef struct DeferredGeometryShaderProgram_Members
{
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
	
	prog->members = DeferredGeometryShaderProgram_AllocateMembers();
	DeferredGeometryShaderProgram_InitializeMembers(prog);

	prog->FreeMembers = DeferredGeometryShaderProgram_FreeMembers;
	prog->Render = (ShaderProgram_RenderFunc)DeferredGeometryShaderProgram_Render;	
	
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
static void DeferredGeometryShaderProgram_FreeMembers(ShaderProgram* prog)
{
	free(prog->members);
}

///
//Sets the uniform variables needed by this shader program
//This function only sets the uniforms which remain constant between meshes.
//
//Parameters: 
//	prog: A pointer to the deferred geometry shader program to set the constant uniforms of
//	buffer: A pointer to the rendering buffer to get the uniform values from
static void DeferredGeometryShaderProgram_SetConstantUniforms(ShaderProgram* prog, RenderingBuffer* buffer)
{
	//None
	(void)prog;
	(void)buffer;
}

///
//Sets the uniform variables needed by this shader program
//This function only sets the uniforms which vary between meshes
//
//Parameters:
//	prog: A pointer to the deferred geometry shader program to set the variable uniforms of
//	cam: A pointer to the active camera containing information needed to calculate the uniforms
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void DeferredGeometryShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj)
{
	DeferredGeometryShaderProgram_Members* members = prog->members;
	
	Matrix model;
	Matrix modelViewProjection;
	
	Matrix_INIT_ON_STACK(model, 4, 4);
	Matrix_INIT_ON_STACK(modelViewProjection, 4, 4);

	FrameOfReference_ToMatrix4(gameObj->frameOfReference, &model);
	Matrix_Copy(&modelViewProjection, &model);
	Matrix_TransformMatrix(cam->viewMatrix, &modelViewProjection);
	Matrix_TransformMatrix(cam->projectionMatrix, &modelViewProjection);

	//Model Matrix
	ProgramUniformMatrix4fv
	(
		prog->shaderProgramID,
		members->modelMatrixLocation,
		1,
		GL_TRUE,
		model.components
	);

	//Model view projection matrix
	ProgramUniformMatrix4fv
	(
		prog->shaderProgramID,
		members->modelViewProjectionMatrixLocation,
		1,
		GL_TRUE,
		modelViewProjection.components
	);

	//if(gameObj->material != NULL)
	//{
		Material* material = MemoryPool_RequestAddress(assetBuffer->materialPool, gameObj->materialID);

		//Color matrix
		ProgramUniformMatrix4fv
		(
			prog->shaderProgramID,
			members->colorMatrixLocation,
			1,
			GL_TRUE,
			material->colorMatrix
		);

		//Tile vector
		ProgramUniform2fv
		(
			prog->shaderProgramID,
			members->tileLocation,
			1,
			material->tile
		);

		//Texture
		glActiveTexture(GL_TEXTURE0);
		GLuint textureID = AssetManager_LookupTextureByID(material->texturePoolID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		//glBindTexture(GL_TEXTURE_2D, material->texture->textureID);
		ProgramUniform1i(prog->shaderProgramID, members->textureLocation, 0);
	/*
	}
	else
	{
		static float defaultTile[2] = {1.0f, 1.0f};
		static float defaultColorMatrix[16] = 
		{
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};
		
		//Color matrix
		ProgramUniformMatrix4fv
		(
			prog->shaderProgramID,
			members->colorMatrixLocation,
			1,
			GL_TRUE,
			defaultColorMatrix
		);

		//Tile vector
		ProgramUniform2fv
		(
			prog->shaderProgramID,
			members->tileLocation,
			1,
			defaultTile
		);

		//Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, AssetManager_LookupTexture("Test"));
		ProgramUniform1i(prog->shaderProgramID, members->textureLocation, 0);


	}
	*/
}


///
//Renders a list of GObjects to the active frame buffer object using a deferred geometry shader program
//Note: Sets the active shader program to this
//
//Parameters:
//	prog: A pointer to the DeferredGeometryShaderProgram which will be rendering the objects
//	buffer: A pointer to the rendering buffer to render with
//	gameObjects: A pointer to a linked list containing GObjects to be rendered
static void DeferredGeometryShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, LinkedList* gameObjects)
{
	glUseProgram(prog->shaderProgramID);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DeferredGeometryShaderProgram_SetConstantUniforms(prog, buffer);

	struct LinkedList_Node* current = gameObjects->head;
	GObject* gameObj = NULL;
	while(current != NULL)
	{
		gameObj = (GObject*)current->data;
		if(gameObj->mesh != NULL)
		{
			DeferredGeometryShaderProgram_SetVariableUniforms(prog,  buffer->camera, gameObj);
			Mesh_Render(gameObj->mesh, gameObj->mesh->primitive);
		}
		current = current->next;
	}

}
