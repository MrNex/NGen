#ifndef COLORCOPY_H
#define COLORCOPY_H

#include "State.h"
#include "../GObject/GObject.h"

///
//Initializes a Color Copy state
//
//Parameters:
//	s: A pointer to the state to initialize
//	m: The matrix to copy to the object's color matrix
void State_ColorCopy_Initialize(State* s, Matrix* m);

///
//Updates a GObject using a ColorCopy state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_ColorCopy_Update(GObject* obj, State* state);

///
//Frees memory allocated by a ColorCopy state
//
//Parameters:
//	state: The Template state to free
void State_ColorCopy_Free(State* state);

#endif
