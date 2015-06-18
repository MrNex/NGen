#ifndef RUNNERCONTROLLER_H
#define RUNNERCONTROLLER_H

#include "State.h"
#include "../GObject/GObject.h"

static const float spinningRate = 3.14159f * 5.0f;

///
//Initializes a runner controller state
//
//Parameters:
//	state: A pointer to the state to initialize as a runner controller
//	acceleration: The acceleration of the runner
//	maxvelocity: The max speed of the runner
//	angularVelocity: the angular velocity of the runner
//	jumpMag: The magnitude of the impulse which will be applied when the player jumps
void State_RunnerController_Initialize(State* state, const float acceleration, const float maxVelocity, const float angularVelocity, const float jumpMag);

///
//Frees members in a runner controller state
//
//Parameters:
//	state: A pointer to the state to free the members of
void State_RunnerController_Free(State* state);


///
//Updates the object attached to a runner controller
//This object will always accelerate forward while on a surface unless the velocity is already at the max velocity.
//
//Parameters:
//	obj: A pointer to the gameobject to update as a runner
//	state: A pointer to the runner controller state
void State_RunnerController_Update(GObject* obj, State* state);

///
//Accelerates the runner controller
//
//PArameters:
//	obj: A pointer to the game object to accelerate
//	state: A pointer to rhe runner controller state which is accelerating this object
void State_RunnerController_Accelerate(GObject* obj, State* state);

///
//Rotates the runner controller
//	obj: A pointer to the game object to rotate
//	state: A pointer to the runner controller rotating the object
void State_RunnerController_Rotate(GObject* obj, State* state);

///
//Allows the runner controller to jump if necessary conditions are met
//
//Parameters:
//	obj: A pointer to the object jumping
//	state: A pointer to the runner controller state which is jumping the object
void State_RunnerController_Jump(GObject* obj, State* state);

///
//Allows the runner controller to wallrun if necessary conditions are met
//
//Parameters:
//	obj: A pointer to the object which is running on walls
//	state: A pointer to the runner controller state which is allowing the object to wallrun
void State_RunnerController_Wallrun(GObject* obj, State* state);

///
//Lets the runner jump off of a wall
//	obj: A pointer to the object
//	state: A pointer to the runner controller state which is allowing the object to wall jump
void State_RunnerController_WallJump(GObject* obj, State* state);

///
//Lets the runner vault off of a wall
//	obj: A pointer to the object
//	state: A pointer to the runner controller state which is allowing the object to wall vault
void State_RunnerController_WallVault(GObject* obj, State* state);

#endif
