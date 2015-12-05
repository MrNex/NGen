#version 330
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec2 in_textureCoordinates;
layout (location = 2) in vec3 in_normal;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 modelViewProjectionMatrix;

uniform vec3 directionalLightVector;

uniform sampler2D textureDiffuse;

//To the fragment shader
out vec2 f_textureCoordinates;
out vec3 f_normal;

void main()
{
	//Finally got it to work.
	//gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(in_position, 1.0f);	
	gl_Position = modelViewProjectionMatrix * vec4(in_position, 1.0f);

	//Preparing for Fragment shader
	f_normal = normalize(mat3(modelMatrix) * in_normal);
	f_textureCoordinates = in_textureCoordinates;
}
