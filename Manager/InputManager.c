#include "InputManager.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

///
//Initializes the Input Manager
void InputManager_Initialize(void)
{
	inputBuffer = InputManager_AllocateBuffer();
	InputManager_InitializeBuffer(inputBuffer);
}

///
//Frees resources taken by the Input Manager
void InputManager_Free(void)
{
	InputManager_FreeBuffer(inputBuffer);
}

InputBuffer InputManager_GetInputBuffer()
{
	return *inputBuffer;
}

///
//Updates the state of the input manager.
//Current mouse state becomes previous, Key input for mouse trapping gets done here.
void InputManager_Update(void)
{
	if (InputManager_IsKeyDown('m'))
	{
		//Toggle mouseLock and mouseVisible
		inputBuffer->mouseLock = 1;
		inputBuffer->mouseVisible = 0;

		glutSetCursor(GLUT_CURSOR_NONE);
	}
	if(InputManager_IsKeyDown('q'))
	{
		inputBuffer->mouseLock = 0;
		inputBuffer->mouseVisible = 1;

		glutSetCursor(GLUT_CURSOR_INHERIT);
	}

	if (inputBuffer->mouseLock == 1) InputManager_TrapMouse();

	inputBuffer->previousMousePosition[0] = inputBuffer->mousePosition[0];
	inputBuffer->previousMousePosition[1] = inputBuffer->mousePosition[1];

}


///
//Called when mouse movement is registered
//Updates the current position of the mouse
//
//Parameters:
//	x: current X position of the mouse relative to the window
//	y: current y position of the mouse relative to the window
void InputManager_OnMouseMove(int x, int y)
{
	inputBuffer->mousePosition[0] = x;
	inputBuffer->mousePosition[1] = y;
}

///
//Called when mouse movement is registered while a mouse button is pressed
//Updates the current position of the mouse
//
//Parameters:
//	x: The current x position of the mouse
//	y: The current y position of the mouse
void InputManager_OnMouseDrag(int x, int y)
{
	inputBuffer->mousePosition[0] = x;
	inputBuffer->mousePosition[1] = y;
}

///
//Called when a mouse click is registered
//Updates the current state of the mouse button being clicked
//
//Parameters:
//	button: The button that was pressed (0-LEFT | 1-MIDDLE | 2-RIGHT)
//	state: The state of the button (0-RELEASED | 1-PRESSED)
//	x: The current x position of the mouse
//	y: the current y position of the mouse
void InputManager_OnMouseClick(int button, int state, int x, int y)
{
	inputBuffer->mouseButtonStates[button] = state == 1 ? 0 : 1;
}

///
//Called when a key press is registered
//Updates the state of the key being pressed
//
//Parameters:
//	key: character of the key which was pressed
//	x: Current x position of the mouse
//	y: Current y position of the mouse
void InputManager_OnKeyPress(unsigned char key, int x, int y)
{
	inputBuffer->keyStates[key] = 1;
}

///
//Called when a key release is registered
//Updates the state of the key being released
//
//Parameters:
//	key: character of the key which was released
//	x: Current x position of the mouse
//	y: Current y position of the mouse
void InputManager_OnKeyRelease(unsigned char key, int x, int y)
{
	inputBuffer->keyStates[key] = 0;
}

///
//Determines whether a key is pressed down
//
//Parameters:
//	key: The character representing the key to query
//
//Returns:
//	0 if state of key is Released
//	1 if state of key is Pressed
short InputManager_IsKeyDown(unsigned char key)
{
	return inputBuffer->keyStates[key];
}

///
//Determines whether a mouse button is pressed down
//
//Parameters:
// button: The mouse button to check (0 = LMB | 1 = MMB | 2 = RMB)
//
//Returns:
//	0 if state of button is released
//	1 if state of button is pressed
unsigned char InputManager_IsMouseButtonPressed(unsigned char button)
{
	return inputBuffer->mouseButtonStates[button];
}

///
//Allocates memory for a new input buffer
//
//Returns:
//	Pointer to newly allocated inputBuffer
static InputBuffer* InputManager_AllocateBuffer(void)
{
	InputBuffer* buffer = (InputBuffer*)malloc(sizeof(InputBuffer));
	return buffer;
}

///
//Initializes an Input Buffer
//
//Parameters:
//	buffer: Input Buffer to initialize
static void InputManager_InitializeBuffer(InputBuffer* buffer)
{
	buffer->mousePosition = (int*)calloc(sizeof(int), 2);
	buffer->previousMousePosition = (int*)calloc(sizeof(int), 2);

	buffer->mouseButtonStates = (unsigned short*)calloc(sizeof(unsigned short), 3);
	buffer->keyStates = (unsigned short*)calloc(sizeof(unsigned short), 256);

	buffer->mouseLock = 0;
	buffer->mouseVisible = 1;
}

///
//Frees the memory taken by an Input Buffer
//
//Parameters:
//	buffer: The buffer to free
static void InputManager_FreeBuffer(InputBuffer* buffer)
{
	free(buffer->mousePosition);
	free(buffer->previousMousePosition);

	free(buffer->mouseButtonStates);
	free(buffer->keyStates);

	free(buffer);
}


///
//Traps the mouse inside of the window
//TODO: Remove constants and reference Window Manager for window sizes
//TODO: Create Window Manager
static void InputManager_TrapMouse(void)
{
	if (
		inputBuffer->mousePosition[0] >= 800 - (800 / 5) ||
		inputBuffer->mousePosition[1] >= 600 - (600 / 5) ||
		inputBuffer->mousePosition[0] <= 800 / 5 ||
		inputBuffer->mousePosition[1] <= 600 / 5)
	{
		inputBuffer->mousePosition[0] = 400;
		inputBuffer->mousePosition[1] = 300;

		glutWarpPointer(400, 300);

	}
}

