#ifndef STATE_ROTATE_H
#define STATE_ROTATE_H

#include "State.h"
#include "GObject.h"
#include "Vector.h"

///
//Initializes a rotation state
//
//Parameters:
//	s: The state to be initialized as a rotation state
//	axis: The axis which this state will rotate it's object around
//	aVel: The angular velocity which this state will rotate it's object at
void State_Rotate_Initialize(State* s, const Vector* axis, const float aVel);

///
//Frees memory used by a Rotation State
//
//Parameters:
//	s: The rotation state to free
void State_Rotate_Free(State* s);

///
//Rotates an object according to it's rotation state
//
//Parameters:
//	GO: The game object with a RotationState being rotated
//	state: The Rotate State updating the GameObject
void State_Rotate_Update(GObject* GO, State* state);

#endif

