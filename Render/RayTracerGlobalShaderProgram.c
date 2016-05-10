#include "RayTracerGlobalShaderProgram.h"

#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"
#include "../Manager/EnvironmentManager.h"

#include "../Compatibility/ProgramUniform.h"

#include "GlobalBuffer.h"

typedef struct RayTracerGlobalShaderProgram_Members
{
	GLint localTextureLocation;
	GLint reflectionTextureLocation;
	GLint transmissionTextureLocation;
	GLint globalMaterialTextureLocation;
	GLint transMaterialTextureLocation;

	GLint screenSizeLocation;
} RayTracerGlobalShaderProgram_Members;

///
//Static function declarations
///

///
//Allocates a set of varaibles needed by the shader program
//
//Returns:
//	a (void) pointer to a struct containing the set of members the shader program needs
static ShaderProgram_Members RayTracerGlobalShaderProgram_AllocateMembers(void);

///
//Initializes the set of members needed by the shader program
//
//Parameters:
//	prog: A pointer to the shader program to initialize the members of
static void RayTracerGlobalShaderProgram_InitializeMembers(ShaderProgram* prog);

///
//Frees the memory used up by this shader programs internal members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of
static void RayTracerGlobalShaderProgram_FreeMembers(ShaderProgram* prog);

///
//Sets the uniform variables needed by this shader program
//Which do not vary between meshes
//
//Parameters:
//	prog: A pointer to the RayTracerGlobal shader program to set the constant uniforms of
static void RayTracerGlobalShaderProgram_SetConstantUniforms(ShaderProgram* prog);

///
//Renders a frame after global illumination calculations
//
//Parameters:
//	prog: A pointer to the RayTracerGlobalShaderProgram to render with
//	buffer: A pointer to the active rendering buffer 
//	gBuffer: A pointer to the global buffer containing global illumination information necessary
static void RayTracerGlobalShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, GlobalBuffer* gBuffer);

///
//Function Definitions
///

///
//Initializes the RayTracerGlobal Shader Program
//
//Parameters:
//	prog: A pointer to the shader program to initialize as a RayTracerGlobalShaderProgram
void RayTracerGlobalShaderProgram_Initialize(ShaderProgram* prog)
{
	ShaderProgram_Initialize(prog, "Shader/RayTracerGlobalVertexShader.glsl", "Shader/RayTracerGlobalFragmentShader.glsl");

	prog->members = RayTracerGlobalShaderProgram_AllocateMembers();
	RayTracerGlobalShaderProgram_InitializeMembers(prog);

	prog->FreeMembers = RayTracerGlobalShaderProgram_FreeMembers;
	prog->Render = (ShaderProgram_RenderFunc)RayTracerGlobalShaderProgram_Render;
}

///
//Allocates a set of varaibles needed by the shader program
//
//Returns:
//	a (void) pointer to a struct containing the set of members the shader program needs
static ShaderProgram_Members RayTracerGlobalShaderProgram_AllocateMembers(void)
{
	return malloc(sizeof(RayTracerGlobalShaderProgram_Members));
}

///
//Initializes the set of members needed by the shader program
//
//Parameters:
//	prog: A pointer to the shader program to initialize the members of
static void RayTracerGlobalShaderProgram_InitializeMembers(ShaderProgram* prog)
{
	RayTracerGlobalShaderProgram_Members* members = prog->members;
	
	glUseProgram(prog->shaderProgramID);

	members->localTextureLocation = glGetUniformLocation(prog->shaderProgramID, "localTexture");
	members->reflectionTextureLocation = glGetUniformLocation(prog->shaderProgramID, "reflectionTexture");
	members->transmissionTextureLocation = glGetUniformLocation(prog->shaderProgramID, "transmissionTexture");
	members->globalMaterialTextureLocation = glGetUniformLocation(prog->shaderProgramID, "globalMaterialTexture");
	members->transMaterialTextureLocation = glGetUniformLocation(prog->shaderProgramID, "transMaterialTexture");
	members->screenSizeLocation = glGetUniformLocation(prog->shaderProgramID, "screenSizeVector");

	glUseProgram(0);

}

///
//Frees the memory used up by this shader programs internal members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of
static void RayTracerGlobalShaderProgram_FreeMembers(ShaderProgram* prog)
{
	free(prog->members);
}

///
//Sets the uniform variables needed by this shader program
//Which do not vary between meshes
//
//Parameters:
//	prog: A pointer to the RayTracerGlobal shader program to set the constant uniforms of
static void RayTracerGlobalShaderProgram_SetConstantUniforms(ShaderProgram* prog)
{
	RayTracerGlobalShaderProgram_Members* members = (RayTracerGlobalShaderProgram_Members*)prog->members;

	EnvironmentBuffer* eBuffer = EnvironmentManager_GetEnvironmentBuffer();

	float screenSizeVec[2] = { (float)eBuffer->windowWidth, (float)eBuffer->windowHeight };

	//Textures
	ProgramUniform1i(prog->shaderProgramID, members->localTextureLocation, GlobalBuffer_TextureType_LOCAL);	
	ProgramUniform1i(prog->shaderProgramID, members->reflectionTextureLocation, GlobalBuffer_TextureType_REFLECTION);
	ProgramUniform1i(prog->shaderProgramID, members->transmissionTextureLocation, GlobalBuffer_TextureType_TRANSMISSION);
	ProgramUniform1i(prog->shaderProgramID, members->globalMaterialTextureLocation, GlobalBuffer_TextureType_GLOBALMATERIAL);
	ProgramUniform1i(prog->shaderProgramID, members->transMaterialTextureLocation, GlobalBuffer_TextureType_TRANSMATERIAL);


	//other Uniforms
	//Screen size
	ProgramUniform2fv
	(
		prog->shaderProgramID,
		members->screenSizeLocation,
		1,
		screenSizeVec
	);
}

///
//Renders a frame after global illumination calculations
//
//Parameters:
//	prog: A pointer to the RayTracerGlobalShaderProgram to render with
//	buffer: A pointer to the active rendering buffer 
//	gBuffer: A pointer to the global buffer containing global illumination information necessary
static void RayTracerGlobalShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, GlobalBuffer* gBuffer)
{
	//Parameters are unused
	(void)buffer;
	(void)gBuffer;
	glUseProgram(prog->shaderProgramID);

	glDisable(GL_CULL_FACE);
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	glDisable(GL_CULL_FACE);


	//The lighting pass will need to blend the effects of the lighting with that of the geometry pass.
	//glEnable(GL_BLEND);
	//glBlendEquation(GL_FUNC_ADD);	//Will add the two resulting colors for each pixel
	//glBlendFunc(GL_ONE, GL_ONE);	//Scales each by a factor of 1 when adding the geometry and lightning passes

	RayTracerGlobalShaderProgram_SetConstantUniforms(prog);

	Mesh_Render(AssetManager_LookupMesh("Square"), GL_TRIANGLES);
	//glDisable(GL_BLEND);
	//glBlendFunc(GL_ONE, GL_ZERO);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);
}
