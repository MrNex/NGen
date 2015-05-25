#ifndef STATE_MESHSWAP_H
#define STATE_MESHSWAP_H

#include "State.h"
#include "GObject.h"

///
//Initializes a mesh swap state
//
//Parameters:
//	s: The state to initialize
void State_MeshSwap_Initialize(State* s);

///
//Frees memory used by a Mesh Swap state
//
//Parameters:
//	s: The mesh swap state to free
void State_MeshSwap_Free(State* s);

///
//Updates a mesh swap state's attached gameObject.
//Switches the mesh of the gameObject based on the current input.
//
//Parameters:
//	GO: The gameObject with a MeshSwapState being updated
//	state: The MeshSwap state updating the gameobject
void State_MeshSwap_Update(GObject* GO, State* state);

#endif
