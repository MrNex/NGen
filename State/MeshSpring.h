#ifndef STATE_MESHSPRING_H
#define STATE_MESHSPRING_H

#include "State.h"
#include "Mesh.h"

///
//Initializes a mesh spring state
//
//Parameters:
//	state: The state to initialize
//	mesh: The grid mesh whose vertices are acting like springs
//	gridWidth: The number of vertices along the width of the grid
//	gridHeight: The number of vertices along the height of the grid
//	gridDepth: The number of vertices along the depth of the grid
//	springConstant: The value of the spring constant to use in the spring simulation between vertices
//	dampingCoefficient: The value of the damping coefficient to use int ehs pring simulation between vertices
void State_MeshSpringState_Initialize(State* state, Mesh* grid, unsigned int gridWidth, unsigned int gridHeight, unsigned int gridDepth, float springconstant, float dampingCoefficient, int anchorDimensions);

///
//Frees resources allocated by a first person camera state
//
//Parameters:
//	s: The state to free
void State_MeshSpringState_Free(State* state);

///
//Updates the mesh spring state
//
//Parameters:
//	GO: The game object this state is attached to
//	state: The state updating this gameObject
void State_MeshSpringState_Update(GObject* GO, State* state);

#endif
