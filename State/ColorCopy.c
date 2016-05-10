#include "ColorCopy.h"
#include "../Manager/AssetManager.h"

struct State_ColorCopy_Members
{
	Matrix* copyMatrix;
};

///
//Initializes a ColorCopy state
//
//Parameters:
//	s: A pointer to the state to initialize
//	m: The matrix to copy in color
void State_ColorCopy_Initialize(State* state, Matrix* m)
{
	struct State_ColorCopy_Members* members = (struct State_ColorCopy_Members*)malloc(sizeof(struct State_ColorCopy_Members));
	state->members = (State_Members*)members;

	members->copyMatrix = m;

	state->State_Update = State_ColorCopy_Update;
	state->State_Members_Free = State_ColorCopy_Free;
}

///
//Updates a GObject using a ColorCopy state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_ColorCopy_Update(GObject* obj, State* state)
{

	Material* material = MemoryPool_RequestAddress(assetBuffer->materialPool, obj->materialID);
	struct State_ColorCopy_Members* members = (struct State_ColorCopy_Members*)state->members;
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			*Matrix_IndexArray(material->colorMatrix, i, j, 4) = Matrix_GetIndex(members->copyMatrix, i, j);
		}
	}
}

///
//Frees memory allocated by a ColorCopy state
//
//Parameters:
//	state: The ColorCopy state to free
void State_ColorCopy_Free(State* state)
{
	free(state->members);
}
