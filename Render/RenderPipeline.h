#ifndef RENDERPIPELINE_H
#define RENDERPIPELINE_H

#include "ShaderProgram.h"

typedef void* RenderPipeline_Members;

typedef struct RenderPipeline
{
	ShaderProgram** programs;
	RenderPipeline_Members* members;

	int numShaderPrograms;

	void (*FreeMembers)(RenderPipeline_Members*);
	void (*Render)(struct RenderPipeline*, struct RenderingBuffer*, void*);
} RenderPipeline;

typedef void(*RenderPipeline_RenderFunc)(struct RenderPipeline*, struct RenderingBuffer*, void*);

///
//Allocates a new RenderPipeline
//
//Returns:
//	A pointer to a newly allocated RenderPipeline
RenderPipeline* RenderPipeline_Allocate(void);

///
//Initializes a renderPipeline
//
//Parameters:
//	pipeline: A pointer to the renderpipeline to initialize
//	numShaderPrograms: The number of shader programs to initialize for this 
void RenderPipeline_Initialize(RenderPipeline* pipeline, int numShaderPrograms);

///
//Frees resources allocated by a RenderPipeline
//
//Parameters:
//	pipeline: A pointer to the pipeline to free
void RenderPipeline_Free(RenderPipeline* pipeline);

#endif
