#version 330

in vec3 f_worldPosition;
in vec2 f_textureCoordinates;
in vec3 f_normal;

uniform sampler2D textureDiffuse;
uniform mat4 colorMatrix;
uniform vec2 tileVector;
uniform vec4 materialVector;
uniform vec4 specularColorVector;

layout (location = 0) out vec3 out_worldPosition;
layout (location = 1) out vec4 out_color;
layout (location = 2) out vec3 out_normal;
layout (location = 3) out vec4 out_material;
layout (location = 4) out vec4 out_specular;

void main()
{
	out_worldPosition = f_worldPosition;
	//Note the component wise multiplication below
	/*
	vec2 finalTexCoord = tileVector * f_textureCoordinates;

	int x = int(trunc(finalTexCoord.x));
	int y = int(trunc(finalTexCoord.y));

	int rowOffset = y % 2;
	int colOffset = x % 2;
	int finalOffset = (colOffset + rowOffset) % 2;

	vec4 colorVector = finalOffset * vec4(1.0f, 0.0f, 0.0f, 1.0f);
	finalOffset = (++finalOffset) % 2;
	colorVector += finalOffset * vec4(1.0f, 1.0f, 0.0f, 1.0f);

	out_color = colorMatrix * colorVector;
	*/

	//Note the component wise multiplication below
	out_color = colorMatrix * texture2D(textureDiffuse, tileVector * f_textureCoordinates); 
	out_normal = f_normal;
	out_material = materialVector;
	out_specular = specularColorVector;
}
