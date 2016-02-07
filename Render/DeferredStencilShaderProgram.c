#include "DeferredStencilShaderProgram.h"

#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"

#include "../Compatibility/ProgramUniform.h"

struct DeferredStencilShaderProgram_Members
{
	GLint modelViewProjectionMatrixLocation;
};

///
//Static function declarations

///
//Allocates a set of variables needed by a DeferredStencilShaderProgram
//
//Returns:
//	a (void) pointer to a struct containing the set of uniforms/members the shader program needs.
static ShaderProgram_Members DeferredStencilShaderProgram_AllocateMembers(void);

///
//Initializes the set of members needed by the shader
//
//Parameters:
//	prog: a pointer to the shader program to initialize the members of
static void DeferredStencilShaderProgram_InitializeMembers(ShaderProgram* prog);

///
//Frees the memory used up by a DefferedStencilShaderProgram's members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of.
static void DeferredStencilShaderProgram_FreeMembers(ShaderProgram* prog);

///
//Sets the uniform variables needed by the shader program
//which do change between meshes
//
//Parameters:
//	prog: A pointer to the deferred setncil shader program to set the uniform variables of
//	cam: A pointer to the active camera containing information needed to calculate the uniforms
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void DeferredStencilShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj);

///
//Renders stencils of point lights to the depth buffers stencil component
//
//Parameters:
//	prog: A pointer to the DeferredStencilShaderProgram with which to render
//	buffer: A pointer to the active rendering buffer which is to be used for rendering
//	gObject: A pointer to the GObject which has a point light that must be rendered
static void DeferredStencilShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, GObject* gObject);


///
//Initializes the deferred stencil shader program
//
//Parameters:
//	prog: A pointer to the shader program
void DeferredStencilShaderProgram_Initialize(ShaderProgram* prog)
{
	ShaderProgram_Initialize(prog, "Shader/DeferredPointVertexShader.glsl", "Shader/NullFragmentShader.glsl");

	prog->members = DeferredStencilShaderProgram_AllocateMembers();
	DeferredStencilShaderProgram_InitializeMembers(prog);

	prog->FreeMembers = DeferredStencilShaderProgram_FreeMembers;
	prog->Render = (ShaderProgram_RenderFunc)DeferredStencilShaderProgram_Render;
}

///
//Allocates a set of variables needed by a DeferredStencilShaderProgram
//
//Returns:
//	a (void) pointer to a struct containing the set of uniforms/members the shader program needs.
static ShaderProgram_Members DeferredStencilShaderProgram_AllocateMembers(void)
{
	return malloc(sizeof(struct DeferredStencilShaderProgram_Members));
}

///
//Initializes the set of members needed by the shader
//
//Parameters:
//	prog: a pointer to the shader program to initialize the members of
static void DeferredStencilShaderProgram_InitializeMembers(ShaderProgram* prog)
{
	struct DeferredStencilShaderProgram_Members* members = prog->members;

	glUseProgram(prog->shaderProgramID);

	members->modelViewProjectionMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelViewProjectionMatrix");
}

///
//Frees the memory used up by a DefferedStencilShaderProgram's members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of.
static void DeferredStencilShaderProgram_FreeMembers(ShaderProgram* prog)
{
	free(prog->members);
}

///
//Sets the uniform variables needed by the shader program
//which do change between meshes
//
//Parameters:
//	prog: A pointer to the deferred setncil shader program to set the uniform variables of
//	cam: A pointer to the active camera containing information needed to calculate the uniforms
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void DeferredStencilShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj)
{
	struct DeferredStencilShaderProgram_Members* members = prog->members;
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

	Matrix modelViewProjectionMatrix;
	Matrix_INIT_ON_STACK(modelViewProjectionMatrix, 4, 4);

	FrameOfReference_ToMatrix4(&tempFrame, &modelViewProjectionMatrix);

	Matrix_TransformMatrix(cam->viewMatrix, &modelViewProjectionMatrix);
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
}

///
//Renders stencils of point lights to the depth buffers stencil component
//
//Parameters:
//	prog: A pointer to the DeferredStencilShaderProgram with which to render
//	buffer: A pointer to the active rendering buffer which is to be used for rendering
//	gObject: A pointer to the GObject which has a point light that must be rendered
static void DeferredStencilShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, GObject* gObject)
{
	Mesh* sphere = AssetManager_LookupMesh("Sphere");

	glUseProgram(prog->shaderProgramID);

	//glEnable(GL_STENCIL_TEST);
	glDisable(GL_CULL_FACE);

	glClear(GL_STENCIL_BUFFER_BIT);
	glStencilFunc(GL_ALWAYS, 0, 0);

	glStencilOpSeparate(GL_BACK, GL_KEEP, GL_INCR_WRAP, GL_KEEP);
	glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_DECR_WRAP, GL_KEEP);

	DeferredStencilShaderProgram_SetVariableUniforms(prog, buffer->camera, gObject);

	Mesh_Render(sphere, sphere->primitive);

	glEnable(GL_CULL_FACE);
	
}
