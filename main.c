#if defined __linux__

//Enable POSIX definitions (for timespec)
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif
//#define _POSIX_C_SOURCE 1

#include <time.h>

#endif


#include <stdlib.h>
#include <stdio.h>



#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/freeglut.h>

#include "Implementation/Implementation.h"

#include "Math/Matrix.h"



#include "Data/LinkedList.h"
#include "Data/DynamicArray.h"

#include "Generation/Generator.h"

long timer;
unsigned char keyTrigger;

GObject* obj;

///
//Checks for any OpenGL errors and prints error code if found
void CheckGLErrors(void)
{
	//Get the openGL error code
	int error = glGetError();

	//If the error code signifies an error has occurred
	if (error != GL_NO_ERROR)
	{
		//Print the error code
		printf("Error: %d\n", error);
	}
}



///
//Initializes all engine components
void Init(void)
{

	//Initialize managers
	InputManager_Initialize();
	RenderingManager_Initialize();
	AssetManager_Initialize();
	ObjectManager_Initialize();
	CollisionManager_Initialize();
	PhysicsManager_Initialize();

	//Load assets
	AssetManager_LoadAssets();

	//Initialize the scene
	InitializeScene();

	CheckGLErrors();

	//Time manager must always be initialized last
	TimeManager_Initialize();
}

void CalculateOctTreeCollisions(struct OctTree_Node* node)
{
	if(node->children != NULL)
	{
		for(int i = 0; i < 8; i++)
		{
			CalculateOctTreeCollisions(node->children+i);
		}
	}
	else
	{
		if(node->data->size != 0)
		{
			LinkedList* collisions = CollisionManager_UpdateArray((GObject**)node->data->data, node->data->size);
			PhysicsManager_ResolveCollisions(collisions);
		}
	}
}


///
//Updates engine
//
void Update(void)
{

	//Update time manager
	TimeManager_Update();

	/*
	long  dt = TimeManager_GetTimeBuffer().deltaTime->QuadPart;
	timer += dt;
	if (timer >= 100000)
	{
		printf("dt:\t%d\tmicroseconds\n", dt);
		timer = 0;

	}
	*/

	//Update objects.
	ObjectManager_Update();

	//Feature in development
	if(InputManager_IsKeyDown('g'))
	{
		TimeManager_SetTimeScale(0.0f);
	}
	if(InputManager_IsKeyDown('t'))
	{
		TimeManager_SetTimeScale(1.0f);
	}
	if (InputManager_IsKeyDown('r') || InputManager_IsKeyDown('y') || InputManager_IsKeyDown('o') || InputManager_IsKeyDown('p'))
	{
		if (keyTrigger == 0)
		{
			if (InputManager_IsKeyDown('r'))
			{
				TimeManager_ScaleTimeScale(0.9f);
			}
			else if (InputManager_IsKeyDown('y'))
			{
				TimeManager_ScaleTimeScale(1.1f);
			}
			else if (InputManager_IsKeyDown('o'))
			{
				RenderingManager_GetRenderingBuffer()->debugOctTree = 0;
			}
			else if (InputManager_IsKeyDown('p'))
			{
				RenderingManager_GetRenderingBuffer()->debugOctTree = 1;
			}
		}
		keyTrigger = 1;
	}
	else
	{
		keyTrigger = 0;
	}




	PhysicsManager_Update(ObjectManager_GetObjectBuffer().gameObjects);

	//Update the oct tree
	ObjectManager_UpdateOctTree();

	//LinkedList* collisions = CollisionManager_UpdateList(ObjectManager_GetObjectBuffer().gameObjects);

	//OctTree_Node* octTreeRoot = ObjectManager_GetObjectBuffer().octTree->root;
	//CalculateOctTreeCollisions(octTreeRoot);

	LinkedList* collisions = CollisionManager_UpdateOctTree(ObjectManager_GetObjectBuffer().octTree);


	//Pass collisions to physics manager to be resolved
	PhysicsManager_ResolveCollisions(collisions);

	//Update input
	InputManager_Update();

	CheckGLErrors();

	//If escape is pressed, leave main loop
	if(InputManager_IsKeyDown((unsigned char)27))
	{
		glutLeaveMainLoop();
	}

}

void DrawLoop(int val)
{
	glutPostRedisplay();
	glutTimerFunc(val, DrawLoop, 16);


}

///
//Draws the current state of the engine
void Draw(void)
{
	RenderingManager_Render(ObjectManager_GetObjectBuffer().gameObjects);
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
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(600, 0);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	//Window creation
	/*int win = */	//Should be cleaned up automatically upon exiting the main loop
	glutCreateWindow("NGenVS V3.9");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) { return -1; }


	//Check for errors
	CheckGLErrors();

	///
	//Set up callback registration
	//
	glutIdleFunc(Update);
	glutDisplayFunc(Draw);


	//Callback registration for Input
	glutPassiveMotionFunc(InputManager_OnMouseMove);
	glutMotionFunc(InputManager_OnMouseDrag);
	glutMouseFunc(InputManager_OnMouseClick);
	glutKeyboardFunc(InputManager_OnKeyPress);
	glutKeyboardUpFunc(InputManager_OnKeyRelease);

	//Initialize engine
	Init();

	//Start the draw loop
	glutTimerFunc(16, DrawLoop, 16);


	//Start the main loop
	glutMainLoop();

	InputManager_Free();
	printf("Input done\n");
	RenderingManager_Free();
	printf("Rendering done\n");
	ObjectManager_Free();
	printf("Object done\n");
	AssetManager_Free();
	printf("Assets done\n");
	CollisionManager_Free();
	printf("Collision done\n");
	PhysicsManager_Free();
	printf("Physics done\n");
	TimeManager_Free();
	printf("Time done\n");

	return 0;
}
