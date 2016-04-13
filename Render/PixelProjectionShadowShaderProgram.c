#include "PixelProjectionShadowShaderProgram.h"

#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"
#include "../Manager/EnvironmentManager.h"
#include "../Compatibility/ProgramUniform.h"

#include "RayBuffer.h"

typedef struct PixelProjectionShadowShaderProgram_Members
{
	//Uniforms
	GLint modelMatrixLocation;
	GLint lightDirectionLocation;
	GLint rightDirectionLocation;
	GLint upDirectionLocation;
	GLint positionTextureLocation;
	GLint screenSizeLocation;
} PixelProjectionShadowShaderProgram_Members;

///
//Static function declarations

///
//Allocates a set of variables needed by a PixelProjectionShadowShaderProgram
//
//Returns:
//	a (void) pointer to a struct containing the set of uniforms/members the shader program needs.
static ShaderProgram_Members PixelProjectionShadowShaderProgram_AllocateMembers(void);

///
//Initializes the set of members needed by the shader
//
//Parameters:
//	prog: a pointer to the shader program to initialize the members of
static void PixelProjectionShadowShaderProgram_InitializeMembers(ShaderProgram* prog);

///
//Frees the memory used up by a DefferedShadowShaderProgram's members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of.
static void PixelProjectionShadowShaderProgram_FreeMembers(ShaderProgram* prog);

///
//Sets the uniform variables needed by this shader program
//which do not change between meshes
//
//Parameters:
//	prog: A pointer to the pixelprojection directional shader program to set the constant uniforms of
//	buffer: A pointer to the rendering buffer to get the uniform values of
static void PixelProjectionShadowShaderProgram_SetConstantUniforms(ShaderProgram* prog, struct RenderingBuffer* buffer);

///
//Sets the uniform variables needed by the shader program
//which do change between meshes
//
//Parameters:
//	prog: A pointer to the pixelprojection directional shader program to set the uniform variables of
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void PixelProjectionShadowShaderProgram_SetVariableUniforms(ShaderProgram* prog, GObject* gameObj);

///
//Renders shadows to the shadow texture
//
//Parameters:
//	prog: A pointer to the PixelProjectiondirectionalShaderProgram with which to render
//	buffer: A pointer to the active rendering buffer which is to be used for rendering
//	gameObjs: A pointer to a linked list containing gameObjs to render shadows of this frame
static void PixelProjectionShadowShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, LinkedList* gameObjs);

///
//Function Definitions
///

///
//Initializes the PixelProjection Shadow shader program
//
//Parameters:
//	prog: A pointer to the shader program to initialize as a PixelProjection Shadow shader program
void PixelProjectionShadowShaderProgram_Initialize(ShaderProgram* prog)
{
	ShaderProgram_InitializeWithGeometry(prog, "Shader/PixelProjectionShadowVertexShader.glsl", "Shader/PixelProjectionShadowGeometryShader.glsl", "Shader/PixelProjectionShadowFragmentShader.glsl");

	prog->members = PixelProjectionShadowShaderProgram_AllocateMembers();
	PixelProjectionShadowShaderProgram_InitializeMembers(prog);

	prog->FreeMembers = PixelProjectionShadowShaderProgram_FreeMembers;
	prog->Render = (ShaderProgram_RenderFunc)PixelProjectionShadowShaderProgram_Render;

	printf("\n***Important Variable Information***\n");
	int outputVertices = 0; 
	glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_VERTICES, &outputVertices);
	int outputComponents = 0;
	glGetIntegerv(GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS, &outputComponents);
	int invocations = 0;
	glGetIntegerv(GL_MAX_GEOMETRY_SHADER_INVOCATIONS, &invocations);

	printf("Max Vertices:\t%d\nMax Components:\t%d\nMax Invocations:\t%d\n", outputVertices, outputComponents, invocations);
	
}

///
//Allocates a set of variables needed by a PixelProjectionShadowShaderProgram
//
//Returns:
//	a (void) pointer to a struct containing the set of uniforms/members the shader program needs.
static ShaderProgram_Members PixelProjectionShadowShaderProgram_AllocateMembers(void)
{
	return malloc(sizeof(PixelProjectionShadowShaderProgram_Members));
}

///
//Initializes the set of members needed by the shader
//
//Parameters:
//	prog: a pointer to the shader program to initialize the members of
static void PixelProjectionShadowShaderProgram_InitializeMembers(ShaderProgram* prog)
{
	PixelProjectionShadowShaderProgram_Members* members = prog->members;

	glUseProgram(prog->shaderProgramID);

	members->positionTextureLocation = glGetUniformLocation(prog->shaderProgramID, "positionTexture");
	members->screenSizeLocation = glGetUniformLocation(prog->shaderProgramID, "screenSize");
	members->modelMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelMatrix");
	members->lightDirectionLocation = glGetUniformLocation(prog->shaderProgramID, "lightDirection");
	members->rightDirectionLocation = glGetUniformLocation(prog->shaderProgramID, "rightDirection");
	members->upDirectionLocation = glGetUniformLocation(prog->shaderProgramID, "upDirection");

	glUseProgram(0);
}

///
//Frees the memory used up by a DefferedShadowShaderProgram's members
//
//Parameters:
//	prog: A pointer to the shader program to free the members of.
static void PixelProjectionShadowShaderProgram_FreeMembers(ShaderProgram* prog)
{
	free(prog->members);
}

///
//Sets the uniform variables needed by this shader program
//which do not change between meshes
//
//Parameters:
//	prog: A pointer to the pixelprojection directional shader program to set the constant uniforms of
//	buffer: A pointer to the rendering buffer to get the uniform values of
//	gBuffer: A pointer to the geometry buffer containing the results from the geometry pass.
static void PixelProjectionShadowShaderProgram_SetConstantUniforms(ShaderProgram* prog, struct RenderingBuffer* buffer)
{
	PixelProjectionShadowShaderProgram_Members* members = (PixelProjectionShadowShaderProgram_Members*)prog->members;

	EnvironmentBuffer* eBuffer = EnvironmentManager_GetEnvironmentBuffer();

	Vector screenSizeVec;
	Vector_INIT_ON_STACK(screenSizeVec, 2);

	screenSizeVec.components[0] = (float)eBuffer->windowWidth;
	screenSizeVec.components[1] = (float)eBuffer->windowHeight;

	ProgramUniform2fv
	(
		prog->shaderProgramID,
		members->screenSizeLocation,
		1,
		screenSizeVec.components
	);

	ProgramUniform1i(prog->shaderProgramID, members->positionTextureLocation, RayBuffer_TextureType_POSITION);

	Vector rightDirection;
	Vector upDirection;

	Vector_INIT_ON_STACK(rightDirection, 3);
	Vector_INIT_ON_STACK(upDirection, 3);

	float* comps = buffer->directionalLight->direction->components;
	if(comps[2] < -0.9999999f)
	{
		rightDirection.components[0] = -1.0f;
		upDirection.components[1] = -1.0f;
	}
	else
	{
		float a = 1.0f / (1.0f + comps[2]);
		float b = -comps[0] * comps[1] * a;
		upDirection.components[0] = 1.0f - comps[0] * comps[0] * a;
		upDirection.components[1] = b;
		upDirection.components[2] = -comps[0];

		rightDirection.components[0] = b;
		rightDirection.components[1] = 1.0 - comps[1] * comps[1] * a;
		rightDirection.components[2] = -comps[1];


		//Vector_PrintTranspose(&rightDirection);
		//Vector_PrintTranspose(&upDirection);
		//Vector_PrintTranspose(buffer->directionalLight->direction);
	}

	ProgramUniform3fv
	(
		prog->shaderProgramID,
		members->lightDirectionLocation,
		1,
		comps
	);

	ProgramUniform3fv
	(
		prog->shaderProgramID,
		members->rightDirectionLocation,
		1,
		rightDirection.components
	);

	ProgramUniform3fv
	(
		prog->shaderProgramID,
		members->upDirectionLocation,
		1,
		upDirection.components
	);

}

///
//Sets the uniform variables needed by the shader program
//which do change between meshes
//
//Parameters:
//	prog: A pointer to the pixelprojection directional shader program to set the uniform variables of
//	gameObj: A pointer to the GObject containing the information needed for the uniforms
static void PixelProjectionShadowShaderProgram_SetVariableUniforms(ShaderProgram* prog, GObject* gameObj)
{
	PixelProjectionShadowShaderProgram_Members* members = prog->members;

	Matrix model;
	Matrix_INIT_ON_STACK(model, 4, 4);

	FrameOfReference_ToMatrix4(gameObj->frameOfReference, &model);
	
	ProgramUniformMatrix4fv
	(
		prog->shaderProgramID,
		members->modelMatrixLocation,
		1,
		GL_TRUE,
		model.components
	);
}

///
//Renders shadows to the shadow texture
//
//Parameters:
//	prog: A pointer to the PixelProjectiondirectionalShaderProgram with which to render
//	buffer: A pointer to the active rendering buffer which is to be used for rendering
//	gameObjs: A pointer to a linked list of gameObjects to render shadows of.
static void PixelProjectionShadowShaderProgram_Render(ShaderProgram* prog, RenderingBuffer* buffer, LinkedList* gameObjs)
{
	glUseProgram(prog->shaderProgramID);

	
	glEnable(GL_CLIP_DISTANCE0);
	glEnable(GL_CLIP_DISTANCE0 + 1);
	glEnable(GL_CLIP_DISTANCE0 + 2);
	glEnable(GL_CLIP_DISTANCE0 + 3);
	
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glClear(GL_COLOR_BUFFER_BIT);

	PixelProjectionShadowShaderProgram_SetConstantUniforms(prog, buffer); 

	struct LinkedList_Node* current = gameObjs->head;
	GObject* gameObj = NULL;

	while(current != NULL)
	{
		gameObj = (GObject*)current->data;
		if(gameObj->mesh != NULL)
		{
			PixelProjectionShadowShaderProgram_SetVariableUniforms(prog, gameObj);
			Mesh_Render(gameObj->mesh, gameObj->mesh->primitive);
		}
		current = current->next;
	}

	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	
	
	glDisable(GL_CLIP_DISTANCE0);
	glDisable(GL_CLIP_DISTANCE0 + 1);
	glDisable(GL_CLIP_DISTANCE0 + 2);
	glDisable(GL_CLIP_DISTANCE0 + 3);
	
	//glDisable(GL_CLIP_PLANE0);
	//glDisable(GL_CLIP_PLANE1);
	//glDisable(GL_CLIP_PLANE2);
	//glDisable(GL_CLIP_PLANE3);
}
