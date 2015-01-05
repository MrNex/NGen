#include <stdlib.h>
#include <stdio.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/freeglut.h>

void Init(void)
{
	printf("Initializing\n");

	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

///
//Checks for any OpenGL errors and prints if found
void CheckGLErrors(void)
{
	int error = glGetError();
	if(error != GL_NO_ERROR)
	{
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
	printf("Updating\n");

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
	printf("Drawing\n");

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

	//initialize glew
	//glewExperimental = GL_TRUE;
	//if(glewInit() != GLEW_OK) 
	//{
	//	return -1;
	//}

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
