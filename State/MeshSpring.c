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


#include "MeshSpring.h"

#include "../Manager/TimeManager.h"
#include "../Manager/InputManager.h"

#include "../Data/DynamicArray.h"

#include <stdio.h>
#include <math.h>


struct MeshSpringState_Node
{
	Vector vertex;
	Vector* velocity;

	Vector* neighbors[6];
	unsigned int numNeighbors;

	unsigned char isAnchor;

};

struct State_MeshSpring_Members
{
	struct MeshSpringState_Node* nodes;
	unsigned int numNodes;

	unsigned int gridWidth;
	unsigned int gridHeight;
	unsigned int gridDepth;

	float springConstant;
	float dampingCoefficient;
};



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
void State_MeshSpringState_Initialize(State* state, Mesh* grid, unsigned int gridWidth, unsigned int gridHeight, unsigned int gridDepth, float springConstant, float dampingCoefficient, int anchorDimensions)
{
	state->State_Members_Free = State_MeshSpringState_Free;
	state->State_Update = State_MeshSpringState_Update;

	state->members = (State_Members)malloc(sizeof(struct State_MeshSpring_Members));

	//Get members
	struct State_MeshSpring_Members* members = (struct State_MeshSpring_Members*)state->members;

	//Assign grid dimensions
	members->gridWidth = gridWidth;
	members->gridHeight = gridHeight;
	members->gridDepth = gridDepth;

	//TODO: Remove hardcoding
	//Set spring constant
	members->springConstant = springConstant;
	members->dampingCoefficient = dampingCoefficient;

	//Create array of nodes
	members->nodes = (struct MeshSpringState_Node*)malloc(sizeof(struct MeshSpringState_Node) * gridWidth * gridHeight * gridDepth);
	members->numNodes = 0;


	int anchors = 0;

	//Create each node in the grid
	for(unsigned int k = 0; k < gridDepth; k++)
	{
		for(unsigned int j = 0; j < gridHeight; j++)
		{
			for(unsigned int i = 0; i < gridWidth; i++)
			{
				unsigned int nodeIndex = i + j * gridWidth + k * gridWidth * gridHeight;
				members->nodes[nodeIndex].vertex.dimension = 3;
				members->nodes[nodeIndex].vertex.components = (float*)(((struct Vertex*)grid->triangles) + nodeIndex);

				members->numNodes++;

				//Check if node is on an edge
				int isBounds = 0;
				if(i == 0 || i == gridWidth - 1) isBounds++;
				if(j == 0 || j == gridHeight - 1) isBounds++;
				if(k == 0 || k == gridDepth - 1) isBounds++;

				//If so, anchor it!
				if(isBounds > 3 - anchorDimensions)
				{
					anchors++;
					members->nodes[nodeIndex].isAnchor = 1;
				}
				//Else, give it a velocity
				else
				{
					members->nodes[nodeIndex].isAnchor = 0;
					members->nodes[nodeIndex].velocity = Vector_Allocate();
					Vector_Initialize(members->nodes[nodeIndex].velocity, 3);

				}



				//initialize neighbors to 0
				for(int n = 0; n < 6; n++)
				{
					members->nodes[nodeIndex].neighbors[n] = 0;
				}
			}
		}

		printf("Number of nodes:\t%d\nNumber of anchors:\t%d\n", members->numNodes, anchors);
	}


	//Connect node to neighbor nodes
	for(unsigned int k = 0; k < gridDepth; k++)
	{
		for(unsigned int j = 0; j < gridHeight; j++)
		{
			for(unsigned int i = 0; i < gridWidth; i++)
			{
				unsigned int nodeIndex = i + j * gridWidth + k * gridWidth * gridHeight;

				//If this node is not an anchor
				if(!members->nodes[nodeIndex].isAnchor)
				{
					int numNeighbors = 0;
					//Can it have a left neighbor?
					if(i > 0)
					{
						//If so grab it! and assign it!
						unsigned int neighborIndex = (i-1) + j * gridWidth + k * gridWidth * gridHeight;
						struct MeshSpringState_Node* neighbor = members->nodes + neighborIndex;
						Vector* neighborPosition = &(neighbor->vertex);
						members->nodes[nodeIndex].neighbors[numNeighbors] = neighborPosition;

						//Increment the number of neighbors
						numNeighbors++;
					}

					//Can it have a right neighbor?
					if(i < gridWidth - 1)
					{
						//If so grab it! and assign it!
						unsigned int neighborIndex = (i+1) + j * gridWidth + k * gridWidth * gridHeight;
						struct MeshSpringState_Node* neighbor = members->nodes + neighborIndex;
						Vector* neighborPosition = &(neighbor->vertex);
						members->nodes[nodeIndex].neighbors[numNeighbors] = neighborPosition;

						//Increment the number of neighbors
						numNeighbors++;
					}

					//Can it have a bottom neighbor?
					if(j > 0)
					{
						//If so grab it! and assign it!
						unsigned int neighborIndex = i + (j-1) * gridWidth + k * gridWidth * gridHeight;
						struct MeshSpringState_Node* neighbor = members->nodes + neighborIndex;
						Vector* neighborPosition = &(neighbor->vertex);
						members->nodes[nodeIndex].neighbors[numNeighbors] = neighborPosition;

						//Increment the number of neighbors
						numNeighbors++;
					}

					//Can it have a top neighbor?
					if(j < gridHeight - 1)
					{
						//If so grab it! and assign it!
						unsigned int neighborIndex = i + (j+1) * gridWidth + k * gridWidth * gridHeight;
						struct MeshSpringState_Node* neighbor = members->nodes + neighborIndex;
						Vector* neighborPosition = &(neighbor->vertex);
						members->nodes[nodeIndex].neighbors[numNeighbors] = neighborPosition;

						//Increment the number of neighbors
						numNeighbors++;
					}

					//Can it have a back neighbor?
					if(k > 0)
					{
						//If so grab it! and assign it!
						unsigned int neighborIndex = i + j * gridWidth + (k-1) * gridWidth * gridHeight;
						struct MeshSpringState_Node* neighbor = members->nodes + neighborIndex;
						Vector* neighborPosition = &(neighbor->vertex);
						members->nodes[nodeIndex].neighbors[numNeighbors] = neighborPosition;

						//Increment the number of neighbors
						numNeighbors++;
					}

					//Can it have a front neighbor?
					if(k < gridDepth - 1)
					{
						//If so grab it! and assign it!
						unsigned int neighborIndex = i + j * gridWidth + (k+1) * gridWidth * gridHeight;
						struct MeshSpringState_Node* neighbor = members->nodes + neighborIndex;
						Vector* neighborPosition = &(neighbor->vertex);
						members->nodes[nodeIndex].neighbors[numNeighbors] = neighborPosition;

						//Increment the number of neighbors
						numNeighbors++;
					}

					members->nodes[nodeIndex].numNeighbors = numNeighbors;
				}
			}
		}
	}
}

///
//Frees resources allocated by a Mesh Spring state
//
//Parameters:
//	s: The state to free
void State_MeshSpringState_Free(State* state)
{
	//Get members
	struct State_MeshSpring_Members* members = (struct State_MeshSpring_Members*)state->members;

	//For each node
	for(unsigned int i = 0; i < members->numNodes; i++)
	{
		//Free the nodes velocity
		Vector_Free(members->nodes[i].velocity);
	}
	//Free the list of nodes
	free(members->nodes);

	//Free the state's members
	free(members);
}

///
//Updates the mesh spring state
//
//Parameters:
//	GO: The game object this state is attached to
//	state: The state updating this gameObject
void State_MeshSpringState_Update(GObject* GO, State* state)
{
	//GO will be unused.
	(void)GO;


	//Get members
	struct State_MeshSpring_Members* members = (struct State_MeshSpring_Members*)state->members;

	//Get the change in second
	float dt = TimeManager_GetDeltaSec();

	//For each node
	for(unsigned int i = 0; i < members->numNodes; i++)
	{
		unsigned int nodeIndex = i;
		struct MeshSpringState_Node* current = members->nodes + nodeIndex;

		unsigned int middleNodeIndex = members->gridWidth /2 + members->gridWidth * members->gridHeight/2;

		//If the node is not an anchor
		if(!current->isAnchor)
		{

			//Compute the net force the neighbors are having upon it
			Vector netForce;
			Vector_INIT_ON_STACK(netForce, 3);

			Vector currForce;
			Vector_INIT_ON_STACK(currForce, 3);

			for(unsigned int j = 0; j < current->numNeighbors; j++)
			{
				//Get current force from this neighbor
				Vector_Subtract(&currForce, &current->vertex, current->neighbors[j]);

				Vector_Scale(&currForce, members->springConstant * (-1.0f));



				//Increment net force by curr force
				Vector_Increment(&netForce, &currForce);
			}

			//Apply damping force
			Vector_GetScalarProduct(&currForce, current->velocity, -members->dampingCoefficient);
			//Increment net force by damping force
			Vector_Increment(&netForce, &currForce);

			//If the current node is in the bottom row && it is not an anchor
			if(i > members->gridWidth && i < members->gridWidth * 2)
			{
				if(!current->isAnchor)
				{
					//If spacebar is pressed
					if(InputManager_IsKeyDown('k'))
					{

						//Apply a force on the positive Z axis
						Vector_GetScalarProduct(&currForce, &Vector_E3, 5.0f);

						Vector_Increment(&netForce, &currForce);
					}
				}
			}
			
			//If the current node is a central node
			if(i == middleNodeIndex || i + 1 == middleNodeIndex || i - 1 == middleNodeIndex || 
				i + members->gridWidth == middleNodeIndex || i - members->gridWidth == middleNodeIndex)
			{
				if(!current->isAnchor)
				{
					//If spacebar is pressed
					if(InputManager_IsKeyDown('i'))
					{

						//Apply a force on the positive Z axis
						Vector_GetScalarProduct(&currForce, &Vector_E3, 10.0f);

						Vector_Increment(&netForce, &currForce);
					}
				}
			}

			//If the current node is on the back face
			if (i < members->gridWidth * members->gridHeight)
			{
				if (!current->isAnchor)
				{
					//If spacebar is pressed
					if (InputManager_IsKeyDown('j'))
					{

						//Apply a force on the positive Z axis
						Vector_GetScalarProduct(&currForce, &Vector_E3, -10.0f);

						Vector_Increment(&netForce, &currForce);
					}
				}

			}
			//Acceleration = Force because mass is negligible
			//Find velocity using V = AT
			Vector_Scale(&netForce, dt);
			//Increment velocity
			Vector_Increment(current->velocity, &netForce);

			//Find dX = VT
			Vector_GetScalarProduct(&netForce, current->velocity, dt);

			//Increment position
			Vector_Increment(&current->vertex, &netForce);



		}
	}
}

