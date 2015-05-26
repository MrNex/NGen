#ifndef STATE_FIRSTPERSONCAMERA_H
#define STATE_FIRSTPERSONCAMERA_H

#include "State.h"

///
//Initializes a first person camera state
//
//Parameters:
//	s: The state to initialize
void State_FirstPersonCamera_Initialize(State* s, const float velocity, const float angularVelocity);

///
//Frees resources taken by a First Person Camera State
//
//Parameters:
//	s: The state to free
void State_FirstPersonCamera_Free(State* s);

///
//Updates the RenderManager's camera according to user input
//Moves, Rotates, and Skews the camera
//
//Parameters:
//	GO: The game object this state is attached to
//	state: The state updating this gameObject
void State_FirstPersonCamera_Update(GObject* GO, State* state);

///
//Rotates the rendering manager's camera according to change in mouse position
//
//Parameters:
//	GO: The game object this state is attached to
//	state: The first person camera state updating the gameObject
void State_FirstPersonCamera_Rotate(GObject* GO, State* state);

///
//Translates the RenderingManager's camera according to keyboard input
//
//Parameters:
//	GO: The game object this state is attached to
//	state: the first person camera state updating the gameObject
void State_FirstPersonCamera_Translate(GObject* GO, State* state);

///
//Skews the rendering manager's camera according to keyboard input
//
//Parameters:
//	GO: The game object this state is attached to
//	state: The First person camera state updating the gameObject
void State_FirstPersonCamera_Skew(GObject* GO, State* state);

#endif

