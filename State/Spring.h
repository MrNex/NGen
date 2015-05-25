#ifndef STATE_SPRING_H
#define STATE_SPRING_H

#include "State.h"
#include "GObject.h"

///
//Initializes a new spring state
//Any GObject with this state attached must have a rigidbody!!
//
//Parametrs:
//	s: State to initialize
//	k: Spring constant (How stiff is the spring, (0.0f - infinity))
//	restPosition: Position where spring sits at rest
void State_Spring_Initialize(State* s, const float k, const Vector* restPosition);

///
//Frees resources allocated by a spring state
//Call State_Free! Not this! State_Free will call this.
//
//Parameters:
//	s: The spring state to free
void State_Spring_Free(State* s);

///
//Applies forces to an object's Rigidbody component to 
//simulate spring mechanics.
//
//Parameters:
//	GO: Game Object with a SpringState being updated.
//	state: The springstate updating the Game Object
void State_Spring_Update(GObject* GO, State* state);

#endif
