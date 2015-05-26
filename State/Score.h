#ifndef STATE_SCORE_H
#define STATE_SCORE_H

#include "State.h"
#include "../GObject/GObject.h"

static int score = 0;

///
//Initializes a new score state
//
//Parameters:
//	state: The state to initialize as a score state
//	increment: The amount of points to add per hit
//	timePerIncrement: The amount of seconds you must wait before scoring on this target again
void State_Score_Initialize(State* state, const int increment, const float timePerIncrement);

///
//Frees resources allocated by a score state
//
//Parameters:
//	state: A pointer to the state to free
void State_Score_Free(State* state);

///
//Checks if the attached object was in a collision with a bullet, then increments & prints the score if so
//
//parameters:
//	GO: a pointer to The game object with an attached score state being updated
//	state: a pointer to the score state updating the attached game object
void State_Score_Update(GObject* GO, State* state);

#endif
