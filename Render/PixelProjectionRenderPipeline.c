#include "PixelProjectionRenderPipeline.h"

#include "../Manager/EnvironmentManager.h"

#include "PixelProjectionGeometryShaderProgram.h"
#include "PixelProjectionShadowShaderProgram.h"
#include "PixelProjectionDirectionalShaderProgram.h"

#include "../Data/LinkedList.h"

#include "../GObject/GObject.h"

#include "RayBuffer.h"

typedef struct PixelProjectionRenderPipeline_Members
{
	RayBuffer* rBuffer;
} PixelProjectionRenderPipeline_Members;

///
//Static Declarations
///

///
//Allocates a new set of members for the PixelProjectionRenderPipeline
//
//Returns:
//	A pointer to an uninitialized PixelProjectionRenderPipeline_Members
static RenderPipeline_Members* PixelProjectionRenderPipeline_AllocateMembers(void);

///
//Initializes a set of PixelProjectionRenderPipeline members
//
//Parameters:
//	mems: A pointer to the set of PixelProjectionRenderPipeline members to
static void PixelProjectionRenderPipeline_InitializeMembers(RenderPipeline_Members* mems);

///
//Frees memory allocated by the PixelProjectionRenderPipeline's members
//
//Parameters:
//	mems: A pointer to the PixelProjectionRenderPipeline_Members to free
static void PixelProjectionRenderPipeline_FreeMembers(RenderPipeline_Members* mems);

///
//Renders a set of gameobjects using a specified render pipeline
//
//Parameters:
//	Pipeline: A pointer to the PixelProjectionRenderPipeline to render with.
//	buffer: A pointer to the RenderingBuffer to render with
//	gameObjs: A pointer to a linked list of GObjects to render
static void PixelProjectionRenderPipeline_Render(RenderPipeline* pipeline, struct RenderingBuffer* buffer, LinkedList* gameObjs);

///
//Function Definitions
///

///
//Intializes a new PixelProjectionRenderPipeline
//
//Parameters:
//	pipeline: A pointer to the RenderPipeline to initialize as a PixelProjectionRenderPipeline
void PixelProjectionRenderPipeline_Initialize(RenderPipeline* pipeline)
{
	RenderPipeline_Initialize(pipeline, 3);

	pipeline->members = PixelProjectionRenderPipeline_AllocateMembers();
	PixelProjectionRenderPipeline_InitializeMembers(pipeline->members);

	pipeline->programs[0] = ShaderProgram_Allocate();
	PixelProjectionGeometryShaderProgram_Initialize(pipeline->programs[0]);

	pipeline->programs[1] = ShaderProgram_Allocate();
	PixelProjectionShadowShaderProgram_Initialize(pipeline->programs[1]);

	pipeline->programs[2] = ShaderProgram_Allocate();
	PixelProjectionDirectionalShaderProgram_Initialize(pipeline->programs[2]);

	pipeline->Render = (RenderPipeline_RenderFunc)PixelProjectionRenderPipeline_Render;
	pipeline->FreeMembers = PixelProjectionRenderPipeline_FreeMembers;
}

///
//Allocates a new set of members for the PixelProjectionRenderPipeline
//
//Returns:
//	A pointer to an uninitialized PixelProjectionRenderPipeline_Members
static RenderPipeline_Members* PixelProjectionRenderPipeline_AllocateMembers(void)
{
	return malloc(sizeof(PixelProjectionRenderPipeline_Members));
}

///
//Initializes a set of PixelProjectionRenderPipeline members
//
//Parameters:
//	mems: A pointer to the set of PixelProjectionRenderPipeline members to initialize
static void PixelProjectionRenderPipeline_InitializeMembers(RenderPipeline_Members* mems)
{
	PixelProjectionRenderPipeline_Members* members = (PixelProjectionRenderPipeline_Members*)mems;
	EnvironmentBuffer* envBuffer = EnvironmentManager_GetEnvironmentBuffer();
	members->rBuffer = RayBuffer_Allocate();
	RayBuffer_Initialize(members->rBuffer, envBuffer->windowWidth, envBuffer->windowHeight);	
}

///
//Frees memory allocated by the PixelProjectionRenderPipeline's members
//
//Parameters:
//	mems: A pointer to the PixelProjectionRenderPipeline_Members to free
static void PixelProjectionRenderPipeline_FreeMembers(RenderPipeline_Members* mems)
{
	PixelProjectionRenderPipeline_Members* members = (PixelProjectionRenderPipeline_Members*)mems;
	RayBuffer_Free(members->rBuffer);
	free(members);
}

///
//Renders a set of gameobjects using a specified render pipeline
//
//Parameters:
//	Pipeline: A pointer to the PixelProjectionRenderPipeline to render with.
//	buffer: A pointer to the RenderingBuffer to render with
//	gameObjs: A pointer to a linked list of GObjects to render
static void PixelProjectionRenderPipeline_Render(RenderPipeline* pipeline, struct RenderingBuffer* buffer, LinkedList* gameObjs)
{
	PixelProjectionRenderPipeline_Members* members = (PixelProjectionRenderPipeline_Members*)pipeline->members;
	EnvironmentBuffer* eBuffer = EnvironmentManager_GetEnvironmentBuffer();

	//Clear the final gBuffer texture
	RayBuffer_ClearTexture(members->rBuffer, RayBuffer_TextureType_FINAL);

	//Bind gBuffer's frame buffer object for writing
	RayBuffer_BindForGeometryPass(members->rBuffer);


	//Perform geometry pass
	pipeline->programs[0]->Render(pipeline->programs[0], buffer, gameObjs);

	RayBuffer_BindForShadowPass(members->rBuffer);
	pipeline->programs[1]->Render(pipeline->programs[1], buffer, gameObjs);

	RayBuffer_BindForLightPass(members->rBuffer);
	//Perform directional light rendering
	pipeline->programs[2]->Render(pipeline->programs[2], buffer, members->rBuffer);

	
	
	RayBuffer_BindForFinalPass(members->rBuffer);
	glBlitFramebuffer
	(
		0, 0, eBuffer->windowWidth, eBuffer->windowHeight,
		0, 0, eBuffer->windowWidth, eBuffer->windowHeight,
		GL_COLOR_BUFFER_BIT, GL_LINEAR
	);
	

	/*	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, members->rBuffer->fbo);
	glReadBuffer(GL_COLOR_ATTACHMENT0 + RayBuffer_TextureType_SHADOW);
	glBlitFramebuffer
	(
		0, 0, eBuffer->windowWidth, eBuffer->windowHeight,
		0, 0, eBuffer->windowWidth, eBuffer->windowHeight,
		GL_COLOR_BUFFER_BIT, GL_LINEAR
	);
	*/
	
	
	glFlush();

}
