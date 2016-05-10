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


#include "FirstPersonCamera.h"

#include "../Manager/InputManager.h"
#include "../Manager/RenderingManager.h"
#include "../Manager/TimeManager.h"
#include "../Manager/PhysicsManager.h"

#include <stdio.h>
#include <math.h>

struct State_FirstPersonCamera_Members
{
	float rotationSpeed;
	float movementSpeed;
	float skewRate;
};

///
//Initializes a first person camera state
//
//Parameters:
//	s: The state to initialize
//	velocity: The movement speed of the camera
//	angularVelocity: The rotation speed of the camera
//	skewSpeed: The rate at which the camera can skew 
void State_FirstPersonCamera_Initialize(State* s, const float velocity, const float angularVelocity, const float skewSpeed)
{
	s->members = (State_Members)malloc(sizeof(struct State_FirstPersonCamera_Members));

	//Get members
	struct State_FirstPersonCamera_Members* members = (struct State_FirstPersonCamera_Members*)s->members;

	members->movementSpeed = velocity;
	members->rotationSpeed = angularVelocity;
	members->skewRate = skewSpeed;

	s->State_Update = State_FirstPersonCamera_Update;
	s->State_Members_Free = State_FirstPersonCamera_Free;
}

void State_FirstPersonCamera_Free(State* s)
{
	//Get members
	struct State_FirstPersonCamera_Members* members = (struct State_FirstPersonCamera_Members*)s->members;

	free(members);
}

///
//Updates the RenderManager's camera according to user input
//Moves, Rotates, and Skews the camera
//
//Parameters:
//	GO: The game object this state is attached to
//	state: The First person camera state updating the gameObject
void State_FirstPersonCamera_Update(GObject* GO, State* state)
{
	State_FirstPersonCamera_Rotate(GO, state);
	State_FirstPersonCamera_Translate(GO, state);
	State_FirstPersonCamera_Skew(GO, state);

	/*
	//Experimental feature
	if(InputManager_IsKeyDown('o'))
	{
		//Get Camera
		Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

		//Get the physics buffer
		PhysicsBuffer* buffer = PhysicsManager_GetPhysicsBuffer();

		//Get the global gravity force
		//Vector* gravity = (Vector*)buffer->globalForces->head->data;

		//Get z axis from camera
		//Vector zAxis;
		//Vector_INIT_ON_STACK(zAxis, 3);

		//Matrix_SliceRow(&zAxis, cam->rotationMatrix, 2, 0, 3);
		//Vector_Scale(&zAxis, -1.0f);

		//Vector_Copy(gravity, &zAxis);
	}
	*/

}

///
//Rotates the rendering manager's camera according to change in mouse position
//
//Parameters:
//	GO: The game object this state is attached to
//	state: The First person camera state updating the gameObject
void State_FirstPersonCamera_Rotate(GObject* GO, State* state)
{
	//GO is unused
	(void)GO;

	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

	//If the player's mouse is locked
	if(InputManager_GetInputBuffer().mouseLock)
	{

		
		//Get members
		struct State_FirstPersonCamera_Members* members = (struct State_FirstPersonCamera_Members*)state->members;
		//Get the change in mouse position
		int deltaMouseX = (InputManager_GetInputBuffer().mousePosition[0] - InputManager_GetInputBuffer().previousMousePosition[0]);
		int deltaMouseY = (InputManager_GetInputBuffer().mousePosition[1] - InputManager_GetInputBuffer().previousMousePosition[1]);

		//If there is X rotation
		if(deltaMouseX != 0)
		{
			if(deltaMouseX > 10)
			{
				deltaMouseX = 10;
			}
			else if(deltaMouseX < -10)
			{
				deltaMouseX = -10;
			}
			//Rotate the camera
			Camera_ChangeYaw(cam, (float)deltaMouseX * members->rotationSpeed);
		}
		//If there is Y rotation
		if(deltaMouseY != 0)
		{
			if(deltaMouseY > 10)
			{
				deltaMouseY = 10;
			}
			else if(deltaMouseY < -10)
			{
				deltaMouseY = -10;
			}
			//TODO: Prevent camera from looking "Too high"
			//Rotate the camera
			Camera_ChangePitch(cam, (float)deltaMouseY * members->rotationSpeed);
		}
	}


}

///
//Translates the RenderingManager's camera according to keyboard input
//
//Parameters:
//	GO: The game object this state is attached to
//	state: the First Person Camera State updating the gameObject
void State_FirstPersonCamera_Translate(GObject* GO, State* state)
{
	//GO is unused
	(void)GO;

	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

	if(InputManager_GetInputBuffer().mouseLock)
	{
		//Get members
		struct State_FirstPersonCamera_Members* members = (struct State_FirstPersonCamera_Members*)state->members;

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

		float dt = TimeManager_GetDeltaSec();

		if (Vector_GetMag(&netMvmtVec) > 0.0f && dt > 0.0f)
		{
			Vector_Normalize(&netMvmtVec);
			Vector_Scale(&netMvmtVec, members->movementSpeed * dt);

			Camera_Translate(cam, &netMvmtVec);
		}
	}

}

///
//Skews the rendering manager's camera according to keyboard input
//
//Parameters:
//	GO: The game object this state is attached to
//	state: The First person camera state updating the gameObject
void State_FirstPersonCamera_Skew(GObject* GO, State* state)
{
	//GO is unused
	(void)GO;	

	//Get members
	struct State_FirstPersonCamera_Members* members = (struct State_FirstPersonCamera_Members*)state->members;
	//Get camera
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;
	//Get dt
	float dt = TimeManager_GetDeltaSec();

	if(InputManager_IsKeyDown('='))
	{
		cam->nearPlane += members->skewRate * dt;
		printf("Near Plane: %f\n", cam->nearPlane); 
		Camera_RefreshPerspectiveMatrix(cam);
	}
	if(InputManager_IsKeyDown('-'))
	{
		cam->nearPlane -= members->skewRate * dt;
		printf("Near Plane: %f\n", cam->nearPlane);
		Camera_RefreshPerspectiveMatrix(cam);
	}

}

