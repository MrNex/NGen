#ifndef POINTLIGHT_H
#define POINTLIGHT_H

#include "Light.h"

struct Attentuation
{
	float constant;
	float linear;
	float exponent;
};

typedef struct PointLight
{
	Light* base;
	Vector* position;
	struct Attentuation attentuation;
} PointLight;

///
//Allocates an uninitialized point light
//
//Returns:
//	A pointer to an uninitialized point light
PointLight* PointLight_Allocate(void);

///
//Initializes a point light
//
//Parameters:
//	light:	A pointer to The pointlight to initialize
//	position:	A pointer to a vector of 3 dimensions which will be copied containing the position of this light source
//	color:	a pointer to a vector of 3 dimensions indicating the RGB color of the light
//	ambIntensity:	the ambient intensity of the light
//	difIntensity:	The diffuse intensity of the light
//	cAtt: The constant attentuation term
//	lAtt: the linear attentuation term
//	eAtt: The exponential attentuation term
void PointLight_Initialize(PointLight* light, const Vector* position, const Vector* color, const float ambIntensity, const float difIntensity, const float cAtt, const float lAtt, const float eAtt);

///
//Frees resources allocated by a point light
//
//Parameters:
//	light: A pointer to the point light to free
void PointLight_Free(PointLight* light);

///
//Calculates the radius of effect of a point light
//
//Paramters:
//	light: A pointer to a point light to find the radius of effect of.
//
//Returns:
//	The radius of effect of a given light as a float
float PointLight_CalculateRadius(PointLight* light);

#endif
