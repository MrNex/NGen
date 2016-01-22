#ifndef ENVIRONMENTMANAGER_H
#define ENVIRONMENTMANAGER_H

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

#endif
