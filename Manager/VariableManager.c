#include "VariableManager.h"

#include <stdlib.h>

//Internals
VariableBuffer* variableBuffer;

///
//Allocates a new Variable Buffer
//
//Returns:
//	A pointer to the newly allocated Variable Buffer
static VariableBuffer* VariableManager_AllocateBuffer(void)
{
	return (VariableBuffer*)malloc(sizeof(VariableBuffer));
}


///
//Initializes a new VariableBuffer and sets default values
//
//Parameters:
//	buffer: A pointer to the VariableBuffer being initialized
static void VariableManager_InitializeBuffer(VariableBuffer* buffer)
{
	buffer->running = 1;
}

//Externals

///
//Initializes the system variable manager to default values.
//Default values include:
//	running = 1
void VariableManager_Initialize(void)
{
	variableBuffer = VariableManager_AllocateBuffer();
	VariableManager_InitializeBuffer(variableBuffer);
}

///
//Free's the internal data of the system variable manager
void VariableManager_Free(void)
{
	free(variableBuffer);
}

///
//Sets the running attribute of the VariableManager's VariableBuffer
//
//Parameters:
//	isRunning: 0 if the system is not running, 1 if it is.
void VariableManager_SetRunning(const uint8_t isRunning)
{
	variableBuffer->running = isRunning;
}

///
//Gets the running attribute of the VariableManager's VariableBuffer.
//
//Returns:
//	0 if the system is not running | 1 if the system is running.
uint8_t VariableManager_GetRunning(void)
{
	return variableBuffer->running;
}
