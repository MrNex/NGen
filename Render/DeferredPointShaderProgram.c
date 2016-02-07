#include "DeferredPointShaderProgram.h"

#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"
#include "../Manager/EnvironmentManager.h"

#include "../Compatibility/ProgramUniform.h"

#include "PointLight.h"
#include "GeometryBuffer.h"

struct DeferredPointShaderProgram_Members
{
	GLint modelViewProjectionMatrixLocation;

	GLint screenSizeLocation;

	GLint lightColorLocation;
	GLint lightPositionLocation;
	GLint ambientIntensityLocation;
	GLint diffuseIntensityLocation;

	GLint constantAttentuationLocation;
	GLint linearAttentuationLocation;
	GLint exponentAttentuationLocation;

	GLint positionTextureLocation;
	GLint diffuseTextureLocation;
	GLint normalTextureLocation;
};

///
//Static function declarations

///
//Allocates a set of variables needed by a DeferredPointShaderProgram
//
//Returns:
//	a (void) pointer to a struct containing the set of uniforms/members the shader program needs.
static ShaderProgram_Members DeferredPointShaderProgram_AllocateMembers(void);

///
//Initializes the set of members needed by the shader
//
//Parameters:
//	prog: a pointer to the shader program to initialize the members of
static void DeferredPointShaderProgram_InitializeMembers(ShaderProgram* prog);

///
//Frees the memory used up by a DefferedDirectionalShaderProgram's members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of.
static void DeferredPointShaderProgram_FreeMembers(ShaderProgram* prog);

///
//Sets the uniform variables needed by the shader program
//which do change between meshes
//
//Parameters:
//	prog: A pointer to the deferred directional shader program to set the uniform variables of
//	cam: A pointer to the active camera containing information needed to calculate the uniforms
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void DeferredPointShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj);

///
//Renders a frame after lighting due to a directional light
//
//Parameters:
//	prog: A pointer to the DeferreddirectionalShaderProgram with which to render
//	buffer: A pointer to the active rendering buffer which is to be used for rendering
//	renderArgs: A pointer to a linked list of GameObject's to be rendered
static void DeferredPointShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, GObject* gObject);

///
//Function definitions

void DeferredPointShaderProgram_Initialize(ShaderProgram* prog)
{
	ShaderProgram_Initialize(prog, "Shader/DeferredPointVertexShader.glsl", "Shader/DeferredPointFragmentShader.glsl");

	prog->members = DeferredPointShaderProgram_AllocateMembers();
	DeferredPointShaderProgram_InitializeMembers(prog);

	prog->FreeMembers = DeferredPointShaderProgram_FreeMembers;
	prog->Render = (ShaderProgram_RenderFunc)DeferredPointShaderProgram_Render;
}

///
//Allocates a set of variables needed by a DeferredPointShaderProgram
//
//Returns:
//	a (void) pointer to a struct containing the set of uniforms/members the shader program needs.
static ShaderProgram_Members DeferredPointShaderProgram_AllocateMembers(void)
{
	return malloc(sizeof(struct DeferredPointShaderProgram_Members));
}

///
//Initializes the set of members needed by the shader
//
//Parameters:
//	prog: a pointer to the shader program to initialize the members of
static void DeferredPointShaderProgram_InitializeMembers(ShaderProgram* prog)
{
	struct DeferredPointShaderProgram_Members* members = prog->members;

	glUseProgram(prog->shaderProgramID);
	
	members->modelViewProjectionMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelViewProjectionMatrix");
	
	members->screenSizeLocation = glGetUniformLocation(prog->shaderProgramID, "screenSize");

	members->lightColorLocation = glGetUniformLocation(prog->shaderProgramID, "lightColor");
	members->lightPositionLocation = glGetUniformLocation(prog->shaderProgramID, "lightPosition");
	members->ambientIntensityLocation = glGetUniformLocation(prog->shaderProgramID, "ambientIntensity");
	members->diffuseIntensityLocation = glGetUniformLocation(prog->shaderProgramID, "diffuseIntensity");

	members->constantAttentuationLocation = glGetUniformLocation(prog->shaderProgramID, "constantAttentuation");
	members->linearAttentuationLocation = glGetUniformLocation(prog->shaderProgramID, "linearAttentuation");
	members->exponentAttentuationLocation = glGetUniformLocation(prog->shaderProgramID, "exponentAttentuation");

	members->positionTextureLocation = glGetUniformLocation(prog->shaderProgramID, "positionTexture");
	members->diffuseTextureLocation = glGetUniformLocation(prog->shaderProgramID, "diffuseTexture");
	members->normalTextureLocation = glGetUniformLocation(prog->shaderProgramID, "normalTexture");

}

///
//Frees the memory used up by a DefferedDirectionalShaderProgram's members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of.
static void DeferredPointShaderProgram_FreeMembers(ShaderProgram* prog)
{
	free(prog->members);
}

///
//Sets the uniform variables needed by this shader program
//which do not change between meshes
//
//Parameters:
//	prog: A pointer to the deferred directional shader program to set the constant uniforms of
//	buffer: A pointer to the rendering buffer to get the uniform values of
//	gBuffer: A pointer to the geometry buffer containing the results from the geometry pass.
void DeferredPointShaderProgram_SetConstantUniforms(ShaderProgram* prog)
{
	struct DeferredPointShaderProgram_Members* members = (struct DeferredPointShaderProgram_Members*) prog->members;

	EnvironmentBuffer* eBuffer = EnvironmentManager_GetEnvironmentBuffer();
	
	Vector screenSizeVector;
	Vector_INIT_ON_STACK(screenSizeVector, 3);

	screenSizeVector.components[0] = (float)eBuffer->windowWidth;
	screenSizeVector.components[1] = (float)eBuffer->windowHeight;

	//Environment
	ProgramUniform2fv
	(
		prog->shaderProgramID,
		members->screenSizeLocation,
		1,
		screenSizeVector.components
	);

	//Geometry pass
	ProgramUniform1i(prog->shaderProgramID, members->positionTextureLocation, GeometryBuffer_TextureType_POSITION);
	ProgramUniform1i(prog->shaderProgramID, members->diffuseTextureLocation, GeometryBuffer_TextureType_DIFFUSE);
	ProgramUniform1i(prog->shaderProgramID, members->normalTextureLocation, GeometryBuffer_TextureType_NORMAL);

}

///
//Sets the uniform variables needed by the shader program
//which do change between meshes
//
//Parameters:
//	prog: A pointer to the deferred directional shader program to set the uniform variables of
//	cam: A pointer to the active camera containing information needed to calculate the uniforms
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void DeferredPointShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj)
{
	struct DeferredPointShaderProgram_Members* members = prog->members;
	PointLight* light = gameObj->light;


	//Calculate model view projection matrix
	FrameOfReference tempFrame;
	FrameOfReference_INIT_ON_STACK(tempFrame);

	Vector_Add(tempFrame.position, light->position, gameObj->frameOfReference->position); 

	float lightRadius = PointLight_CalculateRadius(light);

	Vector scaleVec;
	Vector_INIT_ON_STACK(scaleVec, 3);

	scaleVec.components[0] = scaleVec.components[1] = scaleVec.components[2] = lightRadius;

	FrameOfReference_Scale(&tempFrame, &scaleVec);
	
	Matrix modelMatrix;
	Matrix_INIT_ON_STACK(modelMatrix, 4, 4);
	FrameOfReference_ToMatrix4(&tempFrame, &modelMatrix);

	Matrix modelViewProjectionMatrix;
	Matrix_INIT_ON_STACK(modelViewProjectionMatrix, 4, 4);

	Matrix_GetProductMatrix(&modelViewProjectionMatrix, cam->viewMatrix, &modelMatrix);
	Matrix_TransformMatrix(cam->projectionMatrix, &modelViewProjectionMatrix);

	//Model view projection
	ProgramUniformMatrix4fv
	(
		prog->shaderProgramID,
		members->modelViewProjectionMatrixLocation,
		1,
		GL_TRUE,
		modelViewProjectionMatrix.components
	);

	//Lighting
	//Base
	ProgramUniform3fv
	(
		prog->shaderProgramID,
		members->lightColorLocation,
		1,
		light->base->color->components
	);
	ProgramUniform1f
	(
		prog->shaderProgramID,
		members->ambientIntensityLocation,
		light->base->ambientIntensity
	);
	ProgramUniform1f
	(
		prog->shaderProgramID,
		members->diffuseIntensityLocation,
		light->base->diffuseIntensity
	);
	//Position
	ProgramUniform3fv
	(
		prog->shaderProgramID,
		members->lightPositionLocation,
		1,
		tempFrame.position->components
	);
	//Attentuation
	ProgramUniform1f
	(
		prog->shaderProgramID,
		members->constantAttentuationLocation,
		light->attentuation.constant
	);
	ProgramUniform1f
	(
		prog->shaderProgramID,
		members->linearAttentuationLocation,
		light->attentuation.linear
	);
	ProgramUniform1f
	(
		prog->shaderProgramID,
		members->exponentAttentuationLocation,
		light->attentuation.exponent
	);
}

///
//Renders a frame after lighting due to a directional light
//
//Parameters:
//	prog: A pointer to the DeferreddirectionalShaderProgram with which to render
//	buffer: A pointer to the active rendering buffer which is to be used for rendering
//	gBuffer: A pointer to the geometry buffer containing information needed from the geommetry pass to render this frame.
static void DeferredPointShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, GObject* gObject)
{
	Mesh* sphere = AssetManager_LookupMesh("Sphere");

	glUseProgram(prog->shaderProgramID);


	DeferredPointShaderProgram_SetConstantUniforms(prog);

	glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

	//Now we should not update the depth buffer to perform the lightning pass.
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	//The lighting pass will need to blend the effects of the lighting with that of the geometry pass.
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);	//Will add the two resulting colors for each pixel
	glBlendFunc(GL_ONE, GL_ONE);	//Scales each by a factor of 1 when adding the geometry and lightning passes

	//glDisable(GL_CULL_FACE);

	glCullFace(GL_FRONT);

	DeferredPointShaderProgram_SetVariableUniforms(prog, buffer->camera, gObject);
	Mesh_Render(sphere, sphere->primitive);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);

	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);


	//glDisable(GL_STENCIL_TEST);
}


