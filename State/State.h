#ifndef STATE_H
#define STATE_H

//Forward declaration for object (Else circular dependency problem State <-> Object
typedef struct GObject GObject;
//struct State_Members;
typedef void* State_Members;

typedef struct State
{
	void(*State_Update)(GObject*, struct State*);
	void(*State_Members_Free)(struct State* s);

	//struct State_Members* members;
	State_Members members;
} State;

///
//Allocates memory for a State
//
//Returns:
//	Pointer to newly allocated memory for a State
State* State_Allocate(void);

///
//Frees all resources taken up by a state
//First calls State_Members_Free
//
//Parameters:
//	s: The state to free
void State_Free(State* s);



#endif

