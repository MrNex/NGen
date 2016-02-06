#ifndef LIGHT_H
#define LIGHT_H

#include "../Math/Vector.h"

typedef struct Light
{
	Vector* color;
	float ambientIntensity;
	float diffuseIntensity;
} Light;

///
//Allocates an uninitialized light
//
//Returns:
//	A pointer to an uninitialized light
Light* Light_Allocate(void);

///
//Initializes a light
//
//Parameters:
//	light:	A pointer to The light to initialize
//	color:	a pointer to a vector of 3 dimensions indicating the RGB color of the light
//	ambIntensity:	the ambient intensity of the light
//	difIntensity:	The diffuse intensity of the light
void Light_Initialize(Light* light, const Vector* color, const float ambIntensity, const float difIntensity);

///
//Frees resources allocated by a light
//
//Parameters:
//	light: A pointer to the light to free
void Light_Free(Light* light);

#endif
