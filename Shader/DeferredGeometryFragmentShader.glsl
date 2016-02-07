#version 330

in vec3 f_worldPosition;
in vec2 f_textureCoordinates;
in vec3 f_normal;

uniform sampler2D textureDiffuse;
uniform mat4 colorMatrix;
uniform vec2 tileVector;

layout (location = 0) out vec3 out_worldPosition;
layout (location = 1) out vec4 out_color;
layout (location = 2) out vec3 out_normal;

void main()
{
	out_worldPosition = f_worldPosition;
	//Note the component wise multiplication below
	out_color = colorMatrix * texture2D(textureDiffuse, tileVector * f_textureCoordinates); 
	out_normal = f_normal;
}
