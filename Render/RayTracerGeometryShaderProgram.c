#include "RayTracerGeometryShaderProgram.h"

#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"
#include "../Compatibility/ProgramUniform.h"

#include "GeometryBuffer.h"

typedef struct RayTracerGeometryShaderProgram_Members
{
	//Uniforms
	GLint modelMatrixLocation;
	GLint modelViewProjectionMatrixLocation;

	GLint textureLocation;
	GLint colorMatrixLocation;
	GLint tileLocation;

	GLint localMaterialLocation;
	GLint globalMaterialLocation;
	GLint specularColorLocation;
} RayTracerGeometryShaderProgram_Members;

///
//Static function declarations

///
//Allocates a set of variables needed by a RayTracerGeometryShaderProgram
//
//Returns:
//	A (void) pointerto a struct containing the set of uniforms/members this shader program needs
static ShaderProgram_Members RayTracerGeometryShaderProgram_AllocateMembers(void);

///
//Initializes the set of members needed by a RayTracerGeometryShaderprogram
//
//Parameters:
//	prog: A pointer to the shader program to initialize the members of
static void RayTracerGeometryShaderProgram_InitializeMembers(ShaderProgram* prog);

///
//Frees the set of members used by a RayTracerGeometryShaderProgram
//
//Parameters:
//	prog: A pointer to the shader program to free the members of
static void RayTracerGeometryShaderProgram_FreeMembers(ShaderProgram* prog);

///
//Sets the uniform variables needed by this shader program
//This function only sets the uniforms which remain constant between meshes.
//
//Parameters: 
//	prog: A pointer to the raytracer geometry shader program to set the constant uniforms of
//	buffer: A pointer to the rendering buffer to get the uniform values from
static void RayTracerGeometryShaderProgram_SetConstantUniforms(ShaderProgram* prog, RenderingBuffer* buffer);

///
//Sets the uniform variables needed by this shader program
//This function only sets the uniforms which vary between meshes
//
//Parameters:
//	prog: A pointer to the raytracer geometry shader program to set the variable uniforms of
//	cam: A pointer to the active camera containing information needed to calculate the uniforms
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void RayTracerGeometryShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj, unsigned int objID);


///
//Renders a list of GObjects to the active frame buffer object using a raytracer geometry shader program
//
//Parameters:
//	prog: A pointer to the RayTracerGeometryShaderProgram which will be rendering the objects
//	buffer: A pointer to the rendering buffer to render with
//	gameObjects: A pointer to a linked list containing GObjects to be rendered
static void RayTracerGeometryShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, LinkedList* gameObjects);

///
//Renders a memory pool of GObjects to the active frame buffer object using a raytracer geometry shader program
//
//Parameters:
//	prog: A pointer to the RayTracerGeometryShaderProgram which will be rendering the objects
//	buffer: A pointer to the rendering buffer to render with
//	pool: A pointer to the memory pool containing the GObjects to be rendered
static void RayTracerGeometryShaderProgram_RenderWithMemoryPool(ShaderProgram* prog, RenderingBuffer* buffer, MemoryPool* pool);

///
//Function Definitions
///

///
//Initializes the raytracer rendering shader program
//
//Paramters:
//	prog: A pointer to the shader program to initialize as a raytracer rendering shader program
void RayTracerGeometryShaderProgram_Initialize(ShaderProgram* prog)
{
	ShaderProgram_Initialize(prog, "Shader/RayTracerGeometryVertexShader.glsl", "Shader/RayTracerGeometryFragmentShader.glsl");
	
	prog->members = RayTracerGeometryShaderProgram_AllocateMembers();
	RayTracerGeometryShaderProgram_InitializeMembers(prog);

	prog->FreeMembers = RayTracerGeometryShaderProgram_FreeMembers;
	//prog->Render = (ShaderProgram_RenderFunc)RayTracerGeometryShaderProgram_Render;	
	prog->Render = (ShaderProgram_RenderFunc)RayTracerGeometryShaderProgram_RenderWithMemoryPool;
	
}

///
//Allocates a set of variables needed by a RayTracerGeometryShaderProgram
//
//Returns:
//	A (void) pointerto a struct containing the set of uniforms/members this shader program needs
static ShaderProgram_Members RayTracerGeometryShaderProgram_AllocateMembers(void)
{
	return malloc(sizeof(RayTracerGeometryShaderProgram_Members));
}

///
//Initializes the set of members needed by a RayTracerGeometryShaderprogram
//
//Parameters:
//	prog: A pointer to the shader program to initialize the members of
static void RayTracerGeometryShaderProgram_InitializeMembers(ShaderProgram* prog)
{
	RayTracerGeometryShaderProgram_Members* members = prog->members;

	glUseProgram(prog->shaderProgramID);

	members->modelMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelMatrix");
	members->modelViewProjectionMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelViewProjectionMatrix");

	members->textureLocation = glGetUniformLocation(prog->shaderProgramID, "textureDiffuse");
	members->colorMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "colorMatrix");
	members->tileLocation = glGetUniformLocation(prog->shaderProgramID, "tileVector");

	members->localMaterialLocation = glGetUniformLocation(prog->shaderProgramID, "localMaterialVector");
	members->globalMaterialLocation = glGetUniformLocation(prog->shaderProgramID, "globalMaterialVector");
	members->specularColorLocation = glGetUniformLocation(prog->shaderProgramID, "specularColorVector");

	glUseProgram(0);
}

///
//Frees the set of members used by a RayTracerGeometryShaderProgram
//
//Parameters:
//	prog: A pointer to the shader program to free the members of
static void RayTracerGeometryShaderProgram_FreeMembers(ShaderProgram* prog)
{
	free(prog->members);
}

///
//Sets the uniform variables needed by this shader program
//This function only sets the uniforms which remain constant between meshes.
//
//Parameters: 
//	prog: A pointer to the raytracer geometry shader program to set the constant uniforms of
//	buffer: A pointer to the rendering buffer to get the uniform values from
static void RayTracerGeometryShaderProgram_SetConstantUniforms(ShaderProgram* prog, RenderingBuffer* buffer)
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
//	prog: A pointer to the raytracer geometry shader program to set the variable uniforms of
//	cam: A pointer to the active camera containing information needed to calculate the uniforms
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void RayTracerGeometryShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj, unsigned int objID)
{
	RayTracerGeometryShaderProgram_Members* members = prog->members;
	
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

	float localMaterialVector[4] = 
	{
		material->ambientCoefficient,
		material->diffuseCoefficient,
		material->specularCoefficient,
		(float)objID
	};

	ProgramUniform4fv
	(
		prog->shaderProgramID,
		members->localMaterialLocation,
		1,
		localMaterialVector
	);

	float globalMaterialVector[4] = 
	{
		material->localCoefficient,
		material->reflectedCoefficient,
		material->transmittedCoefficient,
		material->indexOfRefraction
	};

	ProgramUniform4fv
	(
		prog->shaderProgramID,
		members->globalMaterialLocation,
		1,
		globalMaterialVector
	);

	ProgramUniform4fv
	(
		prog->shaderProgramID,
		members->specularColorLocation,
		1,
		material->specularColor
	);

	//Texture
	glActiveTexture(GL_TEXTURE0);
	GLuint textureID = AssetManager_LookupTextureByID(material->texturePoolID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	ProgramUniform1i(prog->shaderProgramID, members->textureLocation, 0);
}


///
//Renders a list of GObjects to the active frame buffer object using a raytracer geometry shader program
//Note: Sets the active shader program to this
//
//Parameters:
//	prog: A pointer to the RayTracerGeometryShaderProgram which will be rendering the objects
//	buffer: A pointer to the rendering buffer to render with
//	gameObjects: A pointer to a linked list containing GObjects to be rendered
static void RayTracerGeometryShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, LinkedList* gameObjects)
{
	glUseProgram(prog->shaderProgramID);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RayTracerGeometryShaderProgram_SetConstantUniforms(prog, buffer);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	struct LinkedList_Node* current = gameObjects->head;
	GObject* gameObj = NULL;
	while(current != NULL)
	{
		gameObj = (GObject*)current->data;
		if(gameObj->mesh != NULL)
		{
			//RayTracerGeometryShaderProgram_SetVariableUniforms(prog,  buffer->camera, gameObj);
			Mesh_Render(gameObj->mesh, gameObj->mesh->primitive);
		}
		current = current->next;
	}
	
	//glDisable
}

///
//Renders a memory pool of GObjects to the active frame buffer object using a raytracer geometry shader program
//
//Parameters:
//	prog: A pointer to the RayTracerGeometryShaderProgram which will be rendering the objects
//	buffer: A pointer to the rendering buffer to render with
//	pool: A pointer to the memory pool containing the GObjects to be rendered
static void RayTracerGeometryShaderProgram_RenderWithMemoryPool(ShaderProgram* prog, RenderingBuffer* buffer, MemoryPool* pool)
{
	glUseProgram(prog->shaderProgramID);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RayTracerGeometryShaderProgram_SetConstantUniforms(prog, buffer);

	//glEnable(GL_CULL_FACE);
	//glCullFace(GL_FRONT);

	//struct LinkedList_Node* current = gameObjects->head;
	GObject* gameObj = NULL;
	//while(current != NULL)
	for(unsigned int i = 0; i < pool->pool->capacity; i++)
	{
		gameObj = (GObject*)MemoryPool_RequestAddress(pool, i);
		if(gameObj->mesh != NULL)
		{
			RayTracerGeometryShaderProgram_SetVariableUniforms(prog,  buffer->camera, gameObj, i);
			Mesh_Render(gameObj->mesh, gameObj->mesh->primitive);
		}
	}
}
