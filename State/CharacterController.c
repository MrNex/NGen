#if defined __linux__

//Enable POSIX definitions (for timespec)
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif
//#define _POSIX_C_SOURCE 1

#include <time.h>

#endif 


#include "CharacterController.h"

#include "../Manager/InputManager.h"
#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"
#include "../Manager/TimeManager.h"
#include "../Manager/PhysicsManager.h"
#include "../Manager/ObjectManager.h"

#include "Remove.h"

#include <stdio.h>

// The members that affect the object
// rotationSpeed = self explanatory
// movementSpeed = also self explanatory
// maxSpeed = the maximum velocity the object can reach
// coolDown = the cooldown for the bullet fired
// timer = the time needed to pass for the bullet is able to be fired again
struct State_CharacterController_Members
{
	float rotationSpeed;
	float movementSpeed;
	float maxSpeed;
	float coolDown;
	float timer;
};



// Initialize the character state

// [Based off of Camera Initialize]
// Initialize the character State
// Param:
//  s: The state of intiialize
void State_CharacterController_Initialize(State* s, const float velocity, const float angularVelocity, float maxVel, float shootSpeed)
{
	s->members = (State_Members)malloc(sizeof(struct State_CharacterController_Members));
	//Get members
	struct State_CharacterController_Members* members = (struct State_CharacterController_Members*)s->members;

	// set member values to the constructor's 
	members->movementSpeed = velocity;
	members->rotationSpeed = angularVelocity;
	members->maxSpeed = maxVel;
	members->coolDown = shootSpeed;
	members->timer = 0.0f;
	s->State_Update = State_CharacterController_Update;
	s->State_Members_Free = State_CharacterController_Free;
}

// Frees all of the members in the state
void State_CharacterController_Free(State* s)
{
	free(s->members);
}

// Updates the render manager and moves/rotates the object.
// GO: Game Object that the state is attached to
// state: The first person camera State updating the game object.
void State_CharacterController_Update(GObject* GO, State* state)
{
	State_CharacterController_Rotate(GO,state);
	State_CharacterController_Translate(GO,state);
	State_CharacterController_ShootBullet(GO, state);
}

// Rotation for the character controller
// This should need no changes?
void State_CharacterController_Rotate(GObject* GO, State* state)
{
	//GO is unused
	(void)GO;

	// create a camera object
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;
	//Get members
	struct State_CharacterController_Members* members = (struct State_CharacterController_Members*)state->members;


	// if player's mouse is locked
	if(InputManager_GetInputBuffer().mouseLock)
	{
		// Gets the time per second
		float dt = TimeManager_GetDeltaSec();

		int deltaMouseX = (InputManager_GetInputBuffer().mousePosition[0] - InputManager_GetInputBuffer().previousMousePosition[0]);
		int deltaMouseY = (InputManager_GetInputBuffer().mousePosition[1] - InputManager_GetInputBuffer().previousMousePosition[1]);

		Vector* axis = Vector_Allocate();
		Vector_Initialize(axis,3);

		if(deltaMouseX != 0)
		{
			axis->components[1] = 1.0f;
			// rotate the camera
			//Camera_Rotate(cam,axis,state->members->rotationSpeed * deltaMouseX);
			Camera_ChangeYaw(cam, members->rotationSpeed * deltaMouseX * dt);
			axis->components[1] = 0.0f;
		}

		if (deltaMouseY != 0)
		{
			Vector forwardVector;
			Vector_INIT_ON_STACK(forwardVector, 3);
			Matrix_SliceRow(&forwardVector, cam->rotationMatrix, 2, 0, 3);

			// Keep camera from overextending it's boundaries.
			if (deltaMouseY > 0)
			{
				if (Vector_DotProduct(&forwardVector, &Vector_E2) < 0.7f)
				{
					axis->components[0] = 1.0f;
					Camera_ChangePitch(cam, members->rotationSpeed * deltaMouseY * dt);
					axis->components[0] = 0.0f;
				}
			}
			else if (deltaMouseY < 0)
			{
				if (Vector_DotProduct(&forwardVector, &Vector_E2) > -0.7f)
				{
					axis->components[0] = 1.0f;
					Camera_ChangePitch(cam, members->rotationSpeed * deltaMouseY * dt);
					axis->components[0] = 0.0f;
				}
			}


		}


		Vector_Free(axis);

	}
}

// translate the character and his bounding box.
// Parameters:
//   GO: The game object this state is attached to (Used for translating the bounding box)
//   state: The first person camera state updating the gameObject
void State_CharacterController_Translate(GObject* GO, State* state)
{
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;
	//Get members
	struct State_CharacterController_Members* members = (struct State_CharacterController_Members*)state->members;

	if(InputManager_GetInputBuffer().mouseLock)
	{
		Vector netMvmtVec;
		Vector partialMvmtVec;
		Vector_INIT_ON_STACK(netMvmtVec, 3);
		Vector_INIT_ON_STACK(partialMvmtVec, 3);


		if (InputManager_IsKeyDown('w'))
		{
			//Get "back" Vector
			Matrix_SliceRow(&partialMvmtVec, cam->rotationMatrix, 2, 0, 3);
			//Subtract "back" Vector from netMvmtVec
			Vector_Decrement(&netMvmtVec, &partialMvmtVec);
			//Or in one step but less pretty... Faster though. I think I want readable here for now though.
			//Vector_DecrementArray(netMvmtVec.components, Matrix_Index(cam->rotationMatrix, 2, 0), 3);
		}
		if (InputManager_IsKeyDown('s'))
		{
			//Get "back" Vector
			Matrix_SliceRow(&partialMvmtVec, cam->rotationMatrix, 2, 0, 3);
			//Add "back" Vector to netMvmtVec
			Vector_Increment(&netMvmtVec, &partialMvmtVec);
		}
		if (InputManager_IsKeyDown('a'))
		{
			//Get "Right" Vector
			Matrix_SliceRow(&partialMvmtVec, cam->rotationMatrix, 0, 0, 3);
			//Subtract "Right" Vector From netMvmtVec
			Vector_Decrement(&netMvmtVec, &partialMvmtVec);
		}
		if (InputManager_IsKeyDown('d'))
		{
			//Get "Right" Vector
			Matrix_SliceRow(&partialMvmtVec, cam->rotationMatrix, 0, 0, 3);
			//Add "Right" Vector to netMvmtVec
			Vector_Increment(&netMvmtVec, &partialMvmtVec);
		}


		if (Vector_GetMag(&netMvmtVec) > 0.0f)
		{
			// Get the projection and keep player grounded
			Vector perpMvmtVec;
			Vector_INIT_ON_STACK(perpMvmtVec, 3);
			Vector_GetProjection(&perpMvmtVec, &netMvmtVec, &Vector_E2);
			Vector_Decrement(&netMvmtVec, &perpMvmtVec);


			// Normalize vector and scale
			Vector_Normalize(&netMvmtVec);
			Vector_Scale(&netMvmtVec, members->movementSpeed);


			//Apply Impulse
			RigidBody_ApplyImpulse(GO->body, &netMvmtVec, &Vector_ZERO);




		}

	}

	// If vector is going too fast, the maxspeed will keep it from going faster, by scaling it by maxspeed.
	if(Vector_GetMag(GO->body->velocity) >= members->maxSpeed)
	{
		Vector_Normalize(GO->body->velocity);
		Vector_Scale(GO->body->velocity,members->maxSpeed);
	}

	// Set position of Camera to the body
	Camera_SetPosition(cam,GO->body->frame->position);
}

// Create an object in front of the character and fire
// Parameters:
//	GO: The object getting passed in, in this case the character
//	State: Needed to grab members
void State_CharacterController_ShootBullet(GObject* GO, State* state)
{
	//Get members
	struct State_CharacterController_Members* members = (struct State_CharacterController_Members*)state->members;

	// Camera local
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;
	// Gets the time per second
	float dt = TimeManager_GetDeltaSec();
	members->timer += dt;

	if(InputManager_GetInputBuffer().mouseLock)
	{
		// Create a net movement vector
		Vector direction;
		Vector_INIT_ON_STACK(direction, 3);
		if (InputManager_IsMouseButtonPressed(0) && members->timer >= members->coolDown)
		{
			//Get "forward" Vector
			Matrix_SliceRow(&direction, cam->rotationMatrix, 2, 0, 3);
			Vector_Scale(&direction,-1.0f);

			// Create the bullet object
			GObject* bullet = GObject_Allocate();
			GObject_Initialize(bullet);

			//bullet->mesh = AssetManager_LookupMesh("Sphere");
			bullet->mesh = AssetManager_LookupMesh("Arrow");
			bullet->texture = AssetManager_LookupTexture("Arrow");


			bullet->body = RigidBody_Allocate();
			RigidBody_Initialize(bullet->body, bullet->frameOfReference, 0.45f);
			bullet->body->coefficientOfRestitution = 0.2f;

			bullet->collider = Collider_Allocate();
			ConvexHullCollider_Initialize(bullet->collider);
			ConvexHullCollider_MakeRectangularCollider(bullet->collider->data->convexHullData, 0.1f, 2.0f, 0.1f);
			//AABBCollider_Initialize(bullet->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

			//Lay arrow flat
			GObject_Rotate(bullet, &Vector_E1, -3.14159f / 2.0f);

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


			Vector vector;
			Vector_INIT_ON_STACK(vector,3);
			vector.components[0] = 0.9f;
			vector.components[1] = 1.0f;
			vector.components[2] = 0.9f;
			GObject_Scale(bullet, &vector);

			Vector translation;
			Vector_INIT_ON_STACK(translation, 3);
			Vector_GetScalarProduct(&translation, &direction, 2.82843);
			GObject_Translate(bullet, GO->frameOfReference->position);
			GObject_Translate(bullet, &translation);

			Vector_Scale(&direction, 25.0f);

			//Vector_Increment(bullet->body->velocity,&direction);
			RigidBody_ApplyImpulse(bullet->body,&direction,&Vector_ZERO);

			//Add remove state
			State* state = State_Allocate();
			State_Remove_Initialize(state, 5.0f);
			GObject_AddState(bullet, state);

			ObjectManager_AddObject(bullet);

			members->timer = 0;
		}
	}
}
