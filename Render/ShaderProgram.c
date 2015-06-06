#include "ShaderProgram.h"
#include "../Load/Loader.h"

//Declarations
static GLuint LoadShader(const char* fileName, GLenum shaderType);


///
//Allocates a new shader program returning a pointer to it in memory
//
//Returns:
//	A pointer to a newly allocated shader program
ShaderProgram* ShaderProgram_Allocate()
{
	ShaderProgram* prog = (ShaderProgram*)calloc(sizeof(ShaderProgram), 1);
	return prog;
}

///
//Initializes a shader program
//
//PArameters:
//	prog: The shader program to initialize
//	vPath: filepath to vertex shader .glsl file
//	fPath: filepath to fragment shader .glsl file
void ShaderProgram_Initialize(ShaderProgram* prog, const char* vPath, const char* fPath)
{

	//Shader cmopilation
	prog->vertexShaderID = LoadShader(vPath, GL_VERTEX_SHADER);
	prog->fragmentShaderID = LoadShader(fPath, GL_FRAGMENT_SHADER);

	//Check for comiler errors
	if (prog->vertexShaderID == 0 || prog->fragmentShaderID == 0)
	{
		return;
	}

	//Shader linking
	prog->shaderProgramID = glCreateProgram();

	glAttachShader(prog->shaderProgramID, prog->vertexShaderID);
	glAttachShader(prog->shaderProgramID, prog->fragmentShaderID);

	glLinkProgram(prog->shaderProgramID);

	//Program complete, checking for errors
	//
	GLint status = 0;
	glGetProgramiv(prog->shaderProgramID, GL_LINK_STATUS, &status);

	//Valid shaders are returned here
	if (status == GL_TRUE)
	{
		//Get uniforms
		prog->modelMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelMatrix");
		prog->viewMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "viewMatrix");
		prog->projectionMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "projectionMatrix");
		prog->modelViewProjectionMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "modelViewProjectionMatrix");

		prog->colorMatrixLocation = glGetUniformLocation(prog->shaderProgramID, "colorMatrix" );

		prog->directionalLightVectorLocation = glGetUniformLocation(prog->shaderProgramID, "directionalLightVector");

		prog->textureLocation = glGetUniformLocation(prog->shaderProgramID, "textureDiffuse");

		return;
	}

	//If code reaches this point, shader program creation was invalid.
	GLint logLength = 0;
	glGetProgramiv(prog->shaderProgramID, GL_INFO_LOG_LENGTH, &logLength);

	//Create character array to store log in
	char* log = (char*)malloc(sizeof(char) * (logLength + 1));			//Leaving room for null terminator

	//Retrieve log and store in character array
	glGetShaderInfoLog(
		prog->shaderProgramID,	//Program's log we are getting
		logLength,			//Length of log
		0,				//Pointer to length, not needed because we provided a length of log
		log				//character array to store log in
		);

	log[logLength] = '\0';

	//Print out log
	printf("Error linking shaders:\nlog: %s", log);

	//Never Forget
	free(log);

}

///
//Frees the memory being taken up by a shader program
//
//Parameters:
//	prog: The shader program to free
void ShaderProgram_Free(ShaderProgram* prog)
{
	//TODO: Call the correct gl Delete functions for the vertex shader, fragment shader, and shader program.
	free(prog);
}

///
//Loads a shader source and compiles it
//
//Parameters:
//	fileName: Filepath of the shader
//	shaderType: Type of shader being compiled
//
//Returns:
//	Shader index
static GLuint LoadShader(const char* fileName, GLenum shaderType)
{
	const char* fileContents = Loader_LoadTextFile(fileName);

	//If loading failed
	if (fileContents == 0)
	{
		printf("Shader file failed to load!\n");
		return 0;
	}

	printf("\n\nSource:\n%s", fileContents);

	GLuint shaderIndex = glCreateShader(shaderType);
	glShaderSource(
		/*Index*/	shaderIndex,
		/*numStrings*/	1,
		/*ArrOfString*/	&fileContents,
		/*Length*/	0		//0 means the length is null terminated.
		);


	glCompileShader(shaderIndex);

	//Done with source code, free memory!
	free((void*)fileContents);


	//Determine if the shader was loaded correctly
	GLint status;
	glGetShaderiv(shaderIndex, GL_COMPILE_STATUS, &status);

	if (status == GL_TRUE)
	{
		return shaderIndex;
	}

	//If code reaches below this point
	//Shader compilation has failed.

	//Proceed to panic here

	GLint logLength = 0;
	//Get length of log
	glGetShaderiv(shaderIndex, GL_INFO_LOG_LENGTH, &logLength);


	char* log = (char*)malloc(sizeof(char) * (logLength + 1));

	//Get log
	glGetShaderInfoLog(
		shaderIndex,	//Shader index to get log of
		logLength,	//Length of log
		0,		//Address of length (We don't need because we gave it the length
		log);		//Character array to store log in


	log[logLength] = '\0';

	printf("Error loading shader\nShader: %s\nlog: %s\n", fileName, log);

	//Delete defective shader
	glDeleteShader(shaderIndex);

	//Never forget
	free(log);

	//Return 0
	return 0;
}


