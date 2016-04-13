#version 330

layout (location = 0) in vec3 v_position;

uniform mat4 modelMatrix;

void main()
{
	gl_Position = modelMatrix * vec4(v_position, 1.0f);
}
