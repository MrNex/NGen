#include <stdlib.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <GL/freeglut.h>

#include "Math/Vector.h"

///
//Initializes all engine components
void Init(void)
{
	printf("Initializing\n");

	//There currently are no other components to the engine
	
	//Set the background color to fully opaque Red
	//TODO: Move background color setting to the render manager once complete
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);


	//Test vector code
	float* a, * b, * c;
	a = (float*)malloc(sizeof(float) * 3);
	b = (float*)malloc(sizeof(float) * 3);
	c = (float*) malloc(sizeof(float) * 3);

	for(int i = 1; i < 4; i++)
	{
		a[i - 1] = i;
		b[i - 1] = 4-i;
		c[i - 1] = 0;
	}

	Vector_Print(a, 3);
	Vector_Print(b, 3);
	Vector_Print(c, 3);

	Vector_Normalize(a, 3);

	Vector_Add(c, a, b, 3);

	Vector_Scale(a, 50, 3);

	Vector_Print(a, 3);
	Vector_Print(b, 3);
	Vector_Print(c, 3);

	float magA = Vector_GetMag(a, 3);
	printf("\n%f\n", magA);

	free(a);
	free(b);
	free(c);

	
	
}

///
//Checks for any OpenGL errors and prints error code if found
void CheckGLErrors(void)
{
	//Get the openGL error code
	int error = glGetError();

	//If the error code signifies an error has occurred
	if(error != GL_NO_ERROR)
	{
		//Print the error code
		printf("Error: %d\n", error);
	}
}

///
//Updates engine
//
//Parameters:
//	int val - Unused, needed for valid glutTimerFunc signature
void Update(int val)
{
	//printf("Updating\n");

	//Put update code here
	
	//Start drawing
	glutPostRedisplay();

	//Recall update in 1/60 seconds
	glutTimerFunc(16, Update, 0);
}


///
//Draws the current state of the engine
void Draw(void)
{
	//printf("Drawing\n");

	//Clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Draw code here
	
	//Start drawing threads on gpu
	glFlush();
}


///
//Program will begin here
//
//Parameters:
//	int argc - Number of arguments passed from cmd line
//	char* argv[] - Array of C strings of arguments passed from cmd line
int main(int argc, char* argv[])
{
	//Initialize glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitContextVersion(4,3);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	//Window creation
	int win = glutCreateWindow("NGen V1.0");
	

	//Check for errors
	CheckGLErrors();

	//Set up callback registration
	glutTimerFunc(16, Update, 0);
	glutDisplayFunc(Draw);

	//Initialize engine
	Init();

	//Start the main loop
	glutMainLoop();

	//When the loop is over, release all memory
	glutDestroyWindow(win);

	
	return 0;
}
