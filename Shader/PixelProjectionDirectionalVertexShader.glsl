#version 330

layout (location = 0) in vec3 in_Position;

void main()
{
	gl_Position = vec4(in_Position, 1.0f);
}
