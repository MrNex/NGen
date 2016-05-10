#ifndef SHOOT_H
#define SHOOT_H

#include "State.h"
#include "../GObject/GObject.h"

///
//Initializeis a Shoot state
//
//Parameters:
//	s: A pointer to the state to initialize
//	shootSpeed: The fire rate
//	ttl: The time the bullets stick around before getting deleted
void State_Shoot_Initialize(State* s, float shootSpeed, float ttl, unsigned int objID);

///
//Updates a GObject using a Shoot state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_Shoot_Update(GObject* obj, State* state);

///
//Frees memory allocated by a Shoot state
//
//Parameters:
//	state: The shoot state to free
void State_Shoot_Free(State* state);

#endif
