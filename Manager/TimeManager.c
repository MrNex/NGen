#include "TimeManager.h"

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <stdio.h>

//#include "InputManager.h"

///
//Declarations

TimeBuffer* timeBuffer;


///
//Implementations


///
//Gets the internal time buffer from the time manager
//
//Returns:
//	Time buffer being managed by the time manager.
TimeBuffer TimeManager_GetTimeBuffer(void)
{
	return *timeBuffer;
}

///
//Initializes the Time Manager
void TimeManager_Initialize(void)
{
	timeBuffer = TimeManager_TimeBuffer_Allocate();
	TimeManager_TimeBuffer_Initialize(timeBuffer);
}

///
//Frees the time manager
void TimeManager_Free(void)
{
	TimeManager_TimeBuffer_Free(timeBuffer);
}

///
//Allocates a new time buffer
//
//Returns:
//	Pointer to a newly allocated uninitialized time buffer
TimeBuffer* TimeManager_TimeBuffer_Allocate(void)
{
	TimeBuffer* buffer = (TimeBuffer*)malloc(sizeof(TimeBuffer));
	return buffer;
}

///
//Initializes and starts a timebuffer
//
//Parameters:
//	buffer: timebuffer to initialize
void TimeManager_TimeBuffer_Initialize(TimeBuffer* buffer)
{

#ifdef windows
	buffer->ticksPerSecond = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	QueryPerformanceFrequency(buffer->ticksPerSecond);

	//buffer->startTime = glutGet(GLUT_ELAPSED_TIME);
	buffer->startTick = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	QueryPerformanceCounter(buffer->startTick);

	//buffer->elapsedTime = 0;
	buffer->elapsedTicks = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->elapsedTicks->QuadPart = 0L;

	buffer->deltaTicks = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->deltaTicks->QuadPart = 0L;
	
	buffer->elapsedTime = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->elapsedTime->QuadPart = 0L;

	buffer->previousTick = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->previousTick->QuadPart = buffer->startTick->QuadPart;
	
	//buffer->deltaTime = 0.0f;
	buffer->deltaTime = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->deltaTime->QuadPart = 0L;

	buffer->physicsAccumulator = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->physicsAccumulator->QuadPart = 0L;

	buffer->renderAccumulator = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->renderAccumulator->QuadPart = 0L;

	buffer->physicsTimeStep = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->physicsTimeStep->QuadPart = 4166L;

	buffer->renderTimeStep = (LARGE_INTEGER*)malloc(sizeof(LARGE_INTEGER));
	buffer->renderTimeStep->QuadPart = 16666L;
#endif

#ifdef linux
	clock_gettime(CLOCK_MONOTONIC, &buffer->startTime);
	buffer->currentTime.tv_sec = buffer->startTime.tv_sec;
	buffer->currentTime.tv_nsec = buffer->startTime.tv_nsec;

	buffer->deltaTime.tv_sec = 0;
	buffer->deltaTime.tv_nsec = 0L;	

	buffer->renderAccumulator.tv_sec = buffer->physicsAccumulator.tv_sec = 0;
	buffer->renderAccumulator.tv_nsec = buffer->physicsAccumulator.tv_nsec = 0L;

	buffer->physicsTimeStep.tv_sec = buffer->renderTimeStep.tv_sec = 0;
	buffer->physicsTimeStep.tv_nsec = 4166666L;
	buffer->renderTimeStep.tv_nsec = 16666666L;

#endif
	buffer->timeScale = 1.0f;
}

///
//Frees a time buffer
//
//Parameters:
//	buffer: Pointer to the buffer to free
void TimeManager_TimeBuffer_Free(TimeBuffer* buffer)
{

#ifdef windows
	free(buffer->ticksPerSecond);
	free(buffer->startTick);
	free(buffer->elapsedTicks);
	free(buffer->deltaTicks);

	free(buffer->elapsedTime);
	free(buffer->deltaTime);

	free(buffer->previousTick);

	free(buffer->physicsAccumulator);
	free(buffer->renderAccumulator);
	
	free(buffer->physicsTimeStep);
	free(buffer->renderTimeStep);

#endif

	free(buffer);
}

///
//Updates the time manager
void TimeManager_Update(void)
{

	TimeManager_UpdateBuffer(timeBuffer);

}

///
//Updates a time buffer
//
//Parameters:
//	buffer: Time buffer to update
void TimeManager_UpdateBuffer(TimeBuffer* buffer)
{

#ifdef windows
	LARGE_INTEGER currentTick;
	QueryPerformanceCounter(&currentTick);

	buffer->deltaTicks->QuadPart = (currentTick.QuadPart - buffer->previousTick->QuadPart);	
	buffer->deltaTime->QuadPart = (buffer->deltaTicks->QuadPart * 1000000.0f * buffer->timeScale) / buffer->ticksPerSecond->QuadPart;
	
	buffer->elapsedTicks->QuadPart += buffer->deltaTicks->QuadPart;
	buffer->elapsedTime->QuadPart = (buffer->elapsedTicks->QuadPart * 1000000.0) / buffer->ticksPerSecond->QuadPart;

	
	//Add dt to accumulators, but limit the amount they can be incremented by 1/4th of a second.
	//THis will prevent game logic from continuing to update in the case of a freeze(Such as window drag)
	if(buffer->deltaTime->QuadPart < 250000L)
	{
	buffer->physicsAccumulator->QuadPart += buffer->deltaTime->QuadPart;
	buffer->renderAccumulator->QuadPart += buffer->deltaTime->QuadPart;
	}
	else
	{
		buffer->physicsAccumulator->QuadPart += 250000L;
		buffer->renderAccumulator->QuadPart += 250000L;
	}

	buffer->previousTick->QuadPart = currentTick.QuadPart;
#endif

#ifdef linux
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);

	//Determine change in time
	buffer->deltaTime.tv_sec = (time_t)((float)(now.tv_sec - buffer->currentTime.tv_sec) * buffer->timeScale);
	buffer->deltaTime.tv_nsec = (time_t)((float)(now.tv_nsec - buffer->currentTime.tv_nsec) * buffer->timeScale);

	//Add dt to accumulators, but limit the amount they can be incremented by 1/4th of a second.
	//THis will prevent game logic from continuing to update in the case of a freeze(Such as window drag)
	if(buffer->deltaTime.tv_nsec < 250000000L)
	{
		buffer->physicsAccumulator.tv_nsec += buffer->deltaTime.tv_nsec;
		buffer->renderAccumulator.tv_nsec += buffer->deltaTime.tv_nsec;
	}
	else
	{
		buffer->physicsAccumulator.tv_nsec += 250000000L;
		buffer->renderAccumulator.tv_nsec += 250000000L;
	}
	//printf("DT:\t%lu\t:\t%lu\n",buffer->deltaTime.tv_sec, buffer->deltaTime.tv_nsec);

	//Update current time
	buffer->currentTime.tv_sec = now.tv_sec;
	buffer->currentTime.tv_nsec = now.tv_nsec;

#endif

}

///
//Checks if the physics timer is ready to perform a new time step.
//If it is, the timestep will be removed from the TimeManager's internal buffer's physics accumulator
//
//Returns:
//	1 if it is time for the physics to update
//	0 if it is not yet time for physics to update
unsigned char TimeManager_CheckPhysicsTime()
{

#if defined windows
	
	if(timeBuffer->physicsAccumulator->QuadPart > timeBuffer->physicsTimeStep->QuadPart)
	{
		timeBuffer->physicsAccumulator->QuadPart -= timeBuffer->physicsTimeStep->QuadPart;
		return 1;
	}
	return 0;

#elif defined linux

	if(timeBuffer->physicsAccumulator.tv_nsec > timeBuffer->physicsTimeStep.tv_nsec)
	{
		timeBuffer->physicsAccumulator.tv_nsec -= timeBuffer->physicsTimeStep.tv_nsec;
		return 1;
	}
	return 0;

#endif

}

///
//Checks if the render timer is ready to perform a new time step.
//If it is, the TimeManager's internal buffer's render accumulator will be set to 0
//
//Returns:
//	1 if it is time for rendering
//	0 if it is not yet time for rendering
unsigned char TimeManager_CheckRenderTime()
{

#if defined windows
	
	if(timeBuffer->renderAccumulator->QuadPart > timeBuffer->renderTimeStep->QuadPart)
	{
		timeBuffer->renderAccumulator->QuadPart -= 0L;
		return 1;
	}
	return 0;

#elif defined linux

	if(timeBuffer->renderAccumulator.tv_nsec > timeBuffer->renderTimeStep.tv_nsec)
	{
		timeBuffer->renderAccumulator.tv_nsec -= 0L;
		return 1;
	}
	return 0;

#endif

}

///
//Sets the time manager's internal buffer's timeScale
//
//Parameters:
//	scale: The new time scale (closer to 0.0 is slower, higher than 1.0 is faster)
//		Don't put the scale below 0.. Time will go backwards.
void TimeManager_SetTimeScale(float scale)
{
	timeBuffer->timeScale = scale;
	//printf("Time scale:\t%f\n", timeBuffer->timeScale);
}

///
//Scales the time manager's internal buffer's timescale
//
//Parameters:
//	Scale: The scalar to multiply the current timescale by
void TimeManager_ScaleTimeScale(float scale)
{
	timeBuffer->timeScale *= scale;
	//printf("Time scale:\t%f\n", timeBuffer->timeScale);

}

///
//gets delta time in seconds as a single floating point
//
//Returns:
//	Number of seconds since last update
float TimeManager_GetDeltaSec(void)
{
#ifdef windows
	return timeBuffer->deltaTime->QuadPart / 1000000.0f;
#endif

#ifdef linux
	float dt = (float)timeBuffer->deltaTime.tv_sec;
	dt += (float)timeBuffer->deltaTime.tv_nsec / 1000000000.0f;
	//printf("DT:\t%f\n", dt);
	return dt;	
#endif
}
