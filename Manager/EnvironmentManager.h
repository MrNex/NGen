#ifndef ENVIRONMENTMANAGER_H
#define ENVIRONMENTMANAGER_H

#if defined __linux__
#define linux
#elif defined(_WIN32) || defined(_WIN64)
#define windows
#endif

#include <GL/glew.h>
#include <GL/freeglut.h>
#ifdef linux
#include <GL/glx.h>
#endif
#ifdef windows
#include <GL/wgl.h>
#endif

enum EnvironmentManager_OS
{
	EnvironmentManager_OS_WINDOWS,
	EnvironmentManager_OS_MAC,
	EnvironmentManager_OS_LINUX,
	EnvironmentManager_OS_OTHER
};

typedef struct EnvironmentBuffer
{
	int winID;

	char* windowTitle;

	int windowWidth;
	int windowHeight;

	int majorGLContext;
	int minorGLContext;

	GLXContext glContextHandle;
	Display* glDeviceHandle;

	enum EnvironmentManager_OS operatingSystem;
	

} EnvironmentBuffer;

///
//Defined internally
extern EnvironmentBuffer* environmentBuffer;

///
//Function Declarations

///
//Initializes the environment manager
//
//Parameters:
//	argc: main programs unmodified command line argument count
//	argv: main programs unmodified command line argument values
void EnvironmentManager_Initialize(int* argc, char** argv);

///
//Frees any memory used by the Environment Manager
void EnvironmentManager_Free(void);

///
//Gets a pointer to the active environment buffer
//
//Returns:
//	A pointer to the currently active internal environment buffer
EnvironmentBuffer* EnvironmentManager_GetEnvironmentBuffer(void);

///
//Updates the EnvironmentManager upon a window being reshaped by the user
//
//Parameters:
//	width: The updated width in pixels of the window
//	height: The updated height in pixels of the window
void EnvironmentManager_OnWindowReshape(int width, int height);

///
//Generates a random integer within the bounds
//
//Parameters:
//	min: The minimum bound
//	max: The maximum bound
//
//Returns:
//	An integer between min and max
int EnvironmentManager_Rand(int min, int max);

///
//Generates a random float within the bounds
//
//Parameters:
//	min: The minimum bound
//	max: The maximum bound
//
//Returns:
//	Float between min and max
float EnvironmentManager_Randf(float min, float max);
#endif
