#include "RayTracerPointShaderProgram.h"

#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"
#include "../Manager/EnvironmentManager.h"

#include "../Compatibility/ProgramUniform.h"

#include "PointLight.h"
#include "RayBuffer.h"

struct RayTracerPointShaderProgram_Members
{
	GLint modelViewProjectionMatrixLocation;

	GLint viewPositionLocation;

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
	GLint materialTextureLocation;
	GLint specularTextureLocation;
	GLint shadowTextureLocation;
};

///
//Static function declarations

///
//Allocates a set of variables needed by a RayTracerPointShaderProgram
//
//Returns:
//	a (void) pointer to a struct containing the set of uniforms/members the shader program needs.
static ShaderProgram_Members RayTracerPointShaderProgram_AllocateMembers(void);

///
//Initializes the set of members needed by the shader
//
//Parameters:
//	prog: a pointer to the shader program to initialize the members of
static void RayTracerPointShaderProgram_InitializeMembers(ShaderProgram* prog);

///
//Frees the memory used up by a RayTracerDirectionalShaderProgram's members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of.
static void RayTracerPointShaderProgram_FreeMembers(ShaderProgram* prog);

///
//Sets the uniform variables needed by the shader program
//which do change between meshes
//
//Parameters:
//	prog: A pointer to the raytracer directional shader program to set the uniform variables of
//	cam: A pointer to the active camera containing information needed to calculate the uniforms
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void RayTracerPointShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj);

///
//Renders a frame after lighting due to a directional light
//
//Parameters:
//	prog: A pointer to the RayTracerdirectionalShaderProgram with which to render
//	buffer: A pointer to the active rendering buffer which is to be used for rendering
//	renderArgs: A pointer to a linked list of GameObject's to be rendered
static void RayTracerPointShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, GObject* gObject);

///
//Function definitions

void RayTracerPointShaderProgram_Initialize(ShaderProgram* prog)
{
	ShaderProgram_Initialize(prog, "Shader/RayTracerPointVertexShader.glsl", "Shader/RayTracerPointFragmentShader.glsl");

	prog->members = RayTracerPointShaderProgram_AllocateMembers();
	RayTracerPointShaderProgram_InitializeMembers(prog);

	prog->FreeMembers = RayTracerPointShaderProgram_FreeMembers;
	prog->Render = (ShaderProgram_RenderFunc)RayTracerPointShaderProgram_Render;
}

///
//Allocates a set of variables needed by a RayTracerPointShaderProgram
//
//Returns:
//	a (void) pointer to a struct containing the set of uniforms/members the shader program needs.
static ShaderProgram_Members RayTracerPointShaderProgram_AllocateMembers(void)
{
	return malloc(sizeof(struct RayTracerPointShaderProgram_Members));
}

///
//Initializes the set of members needed by the shader
//
//Parameters:
//	prog: a pointer to the shader program to initialize the members of
static void RayTracerPointShaderProgram_InitializeMembers(ShaderProgram* prog)
{
	struct RayTracerPointShaderProgram_Members* members = prog->members;

	glUseProgram(prog->shaderProgramID);
	
	members->modelViewProjectionMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelViewProjectionMatrix");
	
	members->viewPositionLocation = glGetUniformLocation(prog->shaderProgramID, "viewPositionVector");

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
	members->materialTextureLocation = glGetUniformLocation(prog->shaderProgramID, "materialTexture");
	members->specularTextureLocation = glGetUniformLocation(prog->shaderProgramID, "specularTexture");
	members->shadowTextureLocation = glGetUniformLocation(prog->shaderProgramID, "shadowTexture");
}

///
//Frees the memory used up by a RayTracerDirectionalShaderProgram's members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of.
static void RayTracerPointShaderProgram_FreeMembers(ShaderProgram* prog)
{
	free(prog->members);
}

///
//Sets the uniform variables needed by this shader program
//which do not change between meshes
//
//Parameters:
//	prog: A pointer to the raytracer directional shader program to set the constant uniforms of
//	buffer: A pointer to the rendering buffer to get the uniform values of
//	gBuffer: A pointer to the geometry buffer containing the results from the geometry pass.
void RayTracerPointShaderProgram_SetConstantUniforms(ShaderProgram* prog)
{
	struct RayTracerPointShaderProgram_Members* members = (struct RayTracerPointShaderProgram_Members*) prog->members;

	EnvironmentBuffer* eBuffer = EnvironmentManager_GetEnvironmentBuffer();

	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;
	Vector camPos;
	Vector_INIT_ON_STACK(camPos, 3);

	Matrix_SliceColumn(&camPos, cam->translationMatrix, 3, 0, 3);

	Vector_Scale(&camPos, -1.0f);
	//Camera
	ProgramUniform3fv
	(
		prog->shaderProgramID,
		members->viewPositionLocation,
		1,
		camPos.components
	);


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
	ProgramUniform1i(prog->shaderProgramID, members->positionTextureLocation, RayBuffer_TextureType_POSITION);
	ProgramUniform1i(prog->shaderProgramID, members->diffuseTextureLocation, RayBuffer_TextureType_DIFFUSE);
	ProgramUniform1i(prog->shaderProgramID, members->normalTextureLocation, RayBuffer_TextureType_NORMAL);
	ProgramUniform1i(prog->shaderProgramID, members->materialTextureLocation, RayBuffer_TextureType_LOCALMATERIAL);
	ProgramUniform1i(prog->shaderProgramID, members->specularTextureLocation, RayBuffer_TextureType_SPECULAR);
	ProgramUniform1i(prog->shaderProgramID, members->shadowTextureLocation, RayBuffer_TextureType_SHADOW);
}

///
//Sets the uniform variables needed by the shader program
//which do change between meshes
//
//Parameters:
//	prog: A pointer to the raytracer directional shader program to set the uniform variables of
//	cam: A pointer to the active camera containing information needed to calculate the uniforms
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void RayTracerPointShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj)
{
	struct RayTracerPointShaderProgram_Members* members = prog->members;
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
//	prog: A pointer to the RayTracerdirectionalShaderProgram with which to render
//	buffer: A pointer to the active rendering buffer which is to be used for rendering
//	gBuffer: A pointer to the geometry buffer containing information needed from the geommetry pass to render this frame.
static void RayTracerPointShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, GObject* gObject)
{
	Mesh* sphere = AssetManager_LookupMesh("Sphere");

	glUseProgram(prog->shaderProgramID);


	RayTracerPointShaderProgram_SetConstantUniforms(prog);

	//Removed because of culling of objects inside of stencil sphere when camera is outside
	//glStencilFunc(GL_NOTEQUAL, 0, 0xFF);

	//Now we should not update the depth buffer to perform the lightning pass.
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	//The lighting pass will need to blend the effects of the lighting with that of the geometry pass.
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);	//Will add the two resulting colors for each pixel
	glBlendFunc(GL_ONE, GL_ONE);	//Scales each by a factor of 1 when adding the geometry and lightning passes

	//glDisable(GL_CULL_FACE);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	RayTracerPointShaderProgram_SetVariableUniforms(prog, buffer->camera, gObject);
	Mesh_Render(sphere, sphere->primitive);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);

	glCullFace(GL_BACK);
	glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);
}


