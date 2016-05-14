float4 PhongDirectionalIllumination(__private float3 surfacePosition,__private float3 surfaceNormal, __private float shadowIntensity,
	__private float3 toViewer, __private float3 lightColor, __private float3 lightDirection, __private float2 lightIntensity,
	__private float4 material, __private float4 diffuseColor, __private float4 specularColor)
{
	
	surfaceNormal = normalize(surfaceNormal);
	lightdirection = normalize(lightDirection);

	__private float4 ambientTerm; 
	ambientTerm = (float4)(material.x * lightIntensity.x * lightColor * diffuseColor.xyz, diffuseColor.w);

	__private float4 diffuseTerm;
	__private float diffuseFactor;
	diffuseFactor = -dot(surfaceNormal, lightDirection);
	if(diffuseFactor <= 0.0f)
	{
		diffuseFactor = 0.0f;
	}
	diffuseTerm = (float4)(material.y * lightIntensity.y * diffuseFactor * lightColor * diffuseColor.xyz, diffuseColor.w);

	__private float4 specularTerm;
	__private float3 lightReflection;
	__private float specularFactor;
	lightReflection = normalize(lightDirection - 2.0f * surfaceNormal * dot(lightDirection, surfaceNormal));
	specularFactor = dot(lightReflection, toViewer);
	if(specularFactor < 0.0f)
	{
		specularFactor = 0.0f;
	}
	specularFactor = pow(specularFactor, material.w);
	specularTerm = (float4)(material.z * specularFactor * specularColor.xyz, specularColor.w);

	__private float4 finalColor;
	finalColor = ambientTerm + (1.0f - shadowIntensity) * (diffuseTerm + specularTerm);

	return finalColor;
}
