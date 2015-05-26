#ifndef STATE_REVOLUTION_H
#define STATE_REVOLUTION_H

#include "State.h"
#include "../GObject/GObject.h"
#include "../Math/Vector.h"

///
//Initializes a revolution state
//
//Parameters:
//	s: The state to be initialized as a revolution state
//	startPoint: The point which the body will begin revolving around the pointOfRevolution
//	pointOfRevolution: The point which this state will revolve it's object around
//	axisOfRevolution: The vector representing the axis to revolve around (unit vector please)
//	aVel: The angular velocity which this state will revolve it's object at
void State_Revolution_Initialize(State* s, const Vector* startPoint, Vector* pointOfRevolution, Vector* axisOfRevolution, const float aVel);

///
//Frees memory used by a revolution State
//
//Parameters:
//	s: The revolution state to free
void State_Revolution_Free(State* s);

///
//Revolves an object according to it's revolution state
//
//Parameters:
//	GO: The game object with a RevolutionState being revolved
//	state: The revolution State updating the GameObject
void State_Revolution_Update(GObject* GO, State* state);

#endif

