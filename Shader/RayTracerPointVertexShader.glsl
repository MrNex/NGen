#version 330

layout (location = 0) in vec3 in_Position;

uniform mat4 modelViewProjectionMatrix;

void main(void)
{
	gl_Position = modelViewProjectionMatrix * vec4(in_Position, 1.0f);
}
