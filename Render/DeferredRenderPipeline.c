#include "DeferredRenderPipeline.h"

#include "../Manager/EnvironmentManager.h"

#include "DeferredGeometryShaderProgram.h"
#include "DeferredDirectionalShaderProgram.h"
#include "DeferredPointShaderProgram.h"
#include "DeferredStencilShaderProgram.h"

#include "../Data/LinkedList.h"

#include "../GObject/GObject.h"

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
	RenderPipeline_Initialize(pipeline, 4);

	pipeline->members = DeferredRenderPipeline_AllocateMembers();
	DeferredRenderPipeline_InitializeMembers(pipeline->members);

	pipeline->programs[0] = ShaderProgram_Allocate();
	DeferredGeometryShaderProgram_Initialize(pipeline->programs[0]);

	pipeline->programs[1] = ShaderProgram_Allocate();
	DeferredDirectionalShaderProgram_Initialize(pipeline->programs[1]);

	pipeline->programs[2] = ShaderProgram_Allocate();
	DeferredPointShaderProgram_Initialize(pipeline->programs[2]);

	pipeline->programs[3] = ShaderProgram_Allocate();
	DeferredStencilShaderProgram_Initialize(pipeline->programs[3]);

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
	EnvironmentBuffer* eBuffer = EnvironmentManager_GetEnvironmentBuffer();

	//Clear the final gBuffer texture
	GeometryBuffer_ClearFinalTexture(members->gBuffer);

	//Bind gBuffer's frame buffer object for writing
	GeometryBuffer_BindForGeometryPass(members->gBuffer);


	//The geometry pass must update the depth buffer
	//glDepthMask(GL_TRUE);
	//glEnable(GL_DEPTH_TEST);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Perform geometry pass
	pipeline->programs[0]->Render(pipeline->programs[0], buffer, gameObjs);



	GeometryBuffer_BindForLightPass(members->gBuffer);
	//Perform directional light rendering
	pipeline->programs[1]->Render(pipeline->programs[1], buffer, members->gBuffer);

	
	//DeferredPointShaderProgram_SetConstantUniforms(pipeline->programs[2]);


	
	glEnable(GL_STENCIL_TEST);
	
	struct LinkedList_Node* current = gameObjs->head;
	GObject* gameObj = NULL;
	while(current != NULL)
	{
		gameObj = (GObject*)current->data;
		if(gameObj->light != NULL)
		{
			GeometryBuffer_BindForStencilPass(members->gBuffer);
			pipeline->programs[3]->Render(pipeline->programs[3], buffer, gameObj);


			GeometryBuffer_BindForLightPass(members->gBuffer);
			pipeline->programs[2]->Render(pipeline->programs[2], buffer, gameObj);
		}
		current = current->next;
	}

	glDisable(GL_STENCIL_TEST);
	
	//Bind gBuffer FBO to be read from
	//Note: this binds the default FBO for writing and activates/binds textures
	//GeometryBuffer_BindForReading(members->gBuffer);
	//glClear(GL_COLOR_BUFFER_BIT);


	//GeometryBuffer_BindForLightPass(members->gBuffer);
	//Perform directional light rendering
	//pipeline->programs[1]->Render(pipeline->programs[1], buffer, members->gBuffer);

	GeometryBuffer_BindForFinalPass(members->gBuffer);
	glBlitFramebuffer
	(
		0, 0, eBuffer->windowWidth, eBuffer->windowHeight,
		0, 0, eBuffer->windowWidth, eBuffer->windowHeight,
		GL_COLOR_BUFFER_BIT, GL_LINEAR
	);
	
	glFlush();

}
