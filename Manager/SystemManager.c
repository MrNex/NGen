#include "SystemManager.h"

#include <stdlib.h>

//Internals
SystemBuffer* systemBuffer;

///
//Allocates a new System Buffer
//
//Returns:
//	A pointer to the newly allocated System Buffer
static SystemBuffer* SystemManager_AllocateBuffer(void)
{
	return (SystemBuffer*)malloc(sizeof(SystemBuffer));
}


///
//Initializes a new SystemBuffer and sets default values
//
//Parameters:
//	buffer: A pointer to the SystemBuffer being initialized
static void SystemManager_InitializeBuffer(SystemBuffer* buffer)
{
	buffer->running = 1;
}

//Externals

///
//Initializes the system system manager to default values.
//Default values include:
//	running = 1
void SystemManager_Initialize(void)
{
	systemBuffer = SystemManager_AllocateBuffer();
	SystemManager_InitializeBuffer(systemBuffer);
}

///
//Free's the internal data of the system system manager
void SystemManager_Free(void)
{
	free(systemBuffer);
}

///
//Sets the running attribute of the SystemManager's SystemBuffer
//
//Parameters:
//	isRunning: 0 if the system is not running, 1 if it is.
void SystemManager_SetRunning(const uint8_t isRunning)
{
	systemBuffer->running = isRunning;
}

///
//Gets the running attribute of the SystemManager's SystemBuffer.
//
//Returns:
//	0 if the system is not running | 1 if the system is running.
uint8_t SystemManager_GetRunning(void)
{
	return systemBuffer->running;
}
