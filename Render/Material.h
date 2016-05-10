#ifndef MATERIAL_H
#define MATERIAL_H

#include "../Math/Matrix.h"
#include "Texture.h"

/*
typedef struct Material_ColorMatrix
{
	float[16] colorMatrix;
}

typedef union Material_Specular
{
	struct Material_SpecularProperties specular;
	float[4] specularVector;
};

struct Material_SpecularProperties
{
	float r;
	float g;
	float b;
	float a;
};

typedef union Material_Local
{
	struct Material_LocalProperties local;
	float[4] localVector;
} Material_Local;

struct Material_LocalProperties
{
	float ambientCoefficient;
	float diffuseCoefficient;
	float specularCoefficient;
	float specularPower;
};

typedef union Material_Global
{
	struct Material_GlobalProperties global;
	float[4] globalVector;
};
struct Material_GlobalProperties
{
	float localCoefficient;
	float reflectionCoefficient;
	float transmissionCoefficient;
	unsigned int texturePoolID;
};
*/

///
//Defines a material
//A material is essentially a set of uniforms which are coupled with a texture.
/*
typedef struct MaterialOLD
{
	Matrix* colorMatrix;
	Vector* tile;
	Vector* specularColor;

	//Texture* texture;	
	unsigned int texturePoolID;

	float ambientCoefficient;
	float diffuseCoefficient;
	float specularCoefficient;

	float specularPower;

} Material;
*/


typedef struct Material
{

	float colorMatrix[16];
	float specularColor[4];
	float tile[2];	
	
	float ambientCoefficient;
	float diffuseCoefficient;
	float specularCoefficient;
	float specularPower;

	float localCoefficient;
	float reflectedCoefficient;
	float transmittedCoefficient;

	unsigned int texturePoolID;	

	float indexOfRefraction;
	int padA;	//OpenCL requires 16 byte alignment of structs
} Material;


///
//Allocates memory for a new material
//
//Returns:
//	ID of newly allocated material
unsigned int Material_Allocate(void);

///
//Initializes a material
//
//Parameters:
//	mat: A pointer to the material to initialize
//	t: A pointer to the texture which this material represents an instance of
//void Material_Initialize(Material* mat, Texture* t);
//	tPoolID: The id of this texture in the AssetMAnager_texturePool
void Material_Initialize(unsigned int mID, unsigned int tPoolID);

///
//Frees a material
//Does not free the associated texture!!
//
//Parameters:
//	mID: the ID of the material to free
void Material_Free(unsigned int mID);

#endif
