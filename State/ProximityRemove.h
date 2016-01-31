#ifndef PROXIMITYREMOVE_H
#define PROXIMITYREMOVE_H

#include "State.h"
#include "../GObject/GObject.h"

///
//Initializes a Proxiimityremove state
//
//Parameters:
//	s: A pointer to the state to initialize as a proximity remove state
//	target: A pointer to the vector to track the distance this game object is away from
//	dist: The maximum distance between attached GObject before deletion
void State_ProximityRemove_Initialize(State* s, Vector* target, float dist);

///
//Updates a GObject using a ProximityRemove state
//the state will delete the attached object once it is further away
//than the specified distance from the target
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the proximityRemove state updating the GObject
void State_ProximityRemove_Update(GObject* obj, State* state);

///
//Frees memory allocated by a ProximityRemove state
//
//Parameters:
//	state: The Template state to free
void State_ProximityRemove_Free(State* state);

#endif
