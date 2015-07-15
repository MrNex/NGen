#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H


#if defined(_WIN32) || defined(_WIN64)
#define windows
#endif


#if defined __linux__
#define linux

//Enable POSIX definitions (for timespec)
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif
//#define _POSIX_C_SOURCE 1

#include <time.h>

#endif


#ifdef windows
#include <windows.h>

typedef struct TimeBuffer
{
	LARGE_INTEGER* ticksPerSecond;
	LARGE_INTEGER* startTick;
	LARGE_INTEGER* elapsedTicks;
	LARGE_INTEGER* deltaTicks;

	LARGE_INTEGER* elapsedTime;		//In microSeconds
	LARGE_INTEGER* deltaTime;		//In microSeconds

	LARGE_INTEGER* previousTick;

	LARGE_INTEGER* physicsAccumulator;
	LARGE_INTEGER* renderAccumulator;

	LARGE_INTEGER* physicsTimeStep;
	LARGE_INTEGER* renderTimeStep;

	float timeScale;

} TimeBuffer;
#elif defined linux
typedef struct TimeBuffer
{
	struct timespec startTime;
	struct timespec currentTime;
	struct timespec deltaTime;

	struct timespec physicsAccumulator;
	struct timespec renderAccumulator;

	struct timespec physicsTimeStep;
	struct timespec renderTimeStep;

	float timeScale;	

} TimeBuffer;
#endif


//Internals
extern TimeBuffer* timeBuffer;

//Functions

///
//Gets the internal time buffer from the time manager
//
//Returns:
//	Time buffer being managed by the time manager.
TimeBuffer TimeManager_GetTimeBuffer(void);

///
//Initializes the Time Manager
void TimeManager_Initialize(void);

///
//Frees the time manager
void TimeManager_Free(void);

///
//Allocates a new time buffer
//
//Returns:
//	Pointer to a newly allocated uninitialized time buffer
TimeBuffer* TimeManager_TimeBuffer_Allocate(void);

///
//Initializes and starts a timebuffer
//
//Parameters:
//	buffer: timebuffer to initialize
void TimeManager_TimeBuffer_Initialize(TimeBuffer* buffer);

///
//Frees a time buffer
//
//Parameters:
//	buffer: Pointer to the buffer to free
void TimeManager_TimeBuffer_Free(TimeBuffer* buffer);

///
//Updates the time manager
void TimeManager_Update(void);

///
//Updates a time buffer
//
//Parameters:
//	buffer: Time buffer to update
void TimeManager_UpdateBuffer(TimeBuffer* buffer);

///
//Checks if the physics timer is ready to perform a new time step.
//If it is, the timestep will be removed from the TimeManager's internal buffer's physics accumulator
//
//Returns:
//	1 if it is time for the physics to update
//	0 if it is not yet time for physics to update
unsigned char TimeManager_CheckPhysicsTime();

///
//Checks if the render timer is ready to perform a new time step.
//If it is, the TimeManager's internal buffer's render accumulator will be set to 0
//
//Returns:
//	1 if it is time for rendering
//	0 if it is not yet time for rendering
unsigned char TimeManager_CheckRenderTime();

///
//Sets the time manager's internal buffer's timeScale
//
//Parameters:
//	scale: The new time scale (closer to 0.0 is slower, higher than 1.0 is faster)
//	Don't put the scale below 0.. Time will go backwards.
void TimeManager_SetTimeScale(float scale);

///
//Scales the time manager's internal buffer's timescale
//
//Parameters:
//	Scale: The scalar to multiply the current timescale by
void TimeManager_ScaleTimeScale(float scale);



///
//gets delta time in seconds as a single floating point
//
//Returns:
//	Number of seconds since last update
float TimeManager_GetDeltaSec(void);



#endif	//If not defined
