#include "DeferredRenderPipeline.h"

#include "../Manager/EnvironmentManager.h"

#include "DeferredGeometryShaderProgram.h"
#include "DeferredDirectionalShaderProgram.h"

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
	RenderPipeline_Initialize(pipeline, 2);

	pipeline->members = DeferredRenderPipeline_AllocateMembers();
	DeferredRenderPipeline_InitializeMembers(pipeline->members);

	pipeline->programs[0] = ShaderProgram_Allocate();
	DeferredGeometryShaderProgram_Initialize(pipeline->programs[0]);

	pipeline->programs[1] = ShaderProgram_Allocate();
	DeferredDirectionalShaderProgram_Initialize(pipeline->programs[1]);

	pipeline->Render = (RenderPipeline_RenderFunc)DeferredRenderPipeline_Render;
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
	GeometryBuffer_BindForWriting(members->gBuffer);

	//The geometry pass must update the depth buffer
	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Perform geometry pass
	pipeline->programs[0]->Render(pipeline->programs[0], buffer, gameObjs);

	//Now we should not update the depth buffer to perform the lightning pass.
	glDepthMask(GL_FALSE);
	glDisable(GL_DEPTH_TEST);

	//The lighting pass will need to blend the effects of the lighting with that of the geometry pass.
	//glEnable(GL_BLEND);
	//glBlendEquation(GL_FUNC_ADD);	//Will add the two resulting colors for each pixel
	//glBlendFunc(GL_ONE, GL_ONE);	//Scales each by a factor of 1 when adding the geometry and lightning passes.

	//Bind gBuffer FBO to be read from
	//Note: this binds the default FBO for writing and activates/binds textures
	GeometryBuffer_BindForReading(members->gBuffer);

	glClear(GL_COLOR_BUFFER_BIT);
	
	//Perform directional light rendering
	pipeline->programs[1]->Render(pipeline->programs[1], buffer, members->gBuffer);

	//glDisable(GL_BLEND);
	
	glFlush();

	//Disable the blending we enabled for the lighting pass.
	//glDisable(GL_BLEND);
}
