#version 330

uniform sampler2D localTexture;
uniform sampler2D reflectionTexture;
uniform sampler2D transmissionTexture;
uniform sampler2D globalMaterialTexture;
uniform sampler2D transMaterialTexture;

uniform vec2 screenSizeVector;

vec2 CalculateTextureCoordinate()
{
	return gl_FragCoord.xy / screenSizeVector;
}

void main()
{
	vec2 textureCoordinate = CalculateTextureCoordinate();
	vec4 localSample = texture(localTexture, textureCoordinate);
	vec4 reflectionSample = texture(reflectionTexture, textureCoordinate);
	vec4 transmissionSample = texture(transmissionTexture, textureCoordinate);
	vec3 globalMaterialSample = texture(globalMaterialTexture, textureCoordinate).xyz;
	vec3 transMaterialSample = texture(transMaterialTexture, textureCoordinate).xyz;

	vec3 finalMaterial = globalMaterialSample;// + transMaterialSample;

	gl_FragColor = finalMaterial.x * localSample + finalMaterial.y * reflectionSample + finalMaterial.z * transmissionSample;
}
