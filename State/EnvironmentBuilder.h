#ifndef ENVIRONMENTBUILDER_H
#define ENVIRONMENTBUILDER_H

#include "State.h"
#include "../GObject/GObject.h"

///
//Initializes an environment builder state
//
//Parameters:
//	s: A pointer to the state to initialize
void State_EnvironmentBuilder_Initialize(State* s);

///
//Updates a GObject using an EnvironmentBuilder state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the EnvironmentBuilder state updating the GObject
void State_EnvironmentBuilder_Update(GObject* obj, State* state);

///
//Frees memory allocated by a EnvironmentBuilder state
//
//Parameters:
//	state: The EnvironmentBuilder state to free
void State_EnvironmentBuilder_Free(State* state);

#endif
