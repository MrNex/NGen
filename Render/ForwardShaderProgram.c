#include "ForwardShaderProgram.h"

#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"
#include "../Compatibility/ProgramUniform.h"

typedef struct ForwardShaderProgram_UniformSet
{
	GLint modelMatrixLocation;
	GLint viewMatrixLocation;
	GLint projectionMatrixLocation;
	GLint modelViewProjectionMatrixLocation;

	GLint directionalLightVectorLocation;

	GLint colorMatrixLocation;
	GLint textureLocation;
	GLint tileLocation;
} ForwardShaderProgram_UniformSet;

///
//Static function declarataions

///
//Allocates a set of uniform variables needed by this shader program
//
//Returns:
//	A (void) pointer to a struct containing the set of uniforms this shader program needs
static UniformSet ForwardShaderProgram_AllocateUniformSet(void);

///
//Initializes the set of uniforms needed by a ForwardShaderProgram
//
//Parameters:
//	prog: A to the uniform set to initialize
static void ForwardShaderProgram_InitializeUniformSet(ShaderProgram* prog);

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

	prog->uniforms = ForwardShaderProgram_AllocateUniformSet();
	ForwardShaderProgram_InitializeUniformSet(prog);
	prog->Render = ForwardShaderProgram_Render;
}

///
//Allocates a set of uniform variables needed by this shader program
//
//Parameters:
//	prog: A pointer to the shader program to allocate the uniform set for
//
//Returns:
//	A (void) pointer to a struct containing the set of uniforms this shader program needs
static UniformSet ForwardShaderProgram_AllocateUniformSet(void)
{
	ForwardShaderProgram_UniformSet* uniforms = malloc(sizeof(ForwardShaderProgram_UniformSet));
	return uniforms;
}

///
//Initializes the set of uniforms needed by a ForwardShaderProgram
//
//Parameters:
//	prog: A pointer to the program with the uniform set to initialize
static void ForwardShaderProgram_InitializeUniformSet(ShaderProgram* prog)
{
	ForwardShaderProgram_UniformSet* uniforms = prog->uniforms;
	uniforms->modelMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelMatrix");
	uniforms->viewMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "viewMatrix");
	uniforms->projectionMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "projectionMatrix");
	uniforms->modelViewProjectionMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelViewProjectionMatrix");


	uniforms->directionalLightVectorLocation = glGetUniformLocation(prog->shaderProgramID, "directionalLightVector");

	uniforms->colorMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "colorMatrix" );
	uniforms->textureLocation = glGetUniformLocation(prog->shaderProgramID, "textureDiffuse");
	uniforms->tileLocation = glGetUniformLocation(prog->shaderProgramID, "tileVector");
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
	ForwardShaderProgram_UniformSet* uniforms = prog->uniforms;
	
	//Directional light
	ProgramUniform3fv
	(
		prog->shaderProgramID, 
		uniforms->directionalLightVectorLocation, 
		1,
		buffer->directionalLightVector->components
	);

	//Camera view matrix
	ProgramUniformMatrix4fv
	(
		prog->shaderProgramID,
		uniforms->viewMatrixLocation,
		1,
		GL_TRUE,
		buffer->camera->viewMatrix->components
	);

	//Camera Projection matrix
	ProgramUniformMatrix4fv
	(
	 	prog->shaderProgramID,
		uniforms->projectionMatrixLocation,
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
  
	ForwardShaderProgram_UniformSet* uniforms = prog->uniforms;

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
		uniforms->modelMatrixLocation,
		1,
		GL_TRUE,
		model.components
	);

	//Model view projection matrix
	ProgramUniformMatrix4fv
	(
		prog->shaderProgramID,
		uniforms->modelViewProjectionMatrixLocation,
		1,
		GL_TRUE,
		modelViewProjection.components
	);

	if(gameObj->material != NULL)
	{
		Material* material = gameObj->material;

		//Color matrix
		ProgramUniformMatrix4fv
		(
			prog->shaderProgramID,
			uniforms->colorMatrixLocation,
			1,
			GL_TRUE,
			material->colorMatrix->components
		);

		//Tile vector
		ProgramUniform2fv
		(
			prog->shaderProgramID,
			uniforms->tileLocation,
			1,
			material->tile->components
		);

		//Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material->texture->textureID);
		ProgramUniform1i(prog->shaderProgramID, uniforms->textureLocation, 0);
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
			uniforms->colorMatrixLocation,
			1,
			GL_TRUE,
			defaultColorMatrix
		);

		//Tile vector
		ProgramUniform2fv
		(
			prog->shaderProgramID,
			uniforms->tileLocation,
			1,
			defaultTile
		);

		//Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, AssetManager_LookupTexture("Test")->textureID);
		ProgramUniform1i(prog->shaderProgramID, uniforms->textureLocation, 0);


	}
	
}

///
//Renders a list of gameObjects using a ForwardShaderProgram
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
