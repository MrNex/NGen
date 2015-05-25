#ifndef STATE_RESET_H
#define STATE_RESET_H

#include "State.h"
#include "GObject.h"

///
//Initializes a reset state
//
//Parameters:
//	state: The state to initialize as a remove state
//	seconds: The number of seconds until reset of this object in the simulation
//	distance: The distance away from the initial position the object must be to trigger a reset
//	initPos: The position to reset the object to
//	initImp: the initial impulse to re-apply to the object
//	initRot: The initial rotation of the object
void State_Reset_Initialize(State* state, float seconds, float distance, Vector* initPos, Vector* initImp, Matrix* initRot);

///
//Frees memory allocated by a reset state
//
//Parameters:
//	state: The state to free
void State_Reset_Free(State* state);

///
//Updates a reset state, resettig the object when the timer is up
//
//Parameters:
//	GO: The game object with the attached reset state
//	state: THe state being updated
void State_Reset_Update(GObject* GO, State* state);

#endif
