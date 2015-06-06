#include <GL/glew.h>
#include <GL/freeglut.h>

#include "ObjectManager.h"

#include "../Render/ShaderProgram.h"
#include "../Render/Camera.h"

#include "../GObject/GObject.h"

#include "../Data/LinkedList.h"

typedef struct RenderingBuffer
{
	ShaderProgram** shaderPrograms;
	Camera* camera;
	Vector* directionalLightVector;
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
//Renders a gameobject as it's mesh.
//
//Parameters:
//	GO: Game object to render
void RenderingManager_Render(LinkedList* GameObjects);

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

