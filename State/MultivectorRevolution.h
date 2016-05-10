#ifndef MUTLIVECTORREVOLUTIONSTATE_H
#define MUTLIVECTORREVOLUTIONSTATE_H

#include "State.h"
#include "../GObject/GObject.h"

///
//Initializes a MultivectorRevolution state
//
//Parameters:
//	s: A pointer to the state to initialize
void State_MultivectorRevolution_Initialize(State* s, float v);

///
//Updates a GObject using a MultivectorRevolution state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_MultivectorRevolution_Update(GObject* obj, State* state);

///
//Frees memory allocated by a MultivectorRevolution state
//
//Parameters:
//	state: The MultivectorRevolution state to free
void State_MultivectorRevolution_Free(State* state);

#endif
