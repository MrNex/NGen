#include "RayTracerDirectionalShaderProgram.h"

#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"
#include "../Manager/EnvironmentManager.h"

#include "../Compatibility/ProgramUniform.h"

#include "DirectionalLight.h"
#include "RayBuffer.h"

typedef struct RayTracerDirectionalShaderProgram_Members
{
	//Uniforms
	GLint positionTextureLocation;
	GLint diffuseTextureLocation;
	GLint normalTextureLocation;
	GLint localMaterialTextureLocation;
	GLint specularTextureLocation;
	GLint shadowTextureLocation;
	GLint globalMaterialTextureLocation;


	GLint viewPositionLocation;

	GLint screenSizeLocation;

	GLint lightColorLocation;
	GLint lightDirectionLocation;
	GLint ambientIntensityLocation;
	GLint diffuseIntensityLocation;

	GLint isLocalLocation;
	GLint isReflectionLocation;
	GLint isTransmissionLocation;

} RayTracerDirectionalShaderProgram_Members;

///
//Static function declarations

///
//Allocates a set of variables needed by a RayTracerDirectionalShaderProgram
//
//Returns:
//	a (void) pointer to a struct containing the set of uniforms/members the shader program needs.
static ShaderProgram_Members RayTracerDirectionalShaderProgram_AllocateMembers(void);

///
//Initializes the set of members needed by the shader
//
//Parameters:
//	prog: a pointer to the shader program to initialize the members of
static void RayTracerDirectionalShaderProgram_InitializeMembers(ShaderProgram* prog);

///
//Frees the memory used up by a RayTracerDirectionalShaderProgram's members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of.
static void RayTracerDirectionalShaderProgram_FreeMembers(ShaderProgram* prog);

///
//Sets the uniform variables needed by this shader program
//which do not change between meshes
//
//Parameters:
//	prog: A pointer to the raytracer directional shader program to set the constant uniforms of
//	buffer: A pointer to the rendering buffer to get the uniform values of
//	gBuffer: A pointer to the geometry buffer containing the results from the geometry pass.
static void RayTracerDirectionalShaderProgram_SetConstantUniforms(ShaderProgram* prog, struct RenderingBuffer* buffer, unsigned int* passType);

///
//Renders a frame after lighting due to a directional light
//
//Parameters:
//	prog: A pointer to the RayTracerdirectionalShaderProgram with which to render
//	buffer: A pointer to the active rendering buffer which is to be used for rendering
//	rBuffer: A pointer to the ray buffer containing information needed from the geommetry pass to render this frame.
static void RayTracerDirectionalShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, RayBuffer* rBuffer);

///
//Function Definitions
///

///
//Initializes the RayTracer Directional shader program
//
//Parameters:
//	prog: A pointer to the shader program to initialize as a raytracer directional shader program
void RayTracerDirectionalShaderProgram_Initialize(ShaderProgram* prog)
{
	ShaderProgram_Initialize(prog, "Shader/RayTracerDirectionalVertexShader.glsl", "Shader/RayTracerDirectionalFragmentShader.glsl");	

	prog->members = RayTracerDirectionalShaderProgram_AllocateMembers();
	RayTracerDirectionalShaderProgram_InitializeMembers(prog);

	prog->FreeMembers = RayTracerDirectionalShaderProgram_FreeMembers;
	prog->Render = (ShaderProgram_RenderFunc)RayTracerDirectionalShaderProgram_Render;


}


///
//Allocates a set of variables needed by a RayTracerDirectionalShaderProgram
//
//Returns:
//	a (void) pointer to a struct containing the set of uniforms/members the shader program needs.
static ShaderProgram_Members RayTracerDirectionalShaderProgram_AllocateMembers(void)
{
	return malloc(sizeof(RayTracerDirectionalShaderProgram_Members));
}

///
//Initializes the set of members needed by the shader
//
//Parameters:
//	prog: a pointer to the shader program to initialize the members of
static void RayTracerDirectionalShaderProgram_InitializeMembers(ShaderProgram* prog)
{
	RayTracerDirectionalShaderProgram_Members* members = prog->members;

	glUseProgram(prog->shaderProgramID);

	members->positionTextureLocation = glGetUniformLocation(prog->shaderProgramID, "positionTexture");
	members->diffuseTextureLocation = glGetUniformLocation(prog->shaderProgramID, "diffuseTexture");
	members->normalTextureLocation = glGetUniformLocation(prog->shaderProgramID, "normalTexture");
	members->localMaterialTextureLocation = glGetUniformLocation(prog->shaderProgramID, "localMaterialTexture");
	members->specularTextureLocation = glGetUniformLocation(prog->shaderProgramID, "specularTexture");
	members->shadowTextureLocation = glGetUniformLocation(prog->shaderProgramID, "shadowTexture");
	members->globalMaterialTextureLocation = glGetUniformLocation(prog->shaderProgramID, "globalMaterialTexture");

	members->viewPositionLocation = glGetUniformLocation(prog->shaderProgramID, "viewPositionVector");

	members->screenSizeLocation = glGetUniformLocation(prog->shaderProgramID, "screenSize");

	members->lightColorLocation = glGetUniformLocation(prog->shaderProgramID, "lightColor");
	members->lightDirectionLocation = glGetUniformLocation(prog->shaderProgramID, "lightDirection");
	members->ambientIntensityLocation = glGetUniformLocation(prog->shaderProgramID, "ambientIntensity");
	members->diffuseIntensityLocation = glGetUniformLocation(prog->shaderProgramID, "diffuseIntensity");

	members->isLocalLocation = glGetUniformLocation(prog->shaderProgramID, "isLocal");
	members->isReflectionLocation = glGetUniformLocation(prog->shaderProgramID, "isReflection");
	members->isTransmissionLocation = glGetUniformLocation(prog->shaderProgramID, "isTransmission");

	glUseProgram(0);
}

///
//Frees the memory used up by a RayTracerDirectionalShaderProgram's members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of.
static void RayTracerDirectionalShaderProgram_FreeMembers(ShaderProgram* prog)
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
//	gBuffer: A pointer to the geometry buffer containing the results of the geometry pass.
static void RayTracerDirectionalShaderProgram_SetConstantUniforms(ShaderProgram* prog, struct RenderingBuffer* buffer, unsigned int* passType)
{
	RayTracerDirectionalShaderProgram_Members* members = (RayTracerDirectionalShaderProgram_Members*)prog->members;
	
	EnvironmentBuffer* eBuffer = EnvironmentManager_GetEnvironmentBuffer();

	Vector screenSizeVec;
	Vector_INIT_ON_STACK(screenSizeVec, 2);

	screenSizeVec.components[0] = (float)eBuffer->windowWidth;
	screenSizeVec.components[1] = (float)eBuffer->windowHeight;


	//GeometryBuffer_BindForReading(gBuffer);

	//Geometry pass
	ProgramUniform1i(prog->shaderProgramID, members->positionTextureLocation, RayBuffer_TextureType_POSITION);
	ProgramUniform1i(prog->shaderProgramID, members->diffuseTextureLocation, RayBuffer_TextureType_DIFFUSE);
	ProgramUniform1i(prog->shaderProgramID, members->normalTextureLocation, RayBuffer_TextureType_NORMAL);
	ProgramUniform1i(prog->shaderProgramID, members->localMaterialTextureLocation, RayBuffer_TextureType_LOCALMATERIAL);
	ProgramUniform1i(prog->shaderProgramID, members->specularTextureLocation, RayBuffer_TextureType_SPECULAR);
	ProgramUniform1i(prog->shaderProgramID, members->shadowTextureLocation, RayBuffer_TextureType_SHADOW);
	ProgramUniform1i(prog->shaderProgramID, members->globalMaterialTextureLocation, RayBuffer_TextureType_GLOBALMATERIAL);

	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;
	Vector camPos;
	Vector_INIT_ON_STACK(camPos, 3);

	Matrix_SliceColumn(&camPos, cam->translationMatrix, 3, 0, 3);

	Vector_Scale(&camPos, -1.0f);
	
	ProgramUniform3fv
	(
		prog->shaderProgramID,
		members->viewPositionLocation,
		1,
		camPos.components
	);

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

	glProgramUniform1ui
	(
		prog->shaderProgramID,
		members->isLocalLocation,
		passType[0]
	);

	glProgramUniform1ui
	(
		prog->shaderProgramID,
		members->isReflectionLocation,
		passType[1]
	);

	glProgramUniform1ui
	(
		prog->shaderProgramID,
		members->isTransmissionLocation,
		passType[2]
	);
}

///
//Renders a frame after lighting due to a directional light
//
//Parameters:
//	prog: A pointer to the RayTracerdirectionalShaderProgram with which to render
//	buffer: A pointer to the active rendering buffer which is to be used for rendering
//	light: A pointer to the directional light who's effects are being rendered.
static void RayTracerDirectionalShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, RayBuffer* rBuffer)
{
	(void)rBuffer;

	glUseProgram(prog->shaderProgramID);


	//Now we should not update the depth buffer to perform the lightning pass.
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);


	//The lighting pass will need to blend the effects of the lighting with that of the geometry pass.
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);	//Will add the two resulting colors for each pixel
	glBlendFunc(GL_ONE, GL_ONE);	//Scales each by a factor of 1 when adding the geometry and lightning passes
	
	RayTracerDirectionalShaderProgram_SetConstantUniforms(prog, buffer, rBuffer->passType);

	//glClear(GL_COLOR_BUFFER_BIT);
	
	Mesh_Render(AssetManager_LookupMesh("Square"), GL_TRIANGLES);

	glDisable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ZERO);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);


	glEnable(GL_CULL_FACE);
}
