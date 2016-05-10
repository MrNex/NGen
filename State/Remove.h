#ifndef STATE_REMOVE_H
#define STATE_REMOVE_H

#include "State.h"
#include "../GObject/GObject.h"

///
//Initializes a remove state
//
//Parameters:
//	s: The state to initialize as a remove state
//	seconds: The number of seconds until removal of this object from the simulation
void State_Remove_Initialize(State* state, float seconds, unsigned int objID);

///
//Frees memory allocated by a remove state
//
//Parameters:
//	state: The state to free
void State_Remove_Free(State* state);

///
//Updates a remove state removing the object when the timer is up
//
//Parameters:
//	GO: The game object with the attached remove state
//	state: THe state being updated
void State_Remove_Update(GObject* GO, State* state);

#endif
