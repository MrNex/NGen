#include "Spring.h"
#include <stdio.h>

struct State_Spring_Members
{
	Vector* restPosition;
	float k;
};

///
//Initializes a new spring state
//Any GObject with this state attached must have a rigidbody!!
//
//Parameters:
//	s: State to initialize
//	k: Spring constant (How stiff is the spring, (0.0f - 1.0f))
//	restPosition: Position where spring sits at rest
void State_Spring_Initialize(State* s, const float k, const Vector* restPosition)
{
	

	s->members = (State_Members)malloc(sizeof(struct State_Spring_Members));
	
	//Get members as a State_Revolution_Members struct
	struct State_Spring_Members* members = (struct State_Spring_Members*)s->members;

	members->restPosition = Vector_Allocate();
	Vector_Initialize(members->restPosition, 3);
	Vector_Copy(members->restPosition, restPosition);

	members->k = k;

	s->State_Members_Free = State_Spring_Free;
	s->State_Update = State_Spring_Update;
}

///
//Frees resources allocated by a spring state
//Call State_Free! Not this! State_Free will call this.
//
//Parameters:
//	s: The spring state to free
void State_Spring_Free(State* s)
{
	//Get members as a State_Revolution_Members struct
	struct State_Spring_Members* members = (struct State_Spring_Members*)s->members;

	Vector_Free(members->restPosition);
	free(members);
}

///
//Applies forces to an object's Rigidbody component to 
//simulate spring mechanics.
//
//Parameters:
//	GO: Game Object with a SpringState being updated.
//	state: The springstate updating the game object
void State_Spring_Update(GObject* GO, State* state)
{
	//Get members as a State_Revolution_Members struct
	struct State_Spring_Members* members = (struct State_Spring_Members*)state->members;

	Vector dx;
	Vector_INIT_ON_STACK(dx, 3);

	Vector_Subtract(&dx, GO->body->frame->position, members->restPosition);
	Vector_Scale(&dx, -members->k);

	RigidBody_ApplyForce(GO->body, &dx, &Vector_ZERO);

}
