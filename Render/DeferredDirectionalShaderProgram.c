#include "DeferredDirectionalShaderProgram.h"

#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"
#include "../Manager/EnvironmentManager.h"

#include "../Compatibility/ProgramUniform.h"

#include "DirectionalLight.h"
#include "GeometryBuffer.h"

typedef struct DeferredDirectionalShaderProgram_Members
{
	//Uniforms
	GLint positionTextureLocation;
	GLint diffuseTextureLocation;
	GLint normalTextureLocation;

	GLint screenSizeLocation;

	GLint lightColorLocation;
	GLint lightDirectionLocation;
	GLint ambientIntensityLocation;
	GLint diffuseIntensityLocation;
} DeferredDirectionalShaderProgram_Members;

///
//Static function declarations

///
//Allocates a set of variables needed by a DeferredDirectionalShaderProgram
//
//Returns:
//	a (void) pointer to a struct containing the set of uniforms/members the shader program needs.
static ShaderProgram_Members DeferredDirectionalShaderProgram_AllocateMembers(void);

///
//Initializes the set of members needed by the shader
//
//Parameters:
//	prog: a pointer to the shader program to initialize the members of
static void DeferredDirectionalShaderProgram_InitializeMembers(ShaderProgram* prog);

///
//Frees the memory used up by a DefferedDirectionalShaderProgram's members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of.
static void DeferredDirectionalShaderProgram_FreeMembers(ShaderProgram* prog);

///
//Sets the uniform variables needed by this shader program
//which do not change between meshes
//
//Parameters:
//	prog: A pointer to the deferred directional shader program to set the constant uniforms of
//	buffer: A pointer to the rendering buffer to get the uniform values of
//	gBuffer: A pointer to the geometry buffer containing the results from the geometry pass.
static void DeferredDirectionalShaderProgram_SetConstantUniforms(ShaderProgram* prog, struct RenderingBuffer* buffer, GeometryBuffer* gBuffer);

///
//Sets the uniform variables needed by the shader program
//which do change between meshes
//
//Parameters:
//	prog: A pointer to the deferred directional shader program to set the uniform variables of
//	cam: A pointer to the active camera containing information needed to calculate the uniforms
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void DeferredDirectionalShaderProgram_SetVariableUniforms(ShaderProgram* prog, Camera* cam, GObject* gameObj);

///
//Renders a frame after lighting due to a directional light
//
//Parameters:
//	prog: A pointer to the DeferreddirectionalShaderProgram with which to render
//	buffer: A pointer to the active rendering buffer which is to be used for rendering
//	gBuffer: A pointer to the geometry buffer containing information needed from the geommetry pass to render this frame.
static void DeferredDirectionalShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, GeometryBuffer* gBuffer);

///
//Function Definitions
///

///
//Initializes the Deferred Directional shader program
//
//Parameters:
//	prog: A pointer to the shader program to initialize as a deferred directional shader program
void DeferredDirectionalShaderProgram_Initialize(ShaderProgram* prog)
{
	ShaderProgram_Initialize(prog, "Shader/DeferredDirectionalVertexShader.glsl", "Shader/DeferredDirectionalFragmentShader.glsl");	

	prog->members = DeferredDirectionalShaderProgram_AllocateMembers();
	DeferredDirectionalShaderProgram_InitializeMembers(prog);

	prog->FreeMembers = DeferredDirectionalShaderProgram_FreeMembers;
	prog->Render = (ShaderProgram_RenderFunc)DeferredDirectionalShaderProgram_Render;


}


///
//Allocates a set of variables needed by a DeferredDirectionalShaderProgram
//
//Returns:
//	a (void) pointer to a struct containing the set of uniforms/members the shader program needs.
static ShaderProgram_Members DeferredDirectionalShaderProgram_AllocateMembers(void)
{
	return malloc(sizeof(DeferredDirectionalShaderProgram_Members));
}

///
//Initializes the set of members needed by the shader
//
//Parameters:
//	prog: a pointer to the shader program to initialize the members of
static void DeferredDirectionalShaderProgram_InitializeMembers(ShaderProgram* prog)
{
	DeferredDirectionalShaderProgram_Members* members = prog->members;

	glUseProgram(prog->shaderProgramID);

	members->positionTextureLocation = glGetUniformLocation(prog->shaderProgramID, "positionTexture");
	members->diffuseTextureLocation = glGetUniformLocation(prog->shaderProgramID, "diffuseTexture");
	members->normalTextureLocation = glGetUniformLocation(prog->shaderProgramID, "normalTexture");

	members->screenSizeLocation = glGetUniformLocation(prog->shaderProgramID, "screenSize");

	members->lightColorLocation = glGetUniformLocation(prog->shaderProgramID, "lightColor");
	members->lightDirectionLocation = glGetUniformLocation(prog->shaderProgramID, "lightDirection");
	members->ambientIntensityLocation = glGetUniformLocation(prog->shaderProgramID, "ambientIntensity");
	members->diffuseIntensityLocation = glGetUniformLocation(prog->shaderProgramID, "diffuseIntensity");

	glUseProgram(0);
}

///
//Frees the memory used up by a DefferedDirectionalShaderProgram's members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of.
static void DeferredDirectionalShaderProgram_FreeMembers(ShaderProgram* prog)
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
//	gBuffer: A pointer to the geometry buffer containing the results of the geometry pass.
static void DeferredDirectionalShaderProgram_SetConstantUniforms(ShaderProgram* prog, struct RenderingBuffer* buffer, GeometryBuffer* gBuffer)
{
	DeferredDirectionalShaderProgram_Members* members = (DeferredDirectionalShaderProgram_Members*)prog->members;
	
	EnvironmentBuffer* eBuffer = EnvironmentManager_GetEnvironmentBuffer();

	Vector screenSizeVec;
	Vector_INIT_ON_STACK(screenSizeVec, 2);

	screenSizeVec.components[0] = (float)eBuffer->windowWidth;
	screenSizeVec.components[1] = (float)eBuffer->windowHeight;


	//GeometryBuffer_BindForReading(gBuffer);

	//Geometry pass
	ProgramUniform1i(prog->shaderProgramID, members->positionTextureLocation, GeometryBuffer_TextureType_POSITION);
	ProgramUniform1i(prog->shaderProgramID, members->diffuseTextureLocation, GeometryBuffer_TextureType_DIFFUSE);
	ProgramUniform1i(prog->shaderProgramID, members->normalTextureLocation, GeometryBuffer_TextureType_NORMAL);


	//Environment
	ProgramUniform2fv
	(
		prog->shaderProgramID,
		members->screenSizeLocation,
		1,
		screenSizeVec.components
	);

	
	//Lighting
	ProgramUniform3fv
	(
		prog->shaderProgramID,
		members->lightColorLocation,
		1,
		buffer->directionalLight->base->color->components
	);

	ProgramUniform3fv
	(
		prog->shaderProgramID,
		members->lightDirectionLocation,
		1,
		buffer->directionalLight->direction->components
	);

	ProgramUniform1f
	(
		prog->shaderProgramID,
		members->ambientIntensityLocation,
		buffer->directionalLight->base->ambientIntensity
	);

	ProgramUniform1f
	(
		prog->shaderProgramID,
		members->diffuseIntensityLocation,
		buffer->directionalLight->base->diffuseIntensity
	);
}

///
//Renders a frame after lighting due to a directional light
//
//Parameters:
//	prog: A pointer to the DeferreddirectionalShaderProgram with which to render
//	buffer: A pointer to the active rendering buffer which is to be used for rendering
//	light: A pointer to the directional light who's effects are being rendered.
static void DeferredDirectionalShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, GeometryBuffer* gBuffer)
{
	glUseProgram(prog->shaderProgramID);


	//Now we should not update the depth buffer to perform the lightning pass.
	//glDepthMask(GL_FALSE);
	//glDisable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);


	//The lighting pass will need to blend the effects of the lighting with that of the geometry pass.
	//glEnable(GL_BLEND);
	//glBlendEquation(GL_FUNC_ADD);	//Will add the two resulting colors for each pixel
	//glBlendFunc(GL_ONE, GL_ONE);	//Scales each by a factor of 1 when adding the geometry and lightning passes
	
	DeferredDirectionalShaderProgram_SetConstantUniforms(prog, buffer, gBuffer);

	//glClear(GL_COLOR_BUFFER_BIT);
	
	Mesh_Render(AssetManager_LookupMesh("Square"), GL_TRIANGLES);

	//glDisable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ZERO);

	//glDepthMask(GL_TRUE);
	//glEnable(GL_DEPTH_TEST);


	glEnable(GL_CULL_FACE);
}
