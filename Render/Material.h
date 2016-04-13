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
	Vector* specularColor;

	Texture* texture;	

	float ambientCoefficient;
	float diffuseCoefficient;
	float specularCoefficient;

	float specularPower;

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
//Initializes a deep copy of a Material
//This means any pointers will point to Newly Allocated instances of identical memory
//NOTE: Does not deep copy texture (points to same location as original)
//
//Parameters:
//	copy: A pointer to an uninitialized material to initialize as a deep copy
//	original: A pointer to a material to copy
void Material_InitializeDeepCopy(Material* copy, Material* original);

///
//Frees a material
//Does not free the associated texture!!
//
//Parameters:
//	mat: A pointer to the material to free
void Material_Free(Material* mat);

#endif
