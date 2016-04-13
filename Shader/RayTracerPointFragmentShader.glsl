#version 330

uniform sampler2D positionTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;
uniform sampler2D materialTexture;
uniform sampler2D specularTexture;
uniform sampler2D shadowTexture;

uniform vec3 viewPositionVector;

uniform vec2 screenSize;

uniform vec3 lightColor;
uniform vec3 lightPosition;
uniform float ambientIntensity;
uniform float diffuseIntensity;

uniform float constantAttentuation;
uniform float linearAttentuation;
uniform float exponentAttentuation;

///
//Calculates the effect of the point light on the surface drawn in this fragment
//
//Parameters:
//	worldPos: The position of the surface drawn in this fragment in worldspace
//	worldNormal: The normal of the surface in this fragment in worldspace
//	toViewer: A normalized vector pointing from the surfaces world position
//			to the cameras world position
//
//Returns:
//	A vec4 containing the effect of the point light on this surface
vec4 CalculatePointLight(vec3 worldPos, vec3 worldNormal, vec3 toViewer, vec4 materialVector, vec4 diffColor, vec4 specColor, float shadow)
{
	float ambientCoefficient = materialVector.x;
	float diffuseCoefficient = materialVector.y;
	float specularCoefficient = materialVector.z;
	float specularPower = materialVector.w;

	float alpha = diffColor.a;
	float sAlpha = specColor.a;

	//Ambient

	vec4 ambientColor = vec4(ambientCoefficient * ambientIntensity * lightColor * diffColor.rgb, alpha);
	
	//Diffuse

	vec3 lightDirection = worldPos - lightPosition;
	float distance = length(lightDirection);
	lightDirection = normalize(lightDirection);

	float diffuseFactor = -dot(worldNormal, lightDirection);
	diffuseFactor = (diffuseFactor + abs(diffuseFactor))/2.0f;

	vec4 diffuseColor = vec4(diffuseCoefficient * diffuseIntensity * diffuseFactor * lightColor * diffColor.rgb, alpha);

	//Specular
	//TODO: Add Specular Intensity to lights
	vec3 reflection = lightDirection - 2.0f * dot(lightDirection, worldNormal) * worldNormal;
	float specularFactor = dot(reflection, toViewer);
	specularFactor = pow((specularFactor + abs(specularFactor))/2.0f, specularPower);
	vec4 specularColor =  vec4(specularCoefficient * specularFactor * specColor.xyz, sAlpha);

	float attentuation = constantAttentuation + linearAttentuation * distance + exponentAttentuation * distance * distance;

	vec3 finalColor = (ambientColor.rgb + (1.0f - shadow) * (diffuseColor.rgb + specularColor.rgb));

	return vec4(finalColor, ambientColor.a + (diffuseColor.a + specularColor.a)) / attentuation;
}

///
//Calculates the position in the position, diffuse, and normal textures
//this fragment coresponds to.
//
//Returns:
//	A vec2 bound from 0-1 in any dimension representing the position of 
//	this fragment in the uniform textures
vec2 CalculateTextureCoordinate()
{
	return gl_FragCoord.xy / screenSize;
	//gl_FragCoord gives the pixel location in the window of the current fragment.
	//We divide by screen size to get normalized coordinates.
}

void main()
{
	vec2 textureCoordinate = CalculateTextureCoordinate();
	vec3 worldPos = texture(positionTexture, textureCoordinate).xyz;
	vec4 diffuseColor = texture(diffuseTexture, textureCoordinate);
	vec3 worldNormal = texture(normalTexture, textureCoordinate).xyz;
	worldNormal = normalize(worldNormal);
	vec4 materialVector = texture(materialTexture, textureCoordinate);
	vec4 specularColor = texture(specularTexture, textureCoordinate);
	float shadow = texture(shadowTexture, textureCoordinate).r;
	//float shadow = 1.0f;
	vec3 toViewer = normalize(viewPositionVector - worldPos);

	gl_FragColor = CalculatePointLight(worldPos, worldNormal, toViewer, materialVector, diffuseColor, specularColor, shadow);
}
