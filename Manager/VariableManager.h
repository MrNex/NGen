#ifndef VARIABLEMANAGER_H
#define VARIABLEMANAGER_H

#include <stdint.h>

typedef struct VariableBuffer
{
	uint8_t running;	//Is the system running? 
} VariableBuffer;

extern VariableBuffer* variableBuffer; 

///
//Initializes the system variable manager to default values.
//Default values include:
//	running = 1
void VariableManager_Initialize(void);

///
//Free's the internal data of the system variable manager
void VariableManager_Free(void);

///
//Sets the running attribute of the VariableManager's VariableBuffer
//
//Parameters:
//	isRunning: 0 if the system is not running, 1 if it is.
void VariableManager_SetRunning(const uint8_t isRunning);

///
//Gets the running attribute of the VariableManager's VariableBuffer.
//
//Returns:
//	0 if the system is not running | 1 if the system is running.
uint8_t VariableManager_GetRunning(void);


#endif
