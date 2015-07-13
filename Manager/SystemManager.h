#ifndef SYSTEMMANAGER_H
#define SYSTEMMANAGER_H

#include <stdint.h>

typedef struct SystemBuffer
{
	uint8_t running;	//Is the system running? 
} SystemBuffer;

extern SystemBuffer* systemBuffer; 

///
//Initializes the system system manager to default values.
//Default values include:
//	running = 1
void SystemManager_Initialize(void);

///
//Free's the internal data of the system system manager
void SystemManager_Free(void);

///
//Sets the running attribute of the SystemManager's VariableBuffer
//
//Parameters:
//	isRunning: 0 if the system is not running, 1 if it is.
void SystemManager_SetRunning(const uint8_t isRunning);

///
//Gets the running attribute of the SystemManager's VariableBuffer.
//
//Returns:
//	0 if the system is not running | 1 if the system is running.
uint8_t SystemManager_GetRunning(void);


#endif
