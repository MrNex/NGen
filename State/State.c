#include "State.h"
#include <stdlib.h>

///
//Allocates memory for a State
//
//Returns:
//	Pointer to newly allocated memory for a State
State* State_Allocate(void)
{
	State* state = (State*)calloc(sizeof(State), 1);
	return state;
}


///
//Frees all resources taken up by a state
//First calls State_Members_Free
//
//Parameters:
//	s: The state to free
void State_Free(State* s)
{
	s->State_Members_Free(s);
	free(s);
}

