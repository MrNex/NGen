include "FirstPersonCamera.h"

#include "InputManager.h"
#include "RenderingManager.h"
#include "TimeManager.h"
#include "PhysicsManager.h"

#include <stdio.h>


struct State_FirstPersonCamera_Members
{
	float rotationSpeed;
	float movementSpeed;
	float* selectedPlane;
};

///
//Initializes a first person camera state
//
//Parameters:
//	s: The state to initialize
void State_FirstPersonCamera_Initialize(State* s, const float velocity, const float angularVelocity)
{
	s->members = (State_Members)malloc(sizeof(struct State_FirstPersonCamera_Members));

	//Get members
	struct State_FirstPersonCamera_Members* members = (struct State_FirstPersonCamera_Members*)s->members;

	members->movementSpeed = velocity;
	members->rotationSpeed = angularVelocity;
	members->selectedPlane = &(RenderingManager_GetRenderingBuffer()->camera->nearPlane);

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
	//printf("\nUpdating Cam\n");
	State_FirstPersonCamera_Rotate(GO, state);
	State_FirstPersonCamera_Translate(GO, state);
	State_FirstPersonCamera_Skew(GO, state);

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

}

///
//Rotates the rendering manager's camera according to change in mouse position
//
//Parameters:
//	GO: The game object this state is attached to
//	state: The First person camera state updating the gameObject
void State_FirstPersonCamera_Rotate(GObject* GO, State* state)
{

	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

	if(InputManager_GetInputBuffer().mouseLock)
	{
		//Get members
		struct State_FirstPersonCamera_Members* members = (struct State_FirstPersonCamera_Members*)state->members;

		float dt = TimeManager_GetDeltaSec();

		int deltaMouseX = (InputManager_GetInputBuffer().mousePosition[0] - InputManager_GetInputBuffer().previousMousePosition[0]);
		int deltaMouseY = (InputManager_GetInputBuffer().mousePosition[1] - InputManager_GetInputBuffer().previousMousePosition[1]);

		Vector* axis = Vector_Allocate();
		Vector_Initialize(axis, 3);

		if (deltaMouseX != 0)
		{


			axis->components[1] = 1.0f;
			Camera_Rotate(cam, axis, members->rotationSpeed * deltaMouseX);
			axis->components[1] = 0.0f;
		}

		if (deltaMouseY != 0)
		{
			axis->components[0] = 1.0f;

			Camera_Rotate(cam, axis, members->rotationSpeed * deltaMouseY);
			axis->components[0] = 0.0f;
		}

		Vector_Free(axis);
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
	//TODO: Write Skew code here
	Camera* cam = RenderingManager_GetRenderingBuffer()->camera;

	if(InputManager_IsKeyDown('='))
	{
		cam->nearPlane *= 1.001f;
		printf("Near Plane: %f\n", cam->nearPlane);
		Camera_RefreshPerspectiveMatrix(cam);
	}
	if(InputManager_IsKeyDown('-'))
	{
		cam->nearPlane *= 0.999f;
		printf("Near Plane: %f\n", cam->nearPlane);
		Camera_RefreshPerspectiveMatrix(cam);
	}

}

