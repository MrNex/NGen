#include "Material.h"

///
//Allocates memory for a new material
//
//Returns:
//	A pointer to the newly allocated material
Material* Material_Allocate(void)
{
	Material* mat = (Material*)malloc(sizeof(Material));
	return mat;
}

///
//Initializes a material
//
//Parameters:
//	mat: A pointer to the material to initialize
//	t: A pointer to the texture which this material represents an instance of
void Material_Initialize(Material* mat, Texture* t)
{
	//Allocate & Initialize color matrix
	mat->colorMatrix = Matrix_Allocate();
	Matrix_Initialize(mat->colorMatrix, 4, 4);

	//Allocate & initialize tile vector
	mat->tile = Vector_Allocate();
	Vector_Initialize(mat->tile, 2);

	mat->tile->components[0] = mat->tile->components[1] = 1.0f;

	mat->texture = t;

	mat->specularIntensity = 0.0f;
	mat->specularPower = 0.0f;
}

///
//Initializes a deep copy of a Material
//This means any pointers will point to Newly Allocated instances of identical memory
//NOTE: Does not deep copy texture (points to same location as original)
//
//Parameters:
//	copy: A pointer to an uninitialized material to initialize as a deep copy
//	original: A pointer to a material to copy
void Material_InitializeDeepCopy(Material* copy, Material* original)
{
	Material_Initialize(copy, original->texture);
	
	Matrix_Copy(copy->colorMatrix, original->colorMatrix);
	Vector_Copy(copy->tile, original->tile);

	copy->specularPower = original->specularPower;
	copy->specularIntensity = original->specularIntensity;
}

///
//Frees a material
//Does not free the associated texture!!
//
//Parameters:
//	mat: A pointer to the material to free
void Material_Free(Material* mat)
{
	Matrix_Free(mat->colorMatrix);
	Vector_Free(mat->tile);

	free(mat);
}


