#ifndef RENDERINGMANAGER_H
#define RENDERINGMANAGER_H

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ObjectManager.h"

#include "../Render/RenderPipeline.h"
#include "../Render/Camera.h"
#include "../Render/DirectionalLight.h"

#include "../GObject/GObject.h"

#include "../Data/LinkedList.h"

enum RenderingManager_Pipeline
{
	RenderingManager_Pipeline_FORWARD,
	RenderingManager_Pipeline_DEFERRED,
	RenderingManager_Pipeline_RAYTRACER,
	RenderingManager_Pipeline_NUMPIPELINES
};

typedef struct RenderingBuffer
{
	RenderPipeline* renderPipelines[RenderingManager_Pipeline_NUMPIPELINES];
	Camera* camera;
	Vector* directionalLightVector;
	DirectionalLight* directionalLight;
	unsigned char debugOctTree;
} RenderingBuffer;

//Internals
extern RenderingBuffer* renderingBuffer;

//Functions

///
//Initialize the Rendering Manager
void RenderingManager_Initialize(void);

///
//Frees resources taken up by the RenderingManager
void RenderingManager_Free(void);

///
//Assigns the attributes of the active camera to the necessary shader program uniforms
void RenderingManager_SetCameraUniforms(void);

///
//Renders a list of gameobjects using the forward rendering pipeline as their individual meshs.
//Skips a gameobject if it does not have a mesh
//
//Parameters:
//	gameObjectsO: Game object to render
void RenderingManager_Render(LinkedList* gameObjects);

///
//Renders a memory pool of objects with the active rendering pipeline
//Skips gameobjects which are uninitialized or do not have a mesh
//
//Parameters:
//	memoryPool: A pointer to the memory pool of game objects to render
void RenderingManager_RenderWithMemoryPool(MemoryPool* memoryPool);

///
//Renders a list of gameobjects using the Deferred Rendering pipeline
//as their individual meshes. Skips any gameobject which does not have
//a mesh.
//
//Parameters:
//	gameObjects: A pointer to a linked list containing the gameobjects to render.
void RenderingManager_RenderDeferred(LinkedList* gameObjects);

///
//Renders the OctTree
//
//Parameters:
//	nodeToRender: THe node of the oct tree being rendered
//	modelViewProjectionMatrix: THe modelViewProjectionMatrix to send to the shader
//	viewMatrix: THe view matrix of the camera
//	projectionMatrix: The projection matrix of the camera
//	mesh: The mesh to draw as a representation of the oct tree
void RenderingManager_RenderOctTree(struct OctTree_Node* nodeToRender, Matrix* modelViewProjectionMatrix, Matrix* viewMatrix, Matrix* projectionMatrix, Mesh* mesh);


///
//Gets the Rendering Manager's internal Rendering Buffer
//
//Returns:
//	RenderingManager's internal Rendering Buffer
RenderingBuffer* RenderingManager_GetRenderingBuffer();
#endif
