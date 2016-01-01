#version 330

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec2 v_textureCoordinates;
layout (location = 2) in vec3 v_normal;

uniform mat4 modelMatrix;
uniform mat4 modelViewProjectionMatrix;

out vec3 f_worldPosition;
out vec2 f_textureCoordinates;
out vec3 f_normal;

void main()
{
	gl_position = modelViewProjectionMatrix * vec4(v_position, 1.0f);
	f_worldPosition = modelMatrix * vec4(v_position, 1.0f);
	f_textureCoordinates = v_textureCoordinates;
	f_normal = modelMatrix * vec4(v_normal, 0.0f);
}
