#if defined __linux__

//Enable POSIX definitions for timespec
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <time.h>

#endif

#include "Shoot.h"

#include "../Manager/InputManager.h"
#include "../Manager/RenderingManager.h"
#include "../Manager/TimeManager.h"
#include "../Manager/AssetManager.h"

#include "Remove.h"

struct State_Shoot_Members
{
	float fireRate;
	float fireTimer;
	float ttl;
};

///
//Initializeis a Shoot state
//
//Parameters:
//	s: A pointer to the state to initialize
//	shootSpeed: The fire rate
//	ttl: The time the bullets stick around before getting deleted
void State_Shoot_Initialize(State* s, float shootSpeed, float ttl, unsigned int objID)
{
	struct State_Shoot_Members* members = malloc(sizeof(struct State_Shoot_Members));
	(void)objID;
	s->members = members;
	members->fireRate = shootSpeed;
	members->fireTimer = shootSpeed;
	members->ttl = ttl;
	
	s->State_Update = State_Shoot_Update;
	s->State_Members_Free = State_Shoot_Free;
}

///
//Updates a GObject using a Shoot state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_Shoot_Update(GObject* obj, State* state)
{
	(void)obj;
	struct State_Shoot_Members* members = state->members;

	if(members->fireTimer >= members->fireRate)
	{
		if(InputManager_GetInputBuffer().mouseLock && InputManager_IsMouseButtonPressed(0))
		{
			//Start fire rate cooldown
			members->fireTimer = 0.0f;
			
			Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

			unsigned int bulletID = GObject_Request();
			GObject* bullet = ObjectManager_LookupObject(bulletID);
			GObject_Initialize(bullet);

			Vector pos;
			Vector_INIT_ON_STACK(pos, 3);
			Matrix_SliceColumn(&pos, cam->translationMatrix, 3, 0, 3);
			Vector_Scale(&pos, -1.0f);

			GObject_SetPosition(bullet, &pos);

			Vector direction;
			Vector_INIT_ON_STACK(direction, 3);

			//Get "forward" Vector
			Matrix_SliceRow(&direction, cam->rotationMatrix, 2, 0, 3);
			Vector_Scale(&direction,-1.0f);

			bullet->mesh = AssetManager_LookupMesh("Cube");
			bullet->materialID = Material_Allocate();
			GLuint texID = AssetManager_LookupTexture("White");
			Material_Initialize(bullet->materialID, texID);

			Material* material = MemoryPool_RequestAddress(assetBuffer->materialPool, bullet->materialID);
			material->specularPower = 20.0f;
			material->ambientCoefficient = 0.2f;
			material->diffuseCoefficient = 0.50f;
			material->localCoefficient = 1.0f;
			material->reflectedCoefficient = 0.95f;
			material->transmittedCoefficient = 0.0f;

			bullet->collider = Collider_Allocate();
			ConvexHullCollider_Initialize(bullet->collider);
			ConvexHullCollider_MakeRectangularCollider(bullet->collider->data->convexHullData, 2.0f, 2.0f, 2.0f);
			
			bullet->body = RigidBody_Allocate();
			RigidBody_Initialize(bullet->body, bullet->frameOfReference, 1.0f);
			bullet->body->coefficientOfRestitution = 0.5f;
			bullet->body->staticFriction = 1.0f;
			bullet->body->dynamicFriction = 0.85f;

			bullet->body->rollingResistance = 0.6f;

			/*
			//Construct a rotation matrix to orient bullet
			Matrix rot;
			Matrix_INIT_ON_STACK(rot, 3, 3);
			//Grab 4,4 minor to get 3x3 rotation matrix of camera
			Matrix_GetMinor(&rot, cam->rotationMatrix, 3, 3);
			//Transpose it to get correct direction
			Matrix_Transpose(&rot);
			//Rotate the bullet
			Matrix_TransformMatrix(&rot, bullet->frameOfReference->rotation);
			Matrix_TransformMatrix(&rot, bullet->body->frame->rotation);
			*/
			//Apply impulse to the bullet
			Vector_Scale(&direction, 25.0f);
			RigidBody_ApplyImpulse(bullet->body, &direction, &Vector_ZERO);

			//Add remove state
			State* state = State_Allocate();
			State_Remove_Initialize(state, members->ttl, bulletID);
			GObject_AddState(bullet, state);

			ObjectManager_RegisterObject(bulletID);
		}
	}
	else
	{
		float dt = TimeManager_GetDeltaSec();
		members->fireTimer += dt;
	}
}

///
//Frees memory allocated by a Shoot state
//
//Parameters:
//	state: The shoot state to free
void State_Shoot_Free(State* state)
{
	free(state->members);
}
