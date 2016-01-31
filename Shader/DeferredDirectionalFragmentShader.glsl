#version 330

uniform sampler2D positionTexture;
uniform sampler2D diffuseTexture;
uniform sampler2D normalTexture;

uniform vec2 screenSize;

uniform vec3 lightColor;
uniform vec3 lightDirection;
uniform float ambientIntensity;
uniform float diffuseIntensity;

///
//Calculates the effect of the directional light on the surface drawn in this fragment.
//
//Parameters:
//	worldPos: The position of the surface drawn in this fragment in worldspace
//	worldNormal: The normal of the surface in this fragment in worldspace
//
//Returns:
//
vec4 CalculateDirectionalLight(vec3 worldPos, vec3 worldNormal)
{
	vec4 ambientColor = vec4(ambientIntensity * lightColor, 1.0f);

	float diffuseFactor = -dot(normalize(worldNormal), lightDirection);
	diffuseFactor = (diffuseFactor + abs(diffuseFactor))/2.0f;

	vec4 diffuseColor = vec4(diffuseIntensity * diffuseFactor * lightColor, 1.0f);
	return ambientColor + diffuseColor;
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
	vec3 diffuseColor = texture(diffuseTexture, textureCoordinate).xyz;
	vec3 worldNormal = texture(normalTexture, textureCoordinate).xyz;
	worldNormal = normalize(worldNormal);

	gl_FragColor = vec4(diffuseColor, 1.0f) * CalculateDirectionalLight(worldPos, worldNormal);
	//gl_FragColor = vec4(worldNormal, 1.0f);
}
