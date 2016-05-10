struct TransmissionRay
{
	float3 direction;
	float3 pos;
	bool TIR;
};

struct TransmissionRay CalculateTransmissionRaySphere(float4 sphere, float indexOfRefraction, float3 surfacePosition, float3 surfaceNormal, float3 toViewer)
{
	
	__private struct TransmissionRay transmissionRay;//First 3 components are position, second 3 are direction
	
	__private float3 incidentRay;
	__private float iAngle;
	__private float alpha;	//ratio of indices of refraction
	__private float beta, discr;
	
	incidentRay = -toViewer;
	
	iAngle = -dot(incidentRay, surfaceNormal);



	alpha = 1.0f / indexOfRefraction;

	discr = 1.0f + (alpha * alpha * (iAngle * iAngle - 1.0f));

	__private bool totalInternalReflection;
	totalInternalReflection = (discr < 0.0f);

	__private float negate;
	negate = 1.0f - 2.0f * totalInternalReflection;
	discr = discr * negate;

	beta = alpha * iAngle - sqrt(discr);

	__private float3 innerRayDirection;
	innerRayDirection = alpha * incidentRay + beta * surfaceNormal;
	innerRayDirection = normalize(innerRayDirection * (!totalInternalReflection));

	//;Determine exit point on sphere
	__private float scaleFactor;
	scaleFactor = dot(innerRayDirection, sphere.xyz - surfacePosition) * 2.0f;
	transmissionRay.pos = surfacePosition + innerRayDirection * scaleFactor;

	__private float3 exitNormal;
	exitNormal = normalize(transmissionRay.pos - sphere.xyz);

	//;Determine direction of transmission ray
	__private float eAngle, eAlpha, eBeta, eDiscr;

	eAngle = dot(innerRayDirection, exitNormal);
	eAlpha = indexOfRefraction;
	eDiscr = 1.0f + (eAlpha * eAlpha * (eAngle * eAngle - 1.0f));
	
	__private bool tirFinal;
	tirFinal = (eDiscr < 0.0f);

	negate = 1.0f - 2.0f * tirFinal;
	tirFinal = (tirFinal || totalInternalReflection);
	eDiscr = eDiscr * negate;

	eBeta = eAlpha * eAngle - sqrt(eDiscr);

	transmissionRay.direction = alpha * innerRayDirection + beta * exitNormal;
	transmissionRay.direction = normalize(transmissionRay.direction * (!tirFinal));
	transmissionRay.TIR = tirFinal;
	
	return transmissionRay;
}
