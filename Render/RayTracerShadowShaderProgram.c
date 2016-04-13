#include "RayTracerShadowShaderProgram.h"

#include <float.h>

#include "../Collision/Collider.h"
#include "../Manager/CollisionManager.h"

///
//Computes the shadow texture for the RayTracerRenderPipeline
//
//Parameters:
//	shadowDest: A pointer to an array of bytes in which to store the shadow texture
//	buffer: A pointer to the active rendering buffer
//	positionData: A pointer to the memory containing the position of the surfaces at every pixel on the screen
//	textureWidth: The width of the shadow and position textures
//	textureHeight: The height of the shadow and position textures
//	gObjects: A pointer to a linked list containing the GObjects in the scene
void RayTracerShadowShaderProgram_ComputeShadowTexture(unsigned char* shadowDest, struct RenderingBuffer* buffer, float* positionData, int textureWidth, int textureHeight, LinkedList* gObjects)
{
	int pixels = textureWidth * textureHeight;

	struct ColliderData_Ray ray;
	Vector direction;
	Vector_INIT_ON_STACK(direction, 3);

	Vector position;
	position.dimension = 3;

	ray.direction = &direction;
	ray.position = &position;

	Vector_Copy(ray.direction, buffer->directionalLight->direction);
	Vector_Scale(ray.direction, -1.0f);
	Vector_Normalize(ray.direction);

	struct LinkedList_Node* current = gObjects->head;
	GObject* obj = NULL;
	while(current != NULL)
	{
		obj = current->data;

		for(int i = 0; i < pixels; i++)
		{

			position.components = positionData + i * 3;
			

			if(shadowDest[i] == 0 && Vector_GetMagSq(&position) > FLT_EPSILON)
			{
				if(obj->collider != NULL)
				{
					//printf("Setting\n");
					shadowDest[i] = CollisionManager_RayCastGObject(&ray, obj);
				}
			}

		}

		current = current->next;
	}


}
