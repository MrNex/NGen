#include "RenderPipeline.h"

///
//Allocates a new RenderPipeline
//
//Returns:
//	A pointer to a newly allocated RenderPipeline
RenderPipeline* RenderPipeline_Allocate(void)
{
	return malloc(sizeof(RenderPipeline));
}

///
//Initializes a renderPipeline
//
//Parameters:
//	pipeline: A pointer to the renderpipeline to initialize
//	numShaderPrograms: The number of shader programs to initialize for this 
void RenderPipeline_Initialize(RenderPipeline* pipeline, int numShaderPrograms)
{
	pipeline->numShaderPrograms = numShaderPrograms;

	pipeline->programs = malloc(sizeof(ShaderProgram*) * numShaderPrograms);
	
	pipeline->members = NULL;
	pipeline->FreeMembers = NULL;
	pipeline->Render = NULL;
}

///
//Frees resources allocated by a RenderPipeline
//
//Parameters:
//	pipeline: A pointer to the pipeline to free
void RenderPipeline_Free(RenderPipeline* pipeline)
{
	for(int i = 0; i < pipeline->numShaderPrograms; ++i)
	{
		ShaderProgram_Free(pipeline->programs[i]);
	}
	free(pipeline->programs);

	if(pipeline->FreeMembers != NULL)
	{
		pipeline->FreeMembers(pipeline->members);
	}	
}
