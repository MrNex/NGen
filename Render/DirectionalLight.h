#ifndef DIRECTIONALLIGHT_H
#define DIRECTIONALLIGHT_H

#include "Light.h"

typedef struct DirectionalLight
{
	Light* base;
	Vector* direction;
}DirectionalLight;

///
//Allocates an uninitialized DirectionalLight
//
//Returns:
//	A pointer to an uninitialized DirectionalLight
DirectionalLight* DirectionalLight_Allocate(void);

///
//Initializes a directional light
//
//Parameters:
//	light: A pointer to the directional light being initialized
//	color: a pointer to a  normalized vector of 3 dimensions which will be copied indicating the color of the light
//	dir: a pointer to a normalized vector of 3 dimensions which will be copied indicating the direction of the light.
//	aIntensity: A float indicating the intensity of the ambient light produced by this light.
//	dIntensity: a float indicating the intensity of the diffuse light produced by this light.
void DirectionalLight_Initialize(DirectionalLight* light, Vector* color, Vector* dir,
		float aIntensity, float dIntensity);

///
//Frees a directional light
//
//Parameters:
//	light: A pointer to the directional light being freed
void DirectionalLight_Free(DirectionalLight* light);

#endif
