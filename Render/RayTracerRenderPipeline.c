#include "RayTracerRenderPipeline.h"

#include <float.h>

#include "../Manager/EnvironmentManager.h"
#include "../Manager/KernelManager.h"

#include "RayTracerGeometryShaderProgram.h"
#include "RayTracerShadowShaderProgram.h"
#include "RayTracerDirectionalShaderProgram.h"
#include "RayTracerPointShaderProgram.h"
#include "RayTracerStencilShaderProgram.h"

#include "../Device/KernelProgram.h"
#include "../Device/RayTracerDirectionalShadowKernelProgram.h"
#include "../Device/RayTracerPointShadowKernelProgram.h"

#include "../Data/LinkedList.h"

#include "../GObject/GObject.h"

#include "RayBuffer.h"

typedef struct RayTracerRenderPipeline_Members
{
	RayBuffer* rBuffer;
	KernelProgram* directionalShadowKernelProg;
	KernelProgram* pointShadowKernelProg;
} RayTracerRenderPipeline_Members;

///
//Static Declarations
///

///
//Allocates a new set of members for the RayTracerRenderPipeline
//
//Returns:
//	A pointer to an uninitialized RayTracerRenderPipeline_Members
static RenderPipeline_Members* RayTracerRenderPipeline_AllocateMembers(void);

///
//Initializes a set of RayTracerRenderPipeline members
//
//Parameters:
//	mems: A pointer to the set of RayTracerRenderPipeline members to
static void RayTracerRenderPipeline_InitializeMembers(RenderPipeline_Members* mems);

///
//Frees memory allocated by the RayTracerRenderPipeline's members
//
//Parameters:
//	mems: A pointer to the RayTracerRenderPipeline_Members to free
static void RayTracerRenderPipeline_FreeMembers(RenderPipeline_Members* mems);

///
//Renders a set of gameobjects using a specified render pipeline
//
//Parameters:
//	Pipeline: A pointer to the RayTracerRenderPipeline to render with.
//	buffer: A pointer to the RenderingBuffer to render with
//	gameObjs: A pointer to a linked list of GObjects to render
static void RayTracerRenderPipeline_Render(RenderPipeline* pipeline, struct RenderingBuffer* buffer, LinkedList* gameObjs);

///
//Function Definitions
///

///
//Intializes a new RayTracerRenderPipeline
//
//Parameters:
//	pipeline: A pointer to the RenderPipeline to initialize as a RayTracerRenderPipeline
void RayTracerRenderPipeline_Initialize(RenderPipeline* pipeline)
{
	RenderPipeline_Initialize(pipeline, 4);

	pipeline->members = RayTracerRenderPipeline_AllocateMembers();
	RayTracerRenderPipeline_InitializeMembers(pipeline->members);

	pipeline->programs[0] = ShaderProgram_Allocate();
	RayTracerGeometryShaderProgram_Initialize(pipeline->programs[0]);

	pipeline->programs[1] = ShaderProgram_Allocate();
	RayTracerDirectionalShaderProgram_Initialize(pipeline->programs[1]);

	pipeline->programs[2] = ShaderProgram_Allocate();
	RayTracerPointShaderProgram_Initialize(pipeline->programs[2]);

	pipeline->programs[3] = ShaderProgram_Allocate();
	RayTracerStencilShaderProgram_Initialize(pipeline->programs[3]);

	pipeline->Render = (RenderPipeline_RenderFunc)RayTracerRenderPipeline_Render;
	pipeline->FreeMembers = RayTracerRenderPipeline_FreeMembers;
}

///
//Allocates a new set of members for the RayTracerRenderPipeline
//
//Returns:
//	A pointer to an uninitialized RayTracerRenderPipeline_Members
static RenderPipeline_Members* RayTracerRenderPipeline_AllocateMembers(void)
{
	return malloc(sizeof(RayTracerRenderPipeline_Members));
}

///
//Initializes a set of RayTracerRenderPipeline members
//
//Parameters:
//	mems: A pointer to the set of RayTracerRenderPipeline members to initialize
static void RayTracerRenderPipeline_InitializeMembers(RenderPipeline_Members* mems)
{
	RayTracerRenderPipeline_Members* members = (RayTracerRenderPipeline_Members*)mems;
	EnvironmentBuffer* envBuffer = EnvironmentManager_GetEnvironmentBuffer();
	KernelBuffer* kBuffer = KernelManager_GetKernelBuffer();
	members->rBuffer = RayBuffer_Allocate();
	RayBuffer_Initialize(members->rBuffer, envBuffer->windowWidth, envBuffer->windowHeight);	
	
	members->directionalShadowKernelProg = KernelProgram_Allocate();
	RayTracerDirectionalShadowKernelProgram_Initialize(members->directionalShadowKernelProg, kBuffer, members->rBuffer);

	members->pointShadowKernelProg = KernelProgram_Allocate();
	RayTracerPointShadowKernelProgram_Initialize(members->pointShadowKernelProg, kBuffer, members->rBuffer);
}

///
//Frees memory allocated by the RayTracerRenderPipeline's members
//
//Parameters:
//	mems: A pointer to the RayTracerRenderPipeline_Members to free
static void RayTracerRenderPipeline_FreeMembers(RenderPipeline_Members* mems)
{
	RayTracerRenderPipeline_Members* members = (RayTracerRenderPipeline_Members*)mems;
	RayBuffer_Free(members->rBuffer);
	KernelProgram_Free(members->directionalShadowKernelProg);
	KernelProgram_Free(members->pointShadowKernelProg);
	free(members);
}


///
//Renders a set of gameobjects using a specified render pipeline
//
//Parameters:
//	Pipeline: A pointer to the RayTracerRenderPipeline to render with.
//	buffer: A pointer to the RenderingBuffer to render with
//	gameObjs: A pointer to a linked list of GObjects to render
static void RayTracerRenderPipeline_Render(RenderPipeline* pipeline, struct RenderingBuffer* buffer, LinkedList* gameObjs)
{
	RayTracerRenderPipeline_Members* members = (RayTracerRenderPipeline_Members*)pipeline->members;
	EnvironmentBuffer* eBuffer = EnvironmentManager_GetEnvironmentBuffer();

	//Clear the final gBuffer texture
	RayBuffer_ClearTexture(members->rBuffer, RayBuffer_TextureType_FINAL);

	//Bind gBuffer's frame buffer object for writing
	RayBuffer_BindForGeometryPass(members->rBuffer);


	//The geometry pass must update the depth buffer
	//glDepthMask(GL_TRUE);
	//glEnable(GL_DEPTH_TEST);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Perform geometry pass
	pipeline->programs[0]->Render(pipeline->programs[0], buffer, gameObjs);

	//Perform shadow pass
	RayBuffer_ClearTexture(members->rBuffer, RayBuffer_TextureType_SHADOW);

	if(buffer->directionalLight->base->diffuseIntensity > FLT_EPSILON)
	{
		RayBuffer_BindForShadowKernel();

		glFinish();
		members->directionalShadowKernelProg->Execute(members->directionalShadowKernelProg, KernelManager_GetKernelBuffer(), members->rBuffer);


		RayBuffer_BindForLightPass(members->rBuffer);
	
		//Perform directional light rendering
		pipeline->programs[1]->Render(pipeline->programs[1], buffer, members->rBuffer);
	}


	
	//Create parameter struct for the PointShadowKernelProgram
	struct RayTracerPointShadowKernelProgram_ExecutionParameters params;
	params.buffer = members->rBuffer;
	params.light = NULL;
	
	glEnable(GL_STENCIL_TEST);
	
	struct LinkedList_Node* current = gameObjs->head;
	GObject* gameObj = NULL;
	while(current != NULL)
	{
		gameObj = (GObject*)current->data;
		if(gameObj->light != NULL)
		{
			params.light = gameObj;

			RayBuffer_ClearTexture(members->rBuffer, RayBuffer_TextureType_SHADOW);
			glFinish();
			RayBuffer_BindForShadowKernel();
			members->pointShadowKernelProg->Execute(members->pointShadowKernelProg, KernelManager_GetKernelBuffer(), &params);

			//Removed because of culling of objects inside of stencil sphere when camera is outside
			//RayBuffer_BindForStencilPass(members->rBuffer);
			//pipeline->programs[3]->Render(pipeline->programs[3], buffer, gameObj);


			RayBuffer_BindForLightPass(members->rBuffer);
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

	RayBuffer_BindForFinalPass(members->rBuffer);
	glBlitFramebuffer
	(
		0, 0, eBuffer->windowWidth, eBuffer->windowHeight,
		0, 0, eBuffer->windowWidth, eBuffer->windowHeight,
		GL_COLOR_BUFFER_BIT, GL_LINEAR
	);
	
	glFlush();

}
