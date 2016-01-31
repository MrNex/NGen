#ifndef GOBJECT_H
#define GOBJECT_H

#include "../Render/FrameOfReference.h"
#include "../Render/Mesh.h"
#include "../Render/Material.h"

#include "../State/State.h"

#include "../Physics/RigidBody.h"

#include "../Collision/Collider.h"

typedef struct GObject
{
	FrameOfReference* frameOfReference;
	Mesh* mesh;
	Material* material;
	LinkedList* states;
	RigidBody* body;
	Collider* collider;
} GObject;

///
//Allocates memory for a new Game Object
//
//Returns:
//	Pointer to a newly allocated game object
GObject* GObject_Allocate(void);

///
//Initializes all members of a Game Object (GObject)
//
//Parameters:
//	GO: The Game Object to initialize
void GObject_Initialize(GObject* GO);

///
//Initializes a deep copy of a GObject
//This means any pointers will point to a NEWLY ALLOCATED instance of identical memory unless otherwise noted
//NOTE: Does not copy Mesh, Texture, or States!
//
//Parameters:
//	copy: A pointer to an uninitialized GObject to initialize as a deep copy
//	original: A pointer to a GObject to deep copy
void GObject_InitializeDeepCopy(GObject* copy, GObject* original);

///
//Frees all components of a GObject
//
//Parameters:
//	GO: The GObject to free
void GObject_Free(GObject* GO);

///
//Adds a state to a game object
//
//Parameters:
//	GO: Pointer to game object to add state to
//	state: Pointer to state to add
void GObject_AddState(GObject* GO, State* state);

///
//Removes a state from a game object
//
//Parameters:
//	GO: Pointer to game object to remove a state from
//	stateIndex: Index of the state to remove (0 is the first state, up to the most recent)
void GObject_RemoveState(GObject* GO, int stateIndex);

///
//Calls the update function of each state attached to a gameobject
//
//Parameters:
//	GO: The gameobject to update
void GObject_Update(GObject* GO);

///
//Translates a GObject in world space
//
//Parameters:
//	GO: The game object to translate
//	translation: The Vector to translate by
void GObject_Translate(GObject* GO, Vector* translation);

///
//Rotates a GObject around a specified axis by a specified amount
//
//Parameters:
//	GO: The game object being rotated
//	axis: The Vector to rotate the object around
//	radians: The number of radians to rotate by
void GObject_Rotate(GObject* GO, const Vector* axis, float radians);

///
//Scales a GObject on each elementary axis
//
//Parameters:
//	GO: The gameObject being scaled
//	scaleVector: A 3 DIM Vector comtaining the X, Y, and Z scale factors
void GObject_Scale(GObject* GO, Vector* scaleVector);

///
//Sets the position of a GObject
//
//Parameters:
//  GO: The game object to set the position of
//  position: The position to move the object to
void GObject_SetPosition(GObject* GO, Vector* position);

///
//Sets the rotation of a GObject
//
//Parameters:
//  GO: The game object to set the rotation of
//  position: The rotation to set the object to
void GObject_SetRotation(GObject* GO, Matrix* rotation);

#endif
