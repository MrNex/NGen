#ifndef PARKOURCONTROLLER_H
#define PARKOURCONTROLLER_H

#include "State.h"
#include "../GObject/GObject.h"

static const float spinningRate = 3.14159f * 5.0f;
static const float rollRate = 3.14159f * 2.0f * 4.0f;
static const float skewRate = 0.2f;
static const float restoreRate = 2.0f;

///
//Initializes a ParkourController state
//
//Parameters:
//	s: A pointer to the state to initialize
//	acceleration: The acceleration of the parkourController
//	maxVelocity: The maximum linear velocity of the parkourController
//	angularVelocity: The angular velocity of the parkour controller (How fast can you turn?)
//	jumpImpulse: The force exerted when jumping
//	shootSpeed: The amount of seconds you must wait before shooting again
void State_ParkourController_Initialize(State* s, const float acceleration, const float maxVelocity, const float angularVelocity, const float jumpImpulse, const float shootSpeed);

///
//Updates a GObject using a Template state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_ParkourController_Update(GObject* obj, State* state);

///
//Frees memory allocated by a Template state
//
//Parameters:
//	state: The Template state to free
void State_ParkourController_Free(State* state);

#endif
