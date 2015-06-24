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


