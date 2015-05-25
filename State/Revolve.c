#include "RevolutionState.h"

#include "TimeManager.h"

struct State_Revolution_Members
{
	float angularVelocity;
	FrameOfReference* frameOfRevolution;
	Vector* startPoint;
	Vector* axisOfRevolution;
};


///
//Initializes a revolution state
//
//Parameters:
//	s: The state to be initialized as a revolution state
//	startPoint: The point which the body will begin revolving around the pointOfRevolution
//	pointOfRevolution: The point which this state will revolve it's object around
//	axisOfRevolution: The vector representing the axis to revolve around (unit vector please)
//	aVel: The angular velocity which this state will revolve it's object at
void State_Revolution_Initialize(State* s, const Vector* startPoint, Vector* pointOfRevolution, Vector* axisOfRevolution, const float aVel)
{
	s->members = (State_Members)malloc(sizeof(struct State_Revolution_Members));
	
	//Get members as a State_Revolution_Members struct
	struct State_Revolution_Members* members = (struct State_Revolution_Members*)s->members;

	members->frameOfRevolution = FrameOfReference_Allocate();
	members->frameOfRevolution->position = pointOfRevolution;

	members->frameOfRevolution->rotation = Matrix_Allocate();
	Matrix_Initialize(members->frameOfRevolution->rotation, 3, 3);

	members->frameOfRevolution->scale = Matrix_Allocate();
	Matrix_Initialize(members->frameOfRevolution->scale, 3, 3);

	members->startPoint = Vector_Allocate();
	Vector_Initialize(members->startPoint, 3);
	Vector_Copy(members->startPoint, startPoint);

	members->axisOfRevolution = Vector_Allocate();
	Vector_Initialize(members->axisOfRevolution, 3);
	Vector_Copy(members->axisOfRevolution, axisOfRevolution);


	members->angularVelocity = aVel;

	s->State_Update = State_Revolution_Update;
	s->State_Members_Free = State_Revolution_Free;
}

///
//Frees memory used by a revolution State
//
//Parameters:
//	s: The revolution state to free
void State_Revolution_Free(State* s)
{
	//Get members as a State_Revolution_Members struct
	struct State_Revolution_Members* members = (struct State_Revolution_Members*)s->members;

	Vector_Free(members->startPoint);
	
	Matrix_Free(members->frameOfRevolution->rotation);
	Matrix_Free(members->frameOfRevolution->scale);

	free(members->frameOfRevolution);

	free(members);
}

///
//Revolves an object according to it's revolution state
//
//Parameters:
//	GO: The game object with a RevolutionState being revolved
//	state: The revolution State updating the GameObject
void State_Revolution_Update(GObject* GO, State* state)
{
	//Get members as a State_Revolution_Members struct
	struct State_Revolution_Members* members = (struct State_Revolution_Members*)state->members;

	//Figure out the angle to rotate by
	float dt = TimeManager_GetDeltaSec();
	FrameOfReference_Rotate(members->frameOfRevolution, members->axisOfRevolution, members->angularVelocity * dt);

	Matrix_GetProductVector(GO->frameOfReference->position, members->frameOfRevolution->rotation, members->startPoint);
	Vector_Increment(GO->frameOfReference->position, members->frameOfRevolution->position);
}
