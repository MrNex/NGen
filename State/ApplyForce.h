#ifndef STATE_FORCE_H
#define STATE_FORCE_H

#include "State.h"
#include "../GObject/GObject.h"

///
//Initializes a new force state
//
//Parameters:
//	state: A pointer to the state to initialize as a force state
//	force: A pointer to a vector to copy as the force this state will apply
//	radius: A pointer to a vector to copy as the radius this state will apply the force at
void State_Force_Initialize(State* state, Vector* force, Vector* radius);

///
//Frees resources allocated by a force state
//
//Parameters:
//	state: A pointer to the state to free
void State_Force_Free(State* state);

///
//Updates a force state by applying the defined force to the GObject
//
//Parameters:
//	GO: A pointer to the game object to apply the force to
//	state: A pointer to the state updating the attached object
void State_Force_Update(GObject* GO, State* state);

#endif

