#ifndef STATE_ROTATECOORDINATEAXIS_H
#define STATE_ROTATECOORDINATEAXIS_H
#include "State.h"
#include "../GObject/GObject.h"
#include "../Math/Vector.h"


///
//Initializes a RotateCoordinateAxis state
//
//Parameters:
//	s: The state to initialize to a RotateCoordinateAxis state
//	axis: The coordinate axis to rotate (0 - X | 1 - Y | 2 - Z)
//	rotationAxis: The axis to rotate around
//	angularVelocity: The speed by which to rotate
void State_RotateCoordinateAxis_Initialize(State* s, const int axis, const Vector* rotationAxis, const float angularVelocity);

///
//Frees resources allocated by a RotateCoordinateAxis State
//
//Parameters:
//	s: The state to free
void State_RotateCoordinateAxis_Free(State* s);

///
//Updates the coordinate system of the GameObject to rotate the axis
//Represented by row (param axis) of the rotation matrix
//To be rotated around (param rotationAxis) by (param angularVelocity)
//
//Parameters:
//	GO: The game object to rotate one of it's coordinate axis
//	state: The RotateCoordinateAxis state updating the GameObject
void State_RotateCoordinateAxis_Update(GObject* GO, State* state);



#endif
