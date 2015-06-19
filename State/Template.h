#ifndef TEMPLATE_H
#define TEMPLATE_H

#include "State.h"
#include "../GObject/GObject.h"

///
//Initializes a Template state
//
//Parameters:
//	s: A pointer to the state to initialize
void State_Template_Initialize(State* s);

///
//Updates a GObject using a Template state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_Template_Update(GObject* obj, State* state);

///
//Frees memory allocated by a Template state
//
//Parameters:
//	state: The Template state to free
void State_Template_Free(State* state);

#endif
