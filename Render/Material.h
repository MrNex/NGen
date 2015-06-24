#ifndef MATERIAL_H
#define MATERIAL_H

#include "../Math/Matrix.h"
#include "Texture.h"

///
//Defines a material
//A material is essentially a set of uniforms which are coupled with a texture.
typedef struct Material
{
	Matrix* colorMatrix;
	Vector* tile;

	Texture* texture;	

} Material;

///
//Allocates memory for a new material
//
//Returns:
//	A pointer to the newly allocated material
Material* Material_Allocate(void);

///
//Initializes a material
//
//Parameters:
//	mat: A pointer to the material to initialize
//	t: A pointer to the texture which this material represents an instance of
void Material_Initialize(Material* mat, Texture* t);

///
//Frees a material
//Does not free the associated texture!!
//
//Parameters:
//	mat: A pointer to the material to free
void Material_Free(Material* mat);

#endif
