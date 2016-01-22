#if defined __linux__

//Enable POSIX definitions (for timespec)
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif
//#define _POSIX_C_SOURCE 1

#include <time.h>

#endif

#include "../Manager/EnvironmentManager.h"
#include "../Manager/InputManager.h"
#include "../Manager/ObjectManager.h"
#include "../Manager/RenderingManager.h"
#include "../Manager/AssetManager.h"
#include "../Manager/TimeManager.h"
#include "../Manager/CollisionManager.h"
#include "../Manager/PhysicsManager.h"
#include "../Manager/SystemManager.h"


///
//Initializes the scene within the engine.
//This must be done after all engine components are initialized.
void InitializeScene(void);
