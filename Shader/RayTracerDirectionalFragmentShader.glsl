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
vec4 CalculateDirectionalLight(vec3 worldPos, vec3 worldNormal, vec3 toViewer, vec4 materialVector, vec4 diffColor, vec4 specColor, float shadow)
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
	float diffuseFactor = -dot(normalize(worldNormal), lightDirection);
	diffuseFactor = (diffuseFactor + abs(diffuseFactor))/2.0f;

	vec4 diffuseColor = vec4(diffuseCoefficient * diffuseIntensity * diffuseFactor * lightColor * diffColor.xyz, alpha);

	//Specular
	vec3 reflection = lightDirection - 2.0f * worldNormal *  dot(lightDirection, worldNormal);
	float specularFactor = dot(reflection, toViewer);
	specularFactor = pow((specularFactor + abs(specularFactor))/2.0f, specularPower);
	vec4 specularColor = vec4(specularCoefficient * specularFactor * specColor.xyz, sAlpha);

	return ambientColor + ((1.0f - shadow) * (diffuseColor + specularColor));
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
	vec4 shadowVec = texture(shadowTexture, textureCoordinate);
	float shadow = shadowVec.r;
	//shadowVec.a = 1.0f;

	vec3 toViewer = normalize(viewPositionVector - worldPos);

	vec4 surfaceColor = CalculateDirectionalLight(worldPos, worldNormal, toViewer, materialVector, diffuseColor, specularColor, shadow);


	gl_FragColor = surfaceColor;
}
