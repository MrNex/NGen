#include "Template.h"

/* Commented out to avoid empty struct warning
struct State_Template_Members
{
	//Insert members here
};
*/

///
//Initializes a Template state
//
//Parameters:
//	s: A pointer to the state to initialize
void State_Template_Initialize(State* state)
{
	//State is currently unused.
	(void)state;
}

///
//Updates a GObject using a Template state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_Template_Update(GObject* obj, State* state)
{
	//state and obj are currently unused
	(void)obj;
	(void)state;
}

///
//Frees memory allocated by a Template state
//
//Parameters:
//	state: The Template state to free
void State_Template_Free(State* state)
{
	//State is currently unused
	(void)state;
}
