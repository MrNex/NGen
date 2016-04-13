#ifndef STATE_HORIZONTALCONTROLLER_H
#define STATE_HORIZONTALCONTROLLER_H

#include "State.h"
#include "../GObject/GObject.h"

///
//Initializes a HorizontalController state
//
//Parameters:
//	s: A pointer to the state to initialize
//	speed: The speed of the movement
void State_HorizontalController_Initialize(State* s, float speed);

///
//Updates a GObject using a HorizontalController state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_HorizontalController_Update(GObject* obj, State* state);

///
//Frees memory allocated by a HorizontalController state
//
//Parameters:
//	state: The HorizontalController state to free
void State_HorizontalController_Free(State* state);

#endif
