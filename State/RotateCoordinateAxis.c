#include "RotateCoordinateAxis.h"

struct State_RotateCoordinateAxis_Members
{
	struct Vector* rotationAxis;
	int axis;
	float angularVelocity;
};

///
//Initializes a RotateCoordinateAxis state
//
//Parameters:
//	s: The state to initialize to a RotateCoordinateAxis state
//	axis: The coordinate axis to rotate (0 - X | 1 - Y | 2 - Z)
//	rotationAxis: The axis to rotate around
//	angularVelocity: The speed by which to rotate
void State_RotateCoordinateAxis_Initialize(State* s, const int axis, const Vector* rotationAxis, const float angularVelocity)
{
	s->members = (State_Members)malloc(sizeof(struct State_RotateCoordinateAxis_Members));
	//Get members
	struct State_RotateCoordinateAxis_Members* members = (struct State_RotateCoordinateAxis_Members*)s->members;


	members->rotationAxis = Vector_Allocate();
	Vector_Initialize(members->rotationAxis, 3);

	Vector_Copy(members->rotationAxis, rotationAxis);

	members->axis = axis;
	members->angularVelocity = angularVelocity;

	s->State_Update = State_RotateCoordinateAxis_Update;
	s->State_Members_Free = State_RotateCoordinateAxis_Free;
}

///
//Frees resources allocated by a RotateCoordinateAxis State
//
//Parameters:
//	s: The state which members are being freed
void State_RotateCoordinateAxis_Free(State* s)
{
	//Get members
	struct State_RotateCoordinateAxis_Members* members = (struct State_RotateCoordinateAxis_Members*)s->members;

	Vector_Free(members->rotationAxis);
	free(members);
}

///
//Updates the coordinate system of the GameObject to rotate the axis
//Represented by row (param axis) of the rotation matrix
//To be rotated around (param rotationAxis) by (param angularVelocity)
//
//Parameters:
//	GO: The game object to rotate one of it's coordinate axis
//	state: The RotateCoordinateAxis state updating the gameobject
void State_RotateCoordinateAxis_Update(GObject* GO, State* state)
{
	//Get members
	struct State_RotateCoordinateAxis_Members* members = (struct State_RotateCoordinateAxis_Members*)state->members;

	// Create Vector to hold coordinate axis
	Vector axis;
	Vector_INIT_ON_STACK(axis, 3);

	//Slice coordinate system taking the axis being rotated
	Matrix_SliceRow(&axis, GO->frameOfReference->rotation, members->axis, 0, 3);

	if(axis.components[members->axis] <= 0) members->angularVelocity *= -1.0f;

	//Get 3x3 rotation matrix needed for transform 
	Matrix rotation;
	Matrix_INIT_ON_STACK(rotation, 3, 3);

	FrameOfReference_ConstructRotationMatrix(&rotation, members->rotationAxis, members->angularVelocity);

	//Appy the rotation matrix transforming the axis
	Matrix_TransformVector(&rotation, &axis);

	//Replace the entries in the gameobject's coordinate system with the new axis
	*Matrix_Index(GO->frameOfReference->rotation, members->axis, 0) = axis.components[0];
	*Matrix_Index(GO->frameOfReference->rotation, members->axis, 1) = axis.components[1];
	*Matrix_Index(GO->frameOfReference->rotation, members->axis, 2) = axis.components[2];

}
