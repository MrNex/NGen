#version 330
uniform sampler2D textureDiffuse;

in vec2 f_textureCoordinates;
in vec3 f_normal;

uniform mat4 colorMatrix;
uniform vec3 directionalLightVector;

void main()
{
	float lightIntensity = 	dot(f_normal, directionalLightVector);
	lightIntensity *= -0.5f;
	lightIntensity += 0.5f;
	//lightIntensity *= 0.5f;

	vec4 result = texture2D(textureDiffuse, f_textureCoordinates);

	gl_FragColor = lightIntensity * colorMatrix * result;
}


