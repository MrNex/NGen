#include "Light.h"

#include <stdlib.h>

///
//Allocates an uninitialized light
//
//Returns:
//	A pointer to an uninitialized light
Light* Light_Allocate(void)
{
	return malloc(sizeof(Light));
}

///
//Initializes a light
//
//Parameters:
//	light:	A pointer to The light to initialize
//	color:	a pointer to a vector of 3 dimensions indicating the RGB color of the light
//	ambIntensity:	the ambient intensity of the light
//	difIntensity:	The diffuse intensity of the light
void Light_Initialize(Light* light, const Vector* color, const float ambIntensity, const float difIntensity)
{

	light->color = Vector_Allocate();
	Vector_Initialize(light->color, 3);
	Vector_Copy(light->color, color);

	light->ambientIntensity = ambIntensity;
	light->diffuseIntensity = difIntensity;

}

///
//Frees resources allocated by a light
//
//Parameters:
//	light: A pointer to the light to free
void Light_Free(Light* light)
{
	Vector_Free(light->color);
	free(light);
}


