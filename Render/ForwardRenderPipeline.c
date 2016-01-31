#include "ForwardRenderPipeline.h"
#include "ForwardShaderProgram.h"
#include "../Data/LinkedList.h"

///
//Static declarations
///

///
//Renders a set of gameobjects using a specified RenderPipeline
//
//Parameters:
//	pipeline: A pointer to The RenderPipeline to render with
//	buffer: A pointer to the RenderingBuffer to render with
//	gameObjs: A pointer to a linked list of GObjects to render
static void ForwardRenderPipeline_Render(RenderPipeline* pipeline,struct RenderingBuffer* buffer, LinkedList* gameObjs);


///
//Function Definitions
///

///
//Intializes a new Forward RenderPipeline
//
//Parameters:
//	pipeline: A pointer to the RenderPipeline to initialize as a ForwardRenderPipeline
void ForwardRenderPipeline_Initialize(RenderPipeline* pipeline)
{
	RenderPipeline_Initialize(pipeline, 1);
	pipeline->programs[0] = ShaderProgram_Allocate();
	ForwardShaderProgram_Initialize(pipeline->programs[0]);

	pipeline->Render = (RenderPipeline_RenderFunc)ForwardRenderPipeline_Render;
}

///
//Renders a set of gameobjects using a specified RenderPipeline
//
//Parameters:
//	pipeline: A pointer to The RenderPipeline to render with
//	buffer: A pointer to the rendering buffer with which to render
//	gameObjs: A pointer to a linked list of GObjects to render
static void ForwardRenderPipeline_Render(RenderPipeline* pipeline, struct RenderingBuffer* buffer, LinkedList* gameObjs)
{
		pipeline->programs[0]->Render(pipeline->programs[0], buffer, gameObjs); 
}
