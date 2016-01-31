#include "DirectionalLight.h"

///
//Allocates an uninitialized DirectionalLight
//
//Returns:
//	A pointer to an uninitialized DirectionalLight
DirectionalLight* DirectionalLight_Allocate(void)
{
	return malloc(sizeof(DirectionalLight));
}

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
		float aIntensity, float dIntensity)
{
	light->color = Vector_Allocate();
	Vector_Initialize(light->color, 3);
	Vector_Copy(light->color, color);

	light->direction = Vector_Allocate();
	Vector_Initialize(light->direction, 3);
	Vector_Copy(light->direction, dir);

	light->ambientIntensity = aIntensity;
	light->diffuseIntensity = dIntensity;
}

///
//Frees a directional light
//
//Parameters:
//	light: A pointer to the directional light being freed
void DirectionalLight_Free(DirectionalLight* light)
{
	Vector_Free(light->color);
	Vector_Free(light->direction);
	free(light);
}
