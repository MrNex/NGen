#include "Material.h"

#include "../Manager/AssetManager.h"

///
//Allocates memory for a new material
//
//Returns:
//	ID of newly allocated material
unsigned int Material_Allocate(void)
{
	unsigned int mID = MemoryPool_RequestID(assetBuffer->materialPool);
	
	//Material* mat = (Material*)malloc(sizeof(Material));
	return mID;
}

///
//Initializes a material
//
//Parameters:
//	mat: A pointer to the material to initialize
//	t: A pointer to the texture which this material represents an instance of
/*
void Material_Initialize(Material* mat, Texture* t)
{
	//Allocate & Initialize color matrix
	mat->colorMatrix = Matrix_Allocate();
	Matrix_Initialize(mat->colorMatrix, 4, 4);

	//Allocate & initialize tile vector
	mat->tile = Vector_Allocate();
	Vector_Initialize(mat->tile, 2);

	mat->specularColor = Vector_Allocate();
	Vector_Initialize(mat->specularColor, 3);

	mat->specularColor->components[0] = mat->specularColor->components[1] = mat->specularColor->components[2] = 1.0f;

	mat->tile->components[0] = mat->tile->components[1] = 1.0f;

	mat->texture = t;

	mat->specularPower = 1.0f;
	mat->specularCoefficient = 0.2f;
	mat->diffuseCoefficient = 0.6f;
	mat->ambientCoefficient = 0.2f;
}
*/

///
//Initializes a material
//
//Parameters:
//	mat: A pointer to the material to initialize
//	t: A pointer to the texture which this material represents an instance of
//	tPoolID: The id of this texture in the AssetMAnager_texturePool
void Material_Initialize(unsigned int mID, unsigned int tPoolID)
{
	Material* mat = MemoryPool_RequestAddress(assetBuffer->materialPool, mID);

	//Allocate & Initialize color matrix
	memset(mat->colorMatrix, 0, sizeof(mat->colorMatrix));
	mat->colorMatrix[0] = 1.0f;
	mat->colorMatrix[5] = 1.0f;
	mat->colorMatrix[10] = 1.0f;
	mat->colorMatrix[15] = 1.0f;

	//Allocate & initialize tile vector
	mat->tile[0] = mat->tile[1] = 1.0f;

	mat->specularColor[0] = mat->specularColor[1] = mat->specularColor[2] = mat->specularColor[3] = 1.0f;
	
	mat->texturePoolID = tPoolID;

	mat->specularPower = 1.0f;
	mat->specularCoefficient = 0.2f;
	mat->diffuseCoefficient = 0.6f;
	mat->ambientCoefficient = 0.2f;

	mat->localCoefficient = 0.8f;
	mat->reflectedCoefficient = 0.2f;
	mat->transmittedCoefficient = 0.0f;
	mat->indexOfRefraction = 1.0f;
}



///
//Frees a material
//Does not free the associated texture!!
//
//Parameters:
//	mID: the ID of the material to free
void Material_Free(unsigned int mID)
{
	//free(mat);
	MemoryPool_ReleaseID(assetBuffer->materialPool, mID);
}


