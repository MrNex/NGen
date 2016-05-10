#include "ForwardShaderProgram.h"

#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"
#include "../Compatibility/ProgramUniform.h"

typedef struct ForwardShaderProgram_Members
{
	GLint modelMatrixLocation;
	GLint viewMatrixLocation;
	GLint projectionMatrixLocation;
	GLint modelViewProjectionMatrixLocation;

	GLint directionalLightVectorLocation;

	GLint colorMatrixLocation;
	GLint textureLocation;
	GLint tileLocation;
} ForwardShaderProgram_Members;

///
//Static function declarataions

///
//Allocates a set of uniform variables and members needed by this shader program
//
//Returns:
//	A (void) pointer to a struct containing the set of uniforms/members this shader program needs
static ShaderProgram_Members ForwardShaderProgram_AllocateMembers(void);

///
//Initializes the set of uniforms/members needed by a ForwardShaderProgram
//
//Parameters:
//	prog: A shader program to initialize the uniforms/members of
static void ForwardShaderProgram_InitializeMembers(ShaderProgram* prog);

///
//Frees the set of uniforms/members used by this shader program
//
//Parameters:
//	prog: A pointer to the shader program to free the uniforms/members of
static void ForwardShaderProgram_FreeMembers(ShaderProgram* prog);

///
//Sets the uniform varaibles needed by this shader program 
//This function only sets the uniforms which remain constant between meshes
//For the setting of uniforms which do not have this property see
//ForwardShaderProgram_SetVariableUniforms(...)
//
//Parameters:
//	prog: A pointer to The ForwardShaderProgram to set the uniform variables of
//	buffer: A pointer to the active Rendering Buffer containing the information needed for the uniforms
static void ForwardShaderProgram_SetConstantUniforms(ShaderProgram* prog, RenderingBuffer* buffer);

///
//Sets the uniform variables needed by this shader program
//This function only sets the uniforms which vary between meshes
//For the setting of uniforms which are constant between meshes see
//ForwardShaderProgram_SetConstantUniforms(...)
//
//Parameters:
//	prog: A pointer to the ForwardShaderProgram to set the uniform variables of
//	cam: A pointer to the active Camera containing information needed for the uniforms
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void ForwardShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj);

///
//Renders a list of gameObjects using a ForwardShaderProgram
//
//Parameters:
//	prog: A pointer to The shader program to use to render
//	buffer: A pointer to The active rendering buffer
//	gameObjects: A pointer to the linked list of gameObjects to render
static void ForwardShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, LinkedList* gameObjects);


///
//Initializes the forward rendering shader program
//
//Parameters:
//	prog: A pointer to the shader program to initialize as a forward rendering shader program.
void ForwardShaderProgram_Initialize(ShaderProgram* prog)
{
	ShaderProgram_Initialize(prog, "Shader/ForwardVertexShader.glsl", "Shader/ForwardFragmentShader.glsl");

	prog->members = ForwardShaderProgram_AllocateMembers();
	ForwardShaderProgram_InitializeMembers(prog);
	prog->Render = (ShaderProgram_RenderFunc)ForwardShaderProgram_Render;
	prog->FreeMembers = ForwardShaderProgram_FreeMembers;
}

///
//Allocates a set of uniform variables needed by this shader program
//
//Parameters:
//	prog: A pointer to the shader program to allocate the uniform set for
//
//Returns:
//	A (void) pointer to a struct containing the set of uniforms this shader program needs
static ShaderProgram_Members ForwardShaderProgram_AllocateMembers(void)
{
	ForwardShaderProgram_Members* uniforms = malloc(sizeof(ForwardShaderProgram_Members));
	return uniforms;
}

///
//Initializes the set of uniforms needed by a ForwardShaderProgram
//
//Parameters:
//	prog: A pointer to the program with the uniform set to initialize
static void ForwardShaderProgram_InitializeMembers(ShaderProgram* prog)
{
	ForwardShaderProgram_Members* members = prog->members;
	members->modelMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelMatrix");
	//TODO: I don't believe the view matrix is needed in this shader program, remove.
	members->viewMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "viewMatrix");
	members->projectionMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "projectionMatrix");
	members->modelViewProjectionMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelViewProjectionMatrix");


	members->directionalLightVectorLocation = glGetUniformLocation(prog->shaderProgramID, "directionalLightVector");

	members->colorMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "colorMatrix" );
	members->textureLocation = glGetUniformLocation(prog->shaderProgramID, "textureDiffuse");
	members->tileLocation = glGetUniformLocation(prog->shaderProgramID, "tileVector");
}

///
//Frees the set of uniforms/members used by this shader program
//
//Parameters:
//	prog: A pointer to the shader program to free the uniforms/members of
void ForwardShaderProgram_FreeMembers(ShaderProgram* prog)
{
	//No special instructions for this shader
	free(prog->members);
}

///
//Sets the uniform varaibles needed by this shader program 
//This function only sets the uniforms which remain constant between meshes
//For the setting of uniforms which do not have this property see
//ForwardShaderProgram_SetVariableUniforms(...)
//
//Parameters:
//	prog: A pointer to The ForwardShaderProgram to set the uniform variables of
//	buffer: A pointer to the active Rendering Buffer containing the information needed for the uniforms
static void ForwardShaderProgram_SetConstantUniforms(ShaderProgram* prog, struct RenderingBuffer* buffer)
{
	ForwardShaderProgram_Members* members = prog->members;
	
	//Directional light
	ProgramUniform3fv
	(
		prog->shaderProgramID, 
		members->directionalLightVectorLocation, 
		1,
		buffer->directionalLightVector->components
	);

	//Camera view matrix
	ProgramUniformMatrix4fv
	(
		prog->shaderProgramID,
		members->viewMatrixLocation,
		1,
		GL_TRUE,
		buffer->camera->viewMatrix->components
	);

	//Camera Projection matrix
	ProgramUniformMatrix4fv
	(
	 	prog->shaderProgramID,
		members->projectionMatrixLocation,
		1,
		GL_TRUE,
		buffer->camera->projectionMatrix->components
	);
	
}



///
//Sets the uniform variables needed by this shader program
//This function only sets the uniforms which vary between meshes
//For the setting of uniforms which are constant between meshes see
//ForwardShaderProgram_SetConstantUniforms(...)
//
//Parameters:
//	prog: A pointer to the ForwardShaderProgram to set the uniform variables of
//	cam: A pointer to the active Camera containing information needed for the uniforms
//	gameObj: A pointer to the mesh containing the information needed for the uniforms
static void ForwardShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj)
{
  
	ForwardShaderProgram_Members* members = prog->members;

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
		ProgramUniform1i(prog->shaderProgramID, members->textureLocation, 0);
	//}
	/*
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
//Renders a list of gameObjects using a ForwardShaderProgram
//Note: Sets the active shader program to this
//
//Parameters:
//	prog: A pointer to The shader program to use to render
//	buffer: A pointer to The active rendering buffer
//	gameObjects: A pointer to the linked list of gameObjects to render
static void ForwardShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, LinkedList* gameObjects)
{
	glUseProgram(prog->shaderProgramID);

	ForwardShaderProgram_SetConstantUniforms(prog, buffer);

	struct LinkedList_Node* current = gameObjects->head;
	GObject* gameObj = NULL;
	while(current != NULL)
	{
		gameObj = (GObject*)current->data;
		if(gameObj->mesh != NULL)
		{
			ForwardShaderProgram_SetVariableUniforms(prog,  buffer->camera, gameObj);
			Mesh_Render(gameObj->mesh, gameObj->mesh->primitive);
		}
		current = current->next;
	}

	glFlush();
}
