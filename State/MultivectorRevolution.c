#if defined __linux__

//Enable POSIX definitions (for timespec)
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif
//#define _POSIX_C_SOURCE 1

#include <time.h>

#endif 

#include "MultivectorRevolution.h"

#include <math.h>

#include "../Manager/TimeManager.h"

#include "../Math/Multivector.h"

struct State_MultivectorRevolution_Members
{
	float angularVelocity;
};


///
//Initializes a MultivectorRevolution state
//
//Parameters:
//	s: A pointer to the state to initialize
void State_MultivectorRevolution_Initialize(State* s, float v)
{
	struct State_MultivectorRevolution_Members* mems = malloc(sizeof(struct State_MultivectorRevolution_Members));
	s->members = mems;
	mems->angularVelocity = v;

	s->State_Update = State_MultivectorRevolution_Update;
	s->State_Members_Free = State_MultivectorRevolution_Free;
}

///
//Updates a GObject using a MultivectorRevolution state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_MultivectorRevolution_Update(GObject* obj, State* state)
{
	struct State_MultivectorRevolution_Members* mems = state->members;

	float hAngle = mems->angularVelocity * 0.5f;

	Vector* objPos = obj->frameOfReference->position;

	float position[(1 << 3)] = { 
		0.0f,		//e0
		objPos->components[0],
		objPos->components[1],
		0.0f,		//e12
		objPos->components[2],
		0.0f,		//e13
		0.0f,		//e23
		0.0f		//e123
	};
	float rotor[(1 << 3)] = { 
		cosf(hAngle),	//e0
		0.0f, 		//e1
		0.0f, 		//e2
		0.0f,		//e12
		0.0f,		//e3
		sinf(hAngle),	//e13
		0.0f,		//e23
		0.0f		//e123
	};
	float complement[(1 << 3)] = { 
		cosf(hAngle),	//e0
		0.0f, 		//e1
		0.0f, 		//e2
		0.0f,		//e12
		0.0f,		//e3
		-sinf(hAngle),	//e13
		0.0f,		//e23
		0.0f		//e123
	};

	float dest[(1 << 3)] = {0.0f};

	Multivector_GetProduct(dest, 3, complement, 3, position, 3);
	Multivector_GetProduct(position, 3, dest, 3, rotor, 3);

	Vector newPos;
	Vector_INIT_ON_STACK(newPos, 3);
	newPos.components[0] = position[1];
	newPos.components[1] = position[2];
	newPos.components[2] = position[4];

	GObject_SetPosition(obj, &newPos);
}

///
//Frees memory allocated by a MultivectorRevolution state
//
//Parameters:
//	state: The MultivectorRevolution state to free
void State_MultivectorRevolution_Free(State* state)
{
	free(state->members);
}
