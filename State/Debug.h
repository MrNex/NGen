#ifndef Debug_H
#define Debug_H

#include "State.h"
#include "../GObject/GObject.h"

///
//Initializes a Debug state
//
//Parameters:
//	s: A pointer to the state to initialize
//	name: the name to print with the given debug statement
void State_Debug_Initialize(State* s, char* name);

///
//Updates a GObject using a Debug state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_Debug_Update(GObject* obj, State* state);

///
//Frees memory allocated by a Debug state
//
//Parameters:
//	state: The Debug state to free
void State_Debug_Free(State* state);

#endif
