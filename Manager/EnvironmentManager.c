#include "EnvironmentManager.h"

#include <GL/glew.h>
#include <GL/freeglut.h>
#ifdef linux
#include <GL/glx.h>
#endif
#ifdef windows
#include <GL/wgl.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/utsname.h>

///
//Internal Declarations
///

EnvironmentBuffer* environmentBuffer = NULL;

///
//Static Function Declarations
///

///
//Allocates an uninitialized EnvironmentBuffer
//
//Returns:
//	A pointer to an uninitialized environmentBuffer
static EnvironmentBuffer* EnvironmentManager_AllocateBuffer(void);

///
//Identifies the OS of the machine
//
//Returns:
//	The OS of the machine
static enum EnvironmentManager_OS EnvironmentManager_IdentifyOS(void);

///
//Initializes an uninitialized environment buffer
//
//Parameters:
//	buffer: A pointer to the environmentBuffer to initialize
static void EnvironmentManager_InitializeBuffer(EnvironmentBuffer* buffer);

///
//Frees memory utilized by an EnvironmentBuffer
//
//Parameters:
//	buffer: A pointer to the environment buffer to be freed
static void EnvironmentManager_FreeBuffer(EnvironmentBuffer* buffer);

///
//Initializes the environment manager
//
//Parameters:
//	argc: main programs unmodified command line argument count
//	argv: main programs unmodified command line argument values
void EnvironmentManager_Initialize(int* argc, char** argv)
{
	glutInit(argc, argv);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);
	glutInitDisplayMode(GLUT_RGBA);

	environmentBuffer = EnvironmentManager_AllocateBuffer();
	EnvironmentManager_InitializeBuffer(environmentBuffer);

	glutInitWindowSize(environmentBuffer->windowWidth, environmentBuffer->windowHeight);
	glutInitWindowPosition(0, 0);

	glutInitContextVersion(environmentBuffer->majorGLContext, environmentBuffer->minorGLContext);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	environmentBuffer->winID = glutCreateWindow(environmentBuffer->windowTitle);

	glewExperimental = GL_TRUE;

	//Make sure initialization went okay
	if(glewInit() != GLEW_OK)
	{
		printf("Glew initialization has failed! Aborting program.\n");
		_Exit(-1);
	}


	if(environmentBuffer->operatingSystem == EnvironmentManager_OS_LINUX)
	{
#ifdef linux
		environmentBuffer->glContextHandle = glXGetCurrentContext();
		environmentBuffer->glDeviceHandle = glXGetCurrentDisplay();
		printf("Context retrieved\n");
#endif
	}
	else if(environmentBuffer->operatingSystem == EnvironmentManager_OS_WINDOWS)
	{
#ifdef windows
		environmentBuffer->glContextHandle = wglGetCurrentContext();
		environmentBuffer->glDeviceHandle = wglGetCurrentDisplay();
#endif
	}

	//Seed random generator
	srand(time(NULL));
}

///
//Frees any memory used by the Environment Manager
void EnvironmentManager_Free(void)
{
	EnvironmentManager_FreeBuffer(environmentBuffer);
	environmentBuffer = NULL;
}

///
//Gets a pointer to the active environment buffer
//
//Returns:
//	A pointer to the currently active internal environment buffer
EnvironmentBuffer* EnvironmentManager_GetEnvironmentBuffer(void)
{
	return environmentBuffer;
}

///
//Updates the EnvironmentManager upon a window being reshaped by the user
//
//Parameters:
//	width: The updated width in pixels of the window
//	height: The updated height in pixels of the window
void EnvironmentManager_OnWindowReshape(int width, int height)
{
	environmentBuffer->windowWidth = width;
	environmentBuffer->windowHeight = height;
	//printf("New window dimensions:\t(%d, %d)\n", width, height);
}

///
//Generates a random integer within the bounds
//
//Parameters:
//	min: The minimum bound
//	max: The maximum bound
//
//Returns:
//	An integer between min and max
int EnvironmentManager_Rand(int min, int max)
{
	return (rand() % (max - min)) + min;
}

///
//Generates a random float within the bounds
//
//Parameters:
//	min: The minimum bound
//	max: The maximum bound
//
//Returns:
//	Float between min and max
float EnvironmentManager_Randf(float min, float max)
{
	float r = (((float)rand()) / ((float)RAND_MAX)) * (max - min) + min;
	printf("r:\t%f", r);
	return r;
}

///
//Allocates an uninitialized EnvironmentBuffer
//
//Returns:
//	A pointer to an uninitialized environmentBuffer
static EnvironmentBuffer* EnvironmentManager_AllocateBuffer(void)
{
	return malloc(sizeof(EnvironmentBuffer));
}

static enum EnvironmentManager_OS EnvironmentManager_IdentifyOS(void)
{
	//Determine the os
	struct utsname osInfo;
	if(uname(&osInfo))
	{
		//Error
		printf("Error: EnvironmentManager Failed to identify OS\n");
	}
	else
	{
		printf("Operating system is %s\n", osInfo.sysname);
		if(strcmp(osInfo.sysname, "Linux") == 0)
		{
			return EnvironmentManager_OS_LINUX;
		}
		else if(strcmp(osInfo.sysname, "Darwin") == 0)
		{
			return EnvironmentManager_OS_MAC;
		}
		else if(strncmp(osInfo.sysname, "CYGWIN", strlen("CYGWIN")) == 0 ||
				strncmp(osInfo.sysname, "MINGW", strlen("MINGW")) == 0)
		{
			return EnvironmentManager_OS_WINDOWS;
		}
	}
	return EnvironmentManager_OS_OTHER;

}

///
//Initializes an uninitialized environment buffer
//
//Parameters:
//	buffer: A pointer to the environmentBuffer to initialize
static void EnvironmentManager_InitializeBuffer(EnvironmentBuffer* buffer)
{
	buffer->windowWidth = 800;
	buffer->windowHeight = 600;
	buffer->majorGLContext = 4;
	buffer->minorGLContext = 3;
	int titleLen =  strlen("NGen V4.0");
	buffer->windowTitle = malloc(sizeof(char) * titleLen);
	strcpy(buffer->windowTitle, "NGen V4.0");
	buffer->operatingSystem = EnvironmentManager_IdentifyOS();

}

///
//Frees memory utilized by an EnvironmentBuffer
//
//Parameters:
//	buffer: A pointer to the environment buffer to be freed
static void EnvironmentManager_FreeBuffer(EnvironmentBuffer* buffer)
{
	free(buffer->windowTitle);
	free(environmentBuffer);
}
