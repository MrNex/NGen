#include "../Math/Vector.h"

typedef struct InputBuffer
{
	int* mousePosition;
	int* previousMousePosition;

	unsigned short* mouseButtonStates;
	unsigned short* keyStates;

	unsigned short mouseLock;
	unsigned short mouseVisible;
} InputBuffer;
///
//Internals
static InputBuffer* inputBuffer;

///
//Allocates memory for a new input buffer
//
//Returns:
//	Pointer to newly allocated inputBuffer
static InputBuffer* InputManager_AllocateBuffer(void);

///
//Initializes an Input Buffer
//
//Parameters:
//	buffer: Input Buffer to initialize
static void InputManager_InitializeBuffer(InputBuffer* buffer);

///
//Frees the memory taken by an Input Buffer
//
//Parameters:
//	buffer: The buffer to free
static void InputManager_FreeBuffer(InputBuffer* buffer);

///
//Traps the mouse inside of the window
static void InputManager_TrapMouse(void);


///
//Functions

///
//Initializes the Input Manager
void InputManager_Initialize(void);

///
//Frees resources taken by the Input Manager
void InputManager_Free(void);

///
//gets the internal input buffer
//
//Returns:
//	the internal input buffer
InputBuffer InputManager_GetInputBuffer(void);

///
//Updates the state of the input manager.
//Current mouse state becomes previous, Key input for mouse trapping gets done here.
void InputManager_Update(void);

///
//Called when mouse movement is registered
//Updates the current position of the mouse
//
//Parameters:
//	x: current X position of the mouse relative to the window
//	y: current y position of the mouse relative to the window
void InputManager_OnMouseMove(int x, int y);

///
//Called when mouse movement is registered while a mouse button is pressed
//Updates the current position of the mouse
//
//Parameters:
//	x: The current x position of the mouse
//	y: The current y position of the mouse
void InputManager_OnMouseDrag(int x, int y);

///
//Called when a mouse click is registered
//Updates the current state of the mouse button being clicked
//
//Parameters:
//	button: The button that was pressed (0-LEFT | 1-MIDDLE | 2-RIGHT)
//	state: The state of the button (0-PRESSED | 1-RELEASED)
//	x: The current x position of the mouse
//	y: the current y position of the mouse
void InputManager_OnMouseClick(int button, int state, int x, int y);



///
//Called when a key press is registered
//Updates the state of the key being pressed
//
//Parameters:
//	key: character of the key which was pressed
//	x: Current x position of the mouse
//	y: Current y position of the mouse
void InputManager_OnKeyPress(unsigned char key, int x, int y);

///
//Called when a key release is registered
//Updates the state of the key being released
//
//Parameters:
//	key: character of the key which was released
//	x: Current x position of the mouse
//	y: Current y position of the mouse
void InputManager_OnKeyRelease(unsigned char key, int x, int y);

///
//Determines whether a key is pressed down
//
//Parameters:
//	key: The character representing the key to query
//
//Returns:
//	0 if state of key is Released
//	1 if state of key is Pressed
short InputManager_IsKeyDown(unsigned char key);

///
//Determines whether a mouse button is pressed down
//
//Parameters:
// button: The mouse button to check (0 = LMB | 1 = MMB | 2 = RMB)
//
//Returns:
//	0 if state of button is released
//	1 if state of button is pressed
unsigned char InputManager_IsMouseButtonPressed(unsigned char button);

///
//Determines whether a key was pressed the current update loop
//i.e. determines if a key was "Just" pressed.
//
//Will only occur once per key release! A key is considered pressed if and only if
//it's current state is down and it's previous state was up.
//
//Parameters:
//	key: The characer representing the key to query
//short InputManager_IsKeyPressed(unsigned char key);

