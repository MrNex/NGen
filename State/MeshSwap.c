#include "MeshSwap.h"

#include "../Manager/InputManager.h"
#include "../Manager/AssetManager.h"


//This state needs no members.
/*
struct State_Members
{
	
};
*/

///
//Initializes a mesh swap state
//
//Parameters:
//	s: The state to initialize
void State_MeshSwap_Initialize(State* s)
{
	s->members = NULL;
	s->State_Members_Free = State_MeshSwap_Free;
	s->State_Update = State_MeshSwap_Update;
}

///
//Frees memory used by a Mesh Swap state
//
//Parameters:
//	s: The mesh swap state to free
void State_MeshSwap_Free(State* s)
{
	//No members to free
}

///
//Updates a mesh swap state's attached gameObject.
//Switches the mesh of the gameObject based on the current input.
//
//Parameters:
//	GO: The gameObject with a MeshSwapState being updated
//	state: The MeshSwap state updating the gameobject
void State_MeshSwap_Update(GObject* GO, State* state)
{
	if(InputManager_IsKeyDown('1'))
	{
		GO->mesh = AssetManager_LookupMesh("Cube");
	}
	else if(InputManager_IsKeyDown('2'))
	{
		GO->mesh = AssetManager_LookupMesh("Sphere");
	}
	else if(InputManager_IsKeyDown('3'))
	{
		GO->mesh = AssetManager_LookupMesh("Pipe");
	}
	else if(InputManager_IsKeyDown('4'))
	{
		GO->mesh = AssetManager_LookupMesh("Cylinder");
	}
	else if(InputManager_IsKeyDown('5'))
	{
		GO->mesh = AssetManager_LookupMesh("Cone");
	}
	else if(InputManager_IsKeyDown('6'))
	{
		GO->mesh = AssetManager_LookupMesh("Torus");
	}
	else if(InputManager_IsKeyDown('7'))
	{
		GO->mesh = AssetManager_LookupMesh("Suzanne");
	}

	if(InputManager_IsKeyDown('r'))
	{
		*Matrix_Index(GO->colorMatrix, 0, 0) = 1.0f;
		*Matrix_Index(GO->colorMatrix, 1, 1) = 0.0f;
		*Matrix_Index(GO->colorMatrix, 2, 2) = 0.0f;
	}
	else if(InputManager_IsKeyDown('g'))
	{
		*Matrix_Index(GO->colorMatrix, 0, 0) = 0.0f;
		*Matrix_Index(GO->colorMatrix, 1, 1) = 1.0f;
		*Matrix_Index(GO->colorMatrix, 2, 2) = 0.0f;
	}
	else if(InputManager_IsKeyDown('b'))
	{
		*Matrix_Index(GO->colorMatrix, 0, 0) = 0.0f;
		*Matrix_Index(GO->colorMatrix, 1, 1) = 0.0f;
		*Matrix_Index(GO->colorMatrix, 2, 2) = 1.0f;
	}
	else if(InputManager_IsKeyDown('c'))
	{
		*Matrix_Index(GO->colorMatrix, 0, 0) = 1.0f;
		*Matrix_Index(GO->colorMatrix, 1, 1) = 1.0f;
		*Matrix_Index(GO->colorMatrix, 2, 2) = 1.0f;
	}

	if(InputManager_IsKeyDown('p'))
	{
		GO->mesh->primitive = GL_POINTS;
	}
	else if(InputManager_IsKeyDown('l'))
	{
		GO->mesh->primitive = GL_LINES;
	}
	else if(InputManager_IsKeyDown('t'))
	{
		GO->mesh->primitive = GL_TRIANGLES;
	}

	if(InputManager_IsKeyDown(' '))
	{
		Vector radius;
		Vector_INIT_ON_STACK(radius, 3);

		Vector_GetScalarProduct(&radius, &Vector_E1, 10);

		RigidBody_ApplyForce(GO->body, &Vector_E3, &radius);
	}
}
