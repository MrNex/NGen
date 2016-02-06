#include "GObject.h"

///
//Allocates memory for a new Game Object
//
//Returns:
//	Pointer to a newly allocated game object
GObject* GObject_Allocate(void)
{
	GObject* GO = (GObject*)malloc(sizeof(GObject));
	return GO;
}

///
//Initializes all members of a Game Object (GObject)
//
//Parameters:
//	GO: The Game Object to initialize
void GObject_Initialize(GObject* GO)
{
	GO->frameOfReference = FrameOfReference_Allocate();
	FrameOfReference_Initialize(GO->frameOfReference);

	GO->states = LinkedList_Allocate();
	LinkedList_Initialize(GO->states);

	GO->mesh = NULL;
	GO->material = NULL;
	GO->body = NULL;
	GO->collider = NULL;
	GO->light = NULL;
}

///
//Initializes a deep copy of a GObject
//This means any pointers will point to a NEWLY ALLOCATED instance of identical memory
//NOTE: Does not copy Mesh, Texture, or States!
//
//Parameters:
//	copy: A pointer to an uninitialized GObject to initialize as a deep copy
//	original: A pointer to a GObject to deep copy
void GObject_InitializeDeepCopy(GObject* copy, GObject* original)
{
	copy->frameOfReference = FrameOfReference_Allocate();
	FrameOfReference_InitializeDeepCopy(copy->frameOfReference, original->frameOfReference);

	copy->states = LinkedList_Allocate();
	LinkedList_Initialize(copy->states);

	copy->mesh = original->mesh;

	if(original->material != NULL)
	{
		copy->material = Material_Allocate();
		Material_InitializeDeepCopy(copy->material, original->material);
	}
	else
	{
		copy->material = NULL;
	}

	if(original->collider != NULL)
	{
		copy->collider = Collider_Allocate();
		if(original->collider->type == COLLIDER_SPHERE)
		{
			SphereCollider_InitializeDeepCopy(copy->collider, original->collider);
		}
		else if(original->collider->type == COLLIDER_AABB)
		{
			AABBCollider_InitializeDeepCopy(copy->collider, original->collider);
		}
		else if(original->collider->type == COLLIDER_CONVEXHULL)
		{
			ConvexHullCollider_InitializeDeepCopy(copy->collider, original->collider);
		}
		else
		{
			printf("Error collider type unknown! Not Copied!\n");
			free(copy->collider);
			copy->collider = NULL;
		}
	}
	else
	{
		copy->collider = NULL;
	}

	if(original->body != NULL)
	{
		copy->body = RigidBody_Allocate();
		RigidBody_InitializeDeepCopy(copy->body, original->body);
	}
	else
	{
		copy->body = NULL;
	}

	if(original->light != NULL)
	{
		//TODO: Write deepcopy for light and replace
		copy->light = NULL;
	}
	else
	{
		copy->light = NULL;
	}

}

///
//Frees all components of a GObject
//
//Parameters:
//	GO: The GObject to free
void GObject_Free(GObject* GO)
{
	//Free frame of reference
	FrameOfReference_Free(GO->frameOfReference);

	//Free all (if any) states
	struct LinkedList_Node* current = GO->states->head;
	struct LinkedList_Node* next = NULL;
	State* currentState = NULL;

	while(current != NULL)
	{
		next = current->next;
		currentState = (State*)current->data;
		//Free the state
		//which frees the members
		State_Free(currentState);
		current = next;
	}

	//Free list of states
	LinkedList_Free(GO->states);

	//Free other components if gameobject has them

	//One mesh might be shared by many objects, so we probably should leave
	//The deletion of these to the AssetManager
	//if(GO->mesh != NULL)
		//Mesh_Free(GO->mesh);
	//if(GO->texture != NULL)
		//Texture_Free(GO->texture);
	if(GO->material != NULL)
	{
		Material_Free(GO->material);
	}
	if(GO->body != NULL)
	{
		RigidBody_Free(GO->body);
	}
	if(GO->collider != NULL)
	{
		Collider_Free(GO->collider);
	}
	if(GO->light != NULL)
	{
		PointLight_Free(GO->light);
	}

	memset(GO, 0, sizeof(GObject));

	//Free the gameObject
	free(GO);
}

///
//Adds a state to a game object
//
//Parameters:
//	GO: Pointer to game object to add state to
//	state: Pointer to state to add
void GObject_AddState(GObject* GO, State* state)
{
	LinkedList_Append(GO->states, state);
}

///
//Removes a state from a game object
//
//Parameters:
//	GO: Pointer to game object to remove a state from
//	stateIndex: Index of the state to remove (0 is the first state, up to the most recent)
void GObject_RemoveState(GObject* GO, int stateIndex)
{
	LinkedList_RemoveIndex(GO->states, stateIndex);
}

///
//Calls the update function of each state attached to a gameobject
//
//Parameters:
//	GO: The gameobject to update
void GObject_Update(GObject* GO)
{
	struct LinkedList_Node* current = GO->states->head;
	struct LinkedList_Node* next = NULL;
	State* currentState = NULL;

	while(current != NULL)
	{
		next = current->next;
		currentState = (State*)current->data;
		currentState->State_Update(GO, currentState);
		current = next;
	}
}

///
//Translates a GObject's frame of reference in world space
//
//Parameters:
//	GO: The game object to translate
//	translation: The Vector to translate by
void GObject_Translate(GObject* GO, Vector* translation)
{
	FrameOfReference_Translate(GO->frameOfReference, translation);
	if(GO->body != NULL)
	{
		RigidBody_Translate(GO->body, translation);
	}
}

///
//Rotates a GObject's frame of reference around a specified axis by a specified amount
//
//Parameters:
//	GO: The game object being rotated
//	axis: The Vector to rotate the object around
//	radians: The number of radians to rotate by
void GObject_Rotate(GObject* GO,const Vector* axis, float radians)
{
	FrameOfReference_Rotate(GO->frameOfReference, axis, radians);
	if(GO->body != NULL)
	{
		RigidBody_Rotate(GO->body, axis, radians);
	}
}

///
//Scales a GObject's frame of reference on each elementary axis
//
//Parameters:
//	GO: The gameObject being scaled
//	scaleVector: A 3 DIM Vector comtaining the X, Y, and Z scale factors
void GObject_Scale(GObject* GO, Vector* scaleVector)
{
	FrameOfReference_Scale(GO->frameOfReference, scaleVector);
	if(GO->body != NULL)
	{
		RigidBody_Scale(GO->body, scaleVector);
	}
}

///
//Sets the position of a GObject in world space
//
//Parameters:
//      GO: The game object to set the position of
//      position: The vector to set the position to
void GObject_SetPosition(GObject* GO, Vector* position)
{
        FrameOfReference_SetPosition(GO->frameOfReference, position);
        if(GO->body != NULL)
        {
			RigidBody_SetPosition(GO->body, position);
        }
}

///
//Sets the rotation of a GObject in world space
//
//Parameters:
//      GO: The game object to set the rotation of
//      rotation: The matrix to set the rotation to
void GObject_SetRotation(GObject* GO, Matrix* rotation)
{
        FrameOfReference_SetRotation(GO->frameOfReference, rotation);
        if(GO->body != NULL)
        {
			RigidBody_SetRotation(GO->body, rotation);
        }
}

