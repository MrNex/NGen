#version 330

layout(triangles) in;
layout (triangle_strip, max_vertices=128) out;

uniform sampler2D positionTexture;

uniform vec3 lightDirection;
uniform vec3 rightDirection;
uniform vec3 upDirection;

uniform vec2 screenSize;

void main()
{


	int blocki = 10;
	float blockSize = 10.0f;

	mat4 projection = mat4(
		screenSize.x/blockSize, 0.0f, 0.0f, 0.0f,
		0.0f, screenSize.y/blockSize, 0.0f, 0.0f,
		0.0f, 0.0f, -2.0f/99.9f, 0.0f,
	        0.0f, 0.0f, -100.1f/99.9, 1.0f);

	mat4 view = mat4(
	 	rightDirection.x, upDirection.x, lightDirection.x, 0.0f,
		rightDirection.y, upDirection.y, lightDirection.y, 0.0f,
		rightDirection.z, upDirection.z, lightDirection.z, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);


	mat4 pv = projection * view;


	int width = int(screenSize.x);
	int height = int(screenSize.y);


	float stepsW = blockSize/screenSize.x;
	float stepsH = blockSize/screenSize.y;
	vec2 texPos = vec2(0.0f, 0.0f);

	for(int i = 0; i < height; i+=blocki)
	{
		for(int j = 0; j < width; j+=blocki)
		{
			
			vec3 pos = texture(positionTexture, texPos).xyz;

			float leftBound = ((texPos.x - blockSize / screenSize.x) - 0.5f) * 2.0f;
			float rightBound = ((texPos.x + blockSize / screenSize.x) - 0.5f) * 2.0f;
			float upperBound = ((texPos.y + blockSize / screenSize.y) - 0.5f) * 2.0f;
			float lowerBound = ((texPos.y - blockSize / screenSize.y) - 0.5f) * 2.0f;
			
			for(int k = 0; k < 3; k++)
			{
				gl_Position = pv * (gl_in[k].gl_Position - vec4(pos, 0.0f));
				

				gl_ClipDistance[0] = rightBound - gl_Position.x;
				gl_ClipDistance[1] = gl_Position.x - leftBound;
				gl_ClipDistance[2] = upperBound - gl_Position.y;
				gl_ClipDistance[3] = gl_Position.y - lowerBound;
				EmitVertex();	
			}

			EndPrimitive();
			texPos.x += stepsW;
		}


		texPos.x = 0.0f;
		texPos.y += stepsH;
	}
}

