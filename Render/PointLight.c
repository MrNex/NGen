#include "PointLight.h"

#include <math.h>

///
//Allocates an uninitialized point light
//
//Returns:
//	A pointer to an uninitialized point light
PointLight* PointLight_Allocate(void)
{
	return malloc(sizeof(PointLight));
}

///
//Initializes a point light
//
//Parameters:
//	light:	A pointer to The pointlight to initialize
//	color:	a pointer to a vector of 3 dimensions indicating the RGB color of the light
//	ambIntensity:	the ambient intensity of the light
//	difIntensity:	The diffuse intensity of the light
//	cAtt: The constant attentuation term
//	lAtt: The linear attentuation term
//	eAtt: The exponential attentuation term
void PointLight_Initialize(PointLight* light, const Vector* position, const Vector* color, const float ambIntensity, const float difIntensity, const float cAtt, const float lAtt, const float eAtt)
{
	light->base = Light_Allocate();
	Light_Initialize(light->base, color, ambIntensity, difIntensity);

	light->position = Vector_Allocate();
	Vector_Initialize(light->position, 3);
	Vector_Copy(light->position, position);

	light->attentuation.constant = cAtt;
	light->attentuation.linear = lAtt;
	light->attentuation.exponent = eAtt;
}

///
//Frees resources allocated by a point light
//
//Parameters:
//	light: A pointer to the point light to free
void PointLight_Free(PointLight* light)
{
	Light_Free(light->base);
	free(light);
}

///
//Calculates the radius of effect of a point light
//
//Paramters:
//	light: A pointer to a point light to find the radius of effect of.
//
//Returns:
//	The radius of effect of a given light as a float
float PointLight_CalculateRadius(PointLight* light)
{
	float maxChannel = light->base->color->components[0] > light->base->color->components[1] ? light->base->color->components[0] : light->base->color->components[1];
	maxChannel = maxChannel > light->base->color->components[2] ? maxChannel : light->base->color->components[2];


	float a = light->attentuation.exponent;
	float b = light->attentuation.linear;
	float c = light->attentuation.constant - 256.0f * maxChannel * light->base->diffuseIntensity;

	return (-b + sqrtf(b * b - 4.0f *a * c)) / (2.0f * a);
}


