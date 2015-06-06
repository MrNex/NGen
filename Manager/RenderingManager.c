#include "RenderingManager.h"

#include <stdio.h>

#include "AssetManager.h"

///
//Internals
RenderingBuffer* renderingBuffer;

///
//Static Declarations

///
//Allocates memory for a rendering buffer
//
//Returns:
//	Pointer to a newly allocated rendering buffer
static RenderingBuffer* RenderingManager_AllocateBuffer(void);

///
//Initializes a rendering buffer
//
//Parameters:
//      buffer: Rendering buffer to initialize
static void RenderingManager_InitializeBuffer(RenderingBuffer* buffer);

///
//Frees the memory consumed by a Rendering Buffer
//
//Parameters:
//      buffer: The buffer to free
static void RenderingManager_FreeBuffer(RenderingBuffer* buffer);

///
//External functions

///
//Initialize the Rendering Manager
void RenderingManager_Initialize(void)
{
	//Enable Rendering Tests
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);


	renderingBuffer = RenderingManager_AllocateBuffer();
	RenderingManager_InitializeBuffer(renderingBuffer);

	if (renderingBuffer->shaderPrograms[0]->shaderProgramID != 0)
	{

		glUseProgram(renderingBuffer->shaderPrograms[0]->shaderProgramID);
	}
	else
	{
		printf("\nError Creating Shader Program!\nShader Results:\nV: %d\tF: %d\tP: %d\n",
			renderingBuffer->shaderPrograms[0]->vertexShaderID,
			renderingBuffer->shaderPrograms[0]->fragmentShaderID,
			renderingBuffer->shaderPrograms[0]->shaderProgramID);
	}

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glPointSize(2.0f);
	glLineWidth(2.0f);
}

///
//Frees resources taken up by the RenderingManager
void RenderingManager_Free(void)
{
	RenderingManager_FreeBuffer(renderingBuffer);
}

///
//Gets the Rendering Manager's internal Rendering Buffer
//
//Returns:
//	RenderingManager's internal Rendering Buffer
RenderingBuffer* RenderingManager_GetRenderingBuffer()
{
	return renderingBuffer;
}


///
//Renders a gameobject as it's mesh.
//
//Parameters:
//	GO: Game object to render
void RenderingManager_Render(LinkedList* gameObjects)
{

	//Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	//Set directional light
	glProgramUniform3fv(renderingBuffer->shaderPrograms[0]->shaderProgramID, renderingBuffer->shaderPrograms[0]->directionalLightVectorLocation, 1, renderingBuffer->directionalLightVector->components);


	Matrix modelMatrix;
	Matrix_INIT_ON_STACK(modelMatrix, 4, 4);
	Matrix viewMatrix;
	Matrix_INIT_ON_STACK(viewMatrix, 4, 4);
	Matrix modelViewProjectionMatrix;
	Matrix_INIT_ON_STACK(modelViewProjectionMatrix, 4, 4);

	//Turn camera's frame of reference into view matrix
	Camera_ToMatrix4(renderingBuffer->camera, &viewMatrix);
	//Set viewMatrix uniform
	glProgramUniformMatrix4fv(renderingBuffer->shaderPrograms[0]->shaderProgramID, renderingBuffer->shaderPrograms[0]->viewMatrixLocation, 1, GL_TRUE, viewMatrix.components);

	//Set projectionMatrix Uniform
	glProgramUniformMatrix4fv(renderingBuffer->shaderPrograms[0]->shaderProgramID, renderingBuffer->shaderPrograms[0]->projectionMatrixLocation, 1, GL_TRUE, renderingBuffer->camera->projectionMatrix->components);


	struct LinkedList_Node* current = gameObjects->head;

	while (current != NULL)
	{
		GObject* gameObj = (GObject*)(current->data);
		//Render gameobject's mesh if it exists
		if (gameObj->mesh != NULL)
		{
			//Set color matrix
			glProgramUniformMatrix4fv(renderingBuffer->shaderPrograms[0]->shaderProgramID, renderingBuffer->shaderPrograms[0]->colorMatrixLocation, 1, GL_TRUE, gameObj->colorMatrix->components);

			//Set modelMatrix uniform
			FrameOfReference_ToMatrix4(gameObj->frameOfReference, &modelMatrix);
			glProgramUniformMatrix4fv(renderingBuffer->shaderPrograms[0]->shaderProgramID, renderingBuffer->shaderPrograms[0]->modelMatrixLocation, 1, GL_TRUE, modelMatrix.components);

			//Construct modelViewProjectionMatrix
			Matrix_Copy(&modelViewProjectionMatrix, &modelMatrix);
			Matrix_TransformMatrix(&viewMatrix, &modelViewProjectionMatrix);
			Matrix_TransformMatrix(renderingBuffer->camera->projectionMatrix, &modelViewProjectionMatrix);
			//Set modelViewProjectionMatrix uniform
			glProgramUniformMatrix4fv(renderingBuffer->shaderPrograms[0]->shaderProgramID, renderingBuffer->shaderPrograms[0]->modelViewProjectionMatrixLocation, 1, GL_TRUE, modelViewProjectionMatrix.components);

			if (gameObj->texture != NULL)
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, gameObj->texture->textureID);

				//Send texture to uniform
				glUniform1i(renderingBuffer->shaderPrograms[0]->textureLocation, 0);
			}
			else
			{
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, AssetManager_LookupTexture("Test")->textureID);

				//Send texture to uniform
				glUniform1i(renderingBuffer->shaderPrograms[0]->textureLocation, 0);
			}


			//Setup GPU program to draw this mesh
			Mesh_Render(gameObj->mesh, gameObj->mesh->primitive);
		}

		//Render gameObject's collider if it exists & in debug mode
		if(gameObj->collider != NULL && gameObj->collider->debug)
		{
			//Set color matrix
			glProgramUniformMatrix4fv(renderingBuffer->shaderPrograms[0]->shaderProgramID, renderingBuffer->shaderPrograms[0]->colorMatrixLocation, 1, GL_TRUE, gameObj->collider->colorMatrix->components);

			//Create modelMatrix from correct Frame Of Reference
			if(gameObj->body != NULL)
			{
				FrameOfReference_ToMatrix4(gameObj->body->frame, &modelMatrix);
			}
			else
			{
				FrameOfReference_ToMatrix4(gameObj->frameOfReference, &modelMatrix);
			}

			//If the object has an AABB collider, take into account the offset
			if(gameObj->collider->type == COLLIDER_AABB)
			{
				struct ColliderData_AABB* AABB = gameObj->collider->data->AABBData;
				*Matrix_Index(&modelMatrix, 0, 3) += AABB->centroid->components[0];
				*Matrix_Index(&modelMatrix, 1, 3) += AABB->centroid->components[1];
				*Matrix_Index(&modelMatrix, 2, 3) += AABB->centroid->components[2];
			}

			//Set the modelMatrix
			glProgramUniformMatrix4fv(renderingBuffer->shaderPrograms[0]->shaderProgramID, renderingBuffer->shaderPrograms[0]->modelMatrixLocation, 1, GL_TRUE, modelMatrix.components);


			//Construct modelViewProjectionMatrix
			Matrix_Copy(&modelViewProjectionMatrix, &modelMatrix);
			Matrix_TransformMatrix(&viewMatrix, &modelViewProjectionMatrix);
			Matrix_TransformMatrix(renderingBuffer->camera->projectionMatrix, &modelViewProjectionMatrix);
			//Set modelViewProjectionMatrix uniform
			glProgramUniformMatrix4fv(renderingBuffer->shaderPrograms[0]->shaderProgramID, renderingBuffer->shaderPrograms[0]->modelViewProjectionMatrixLocation, 1, GL_TRUE, modelViewProjectionMatrix.components);

			//Bind white texture
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, AssetManager_LookupTexture("White")->textureID);

			//Send texture to uniform
			glUniform1i(renderingBuffer->shaderPrograms[0]->textureLocation, 0);


			//Setup GPU program to draw this mesh
			Mesh_Render(gameObj->collider->representation, GL_LINES);

			//TODO: Remove
			//Change the color of colliders to green until they collide
			*Matrix_Index(gameObj->collider->colorMatrix, 0, 0) = 0.0f;
			*Matrix_Index(gameObj->collider->colorMatrix, 1, 1) = 1.0f;
			*Matrix_Index(gameObj->collider->colorMatrix, 2, 2) = 0.0f;
		}

		current = current->next;
	}

	//Render the oct tree
	if(renderingBuffer->debugOctTree)
	{
		//Set the color matrix
		Matrix octTreeColor;
		Matrix_INIT_ON_STACK(octTreeColor, 4, 4);

		*Matrix_Index(&octTreeColor, 0, 0) = 0.0f;
		*Matrix_Index(&octTreeColor, 1, 1) = 1.0f;
		*Matrix_Index(&octTreeColor, 2, 2) = 0.0f;

		glProgramUniformMatrix4fv(renderingBuffer->shaderPrograms[0]->shaderProgramID, renderingBuffer->shaderPrograms[0]->colorMatrixLocation, 1, GL_TRUE, octTreeColor.components);


		Mesh* cube = AssetManager_LookupMesh("CubeWire");
		Texture* white = AssetManager_LookupTexture("White");

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, white->textureID);

		//Send texture to uniform
		glUniform1i(renderingBuffer->shaderPrograms[0]->textureLocation, 0);

		RenderingManager_RenderOctTree(ObjectManager_GetObjectBuffer().octTree->root, &modelViewProjectionMatrix, &viewMatrix, renderingBuffer->camera->projectionMatrix, cube);
	}
	//Start drawing threads on gpu
	glFlush();
}

///
//Renders the OctTree
//
//Parameters:
//	nodeToRender: THe node of the oct tree being rendered
//	modelViewProjectionMatrix: THe modelViewProjectionMatrix to send to the shader
//	viewMatrix: THe view matrix of the camera
//	projectionMatrix: The projection matrix of the camera
//	mesh: The mesh to draw as a representation of the oct tree
void RenderingManager_RenderOctTree(struct OctTree_Node* nodeToRender, Matrix* modelViewProjectionMatrix, Matrix* viewMatrix, Matrix* projectionMatrix, Mesh* mesh)
{
	//Only render trees which contain objects?

	//If this node has children, draw them too
	if(nodeToRender->children != NULL)
	{
		for(int i = 0; i < 8; i++)
		{
			RenderingManager_RenderOctTree(nodeToRender->children + i, modelViewProjectionMatrix, viewMatrix, projectionMatrix, mesh);
		}
	}
	else
	{
		//Set modelViewMatrix to identity
		Matrix_ToIdentity(modelViewProjectionMatrix);

		//Set the scale values of the modelViewProjection Matrix
		*Matrix_Index(modelViewProjectionMatrix, 0, 0) = nodeToRender->right;
		*Matrix_Index(modelViewProjectionMatrix, 1, 1) = nodeToRender->top;
		*Matrix_Index(modelViewProjectionMatrix, 2, 2) = nodeToRender->front;

		//Set modelMatrix uniform
		glProgramUniformMatrix4fv(renderingBuffer->shaderPrograms[0]->shaderProgramID, renderingBuffer->shaderPrograms[0]->modelMatrixLocation, 1, GL_TRUE, modelViewProjectionMatrix->components);

		//Construct modelViewProjection
		Matrix_TransformMatrix(viewMatrix, modelViewProjectionMatrix);
		Matrix_TransformMatrix(projectionMatrix, modelViewProjectionMatrix);
		//Set modelViewProjectionMatrix uniform
		glProgramUniformMatrix4fv(renderingBuffer->shaderPrograms[0]->shaderProgramID, renderingBuffer->shaderPrograms[0]->modelViewProjectionMatrixLocation, 1, GL_TRUE, modelViewProjectionMatrix->components);

		//Render node
		Mesh_Render(mesh, GL_LINES);
	}
}

///
//Allocates memory for a rendering buffer
//
//Returns:
//	Pointer to a newly allocated rendering buffer
static RenderingBuffer* RenderingManager_AllocateBuffer(void)
{
	RenderingBuffer* buffer = (RenderingBuffer*)malloc(sizeof(RenderingBuffer));
	return buffer;
}

///
//Initializes a rendering buffer
//
//Parameters:
//      buffer: Rendering buffer to initialize
static void RenderingManager_InitializeBuffer(RenderingBuffer* buffer)
{
        //Shaders
        buffer->shaderPrograms = (ShaderProgram**)malloc(sizeof(ShaderProgram*));

        buffer->shaderPrograms[0] = ShaderProgram_Allocate();
        ShaderProgram_Initialize(buffer->shaderPrograms[0], "./Shader/VertexShader.glsl", "./Shader/FragmentShader.glsl");


        //Checking shaders
        if (buffer->shaderPrograms[0]->shaderProgramID == 0)
        {
                printf("\nError Creating Shader Program!\nShader Results:\nV: %d\tF: %d\tP: %d\n",
                        buffer->shaderPrograms[0]->vertexShaderID,
                        buffer->shaderPrograms[0]->fragmentShaderID,
                        buffer->shaderPrograms[0]->shaderProgramID);

        }


        //Camera
        buffer->camera = Camera_Allocate();
        Camera_Initialize(buffer->camera);

        //Lighting
        buffer->directionalLightVector = Vector_Allocate();
        Vector_Initialize(buffer->directionalLightVector, 3);
        //buffer->directionalLightVector->components[0] = -0.77f;
        //buffer->directionalLightVector->components[2] = -0.77f;
        buffer->directionalLightVector->components[2] = -1.0f;

        //Debug
        buffer->debugOctTree = 0;
}

///
//Frees the memory consumed by a Rendering Buffer
//
//Parameters:
//      buffer: The buffer to free
static void RenderingManager_FreeBuffer(RenderingBuffer* buffer)
{
        ShaderProgram_Free(buffer->shaderPrograms[0]);
        free(buffer->shaderPrograms);
        Camera_Free(buffer->camera);
        Vector_Free(buffer->directionalLightVector);
}
