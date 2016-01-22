#include "DeferredRenderPipeline.h"

#include "../Manager/EnvironmentManager.h"

#include "DeferredGeometryShaderProgram.h"
#include "../Data/LinkedList.h"

#include "GeometryBuffer.h"

typedef struct DeferredRenderPipeline_Members
{
	GeometryBuffer* gBuffer;
} DeferredRenderPipeline_Members;

///
//Static Declarations
///

///
//Allocates a new set of members for the DeferredRenderPipeline
//
//Returns:
//	A pointer to an uninitialized DeferredRenderPipeline_Members
static RenderPipeline_Members* DeferredRenderPipeline_AllocateMembers(void);

///
//Initializes a set of DeferredRenderPipeline members
//
//Parameters:
//	mems: A pointer to the set of DeferredRenderPipeline members to
static void DeferredRenderPipeline_InitializeMembers(RenderPipeline_Members* mems);

///
//Frees memory allocated by the DeferredRenderPipeline's members
//
//Parameters:
//	mems: A pointer to the DeferredRenderPipeline_Members to free
static void DeferredRenderPipeline_FreeMembers(RenderPipeline_Members* mems);

///
//Renders a set of gameobjects using a specified render pipeline
//
//Parameters:
//	Pipeline: A pointer to the DeferredRenderPipeline to render with.
//	buffer: A pointer to the RenderingBuffer to render with
//	gameObjs: A pointer to a linked list of GObjects to render
static void DeferredRenderPipeline_Render(RenderPipeline* pipeline, struct RenderingBuffer* buffer, LinkedList* gameObjs);

///
//Function Definitions
///

///
//Intializes a new DeferredRenderPipeline
//
//Parameters:
//	pipeline: A pointer to the RenderPipeline to initialize as a DeferredRenderPipeline
void DeferredRenderPipeline_Initialize(RenderPipeline* pipeline)
{
	RenderPipeline_Initialize(pipeline, 1);

	pipeline->members = DeferredRenderPipeline_AllocateMembers();
	DeferredRenderPipeline_InitializeMembers(pipeline->members);

	pipeline->programs[0] = ShaderProgram_Allocate();
	DeferredGeometryShaderProgram_Initialize(pipeline->programs[0]);

	pipeline->Render = DeferredRenderPipeline_Render;
	pipeline->FreeMembers = DeferredRenderPipeline_FreeMembers;
}

///
//Allocates a new set of members for the DeferredRenderPipeline
//
//Returns:
//	A pointer to an uninitialized DeferredRenderPipeline_Members
static RenderPipeline_Members* DeferredRenderPipeline_AllocateMembers(void)
{
	return malloc(sizeof(DeferredRenderPipeline_Members));
}

///
//Initializes a set of DeferredRenderPipeline members
//
//Parameters:
//	mems: A pointer to the set of DeferredRenderPipeline members to initialize
static void DeferredRenderPipeline_InitializeMembers(RenderPipeline_Members* mems)
{
	DeferredRenderPipeline_Members* members = (DeferredRenderPipeline_Members*)mems;
	EnvironmentBuffer* envBuffer = EnvironmentManager_GetEnvironmentBuffer();
	members->gBuffer = GeometryBuffer_Allocate();
	GeometryBuffer_Initialize(members->gBuffer, envBuffer->windowWidth, envBuffer->windowHeight);	
}

///
//Frees memory allocated by the DeferredRenderPipeline's members
//
//Parameters:
//	mems: A pointer to the DeferredRenderPipeline_Members to free
static void DeferredRenderPipeline_FreeMembers(RenderPipeline_Members* mems)
{
	DeferredRenderPipeline_Members* members = (DeferredRenderPipeline_Members*)mems;
	GeometryBuffer_Free(members->gBuffer);
	free(members);
}

///
//Renders a set of gameobjects using a specified render pipeline
//
//Parameters:
//	Pipeline: A pointer to the DeferredRenderPipeline to render with.
//	buffer: A pointer to the RenderingBuffer to render with
//	gameObjs: A pointer to a linked list of GObjects to render
static void DeferredRenderPipeline_Render(RenderPipeline* pipeline, struct RenderingBuffer* buffer, LinkedList* gameObjs)
{
	DeferredRenderPipeline_Members* members = (DeferredRenderPipeline_Members*)pipeline->members;

	//Bind gBuffer's frame buffer object for writing
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, members->gBuffer->fbo);
	//Render
	pipeline->programs[0]->Render(pipeline->programs[0], buffer, gameObjs);


	//Temp{ 
	
	EnvironmentBuffer* envBuffer = EnvironmentManager_GetEnvironmentBuffer();
	int halfWidth = envBuffer->windowWidth / 2;
	int halfHeight = envBuffer->windowHeight / 2;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Bind gBuffers fbo forreading
	glBindFramebuffer(GL_READ_FRAMEBUFFER, members->gBuffer->fbo);
	
	//Blit gbuffer textures to screen
	//Position texture
	glReadBuffer(GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_POSITION);
	glBlitFramebuffer(0, 0, members->gBuffer->textureWidth, members->gBuffer->textureHeight,
			0, 0, halfWidth, halfHeight, 
			GL_COLOR_BUFFER_BIT, GL_LINEAR);
	
	//Diffuse texture
	glReadBuffer(GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_DIFFUSE);
	glBlitFramebuffer(0, 0, members->gBuffer->textureWidth, members->gBuffer->textureHeight,
			0, halfHeight, halfWidth, envBuffer->windowHeight, 
			GL_COLOR_BUFFER_BIT, GL_LINEAR);

	//Normal texture
	glReadBuffer(GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_NORMAL);
	glBlitFramebuffer(0, 0, members->gBuffer->textureWidth, members->gBuffer->textureHeight,
			halfWidth, halfHeight, envBuffer->windowWidth, envBuffer->windowHeight,
			GL_COLOR_BUFFER_BIT, GL_LINEAR);


	//TextureCoordinate texture
	glReadBuffer(GL_COLOR_ATTACHMENT0 + GeometryBuffer_TextureType_TEXTURECOORDINATE);
	glBlitFramebuffer(0, 0, members->gBuffer->textureWidth, members->gBuffer->textureHeight,
			halfWidth, 0, envBuffer->windowWidth, halfHeight,
			GL_COLOR_BUFFER_BIT, GL_LINEAR);


	glFlush();
}
