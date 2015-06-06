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


#include <stdlib.h>
#include <stdio.h>



#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#endif

#include <GL/glew.h>
#include <GL/freeglut.h>


#include "Manager/InputManager.h"
#include "Manager/ObjectManager.h"
#include "Manager/RenderingManager.h"
#include "Manager/AssetManager.h"
#include "Manager/TimeManager.h"
#include "Manager/CollisionManager.h"
#include "Manager/PhysicsManager.h"

#include "State/Score.h"
#include "State/Reset.h"
#include "State/ApplyForce.h"

#include "Math/Matrix.h"

//#include "FirstPersonCameraState.h"
//#include "RotateState.h"
//#include "RevolutionState.h"
//#include "RotateCoordinateAxisState.h"
//#include "SpringState.h"
//#include "MeshSwapState.h"
//#include "MeshSpringState.h"
#include "State/CharacterController.h"

#include "Data/LinkedList.h"
#include "Data/DynamicArray.h"

#include "Generation/Generator.h"

#ifdef ENABLE_CUDA
#include <cuda.h>
#include <cuda_runtime.h>
#include "AcceleratedVector.h"
#endif

long timer;
unsigned char keyTrigger;

GObject* obj;

///
//Checks for any OpenGL errors and prints error code if found
void CheckGLErrors(void)
{
	//Get the openGL error code
	int error = glGetError();

	//If the error code signifies an error has occurred
	if (error != GL_NO_ERROR)
	{
		//Print the error code
		printf("Error: %d\n", error);
	}
}


/// AddTrashCan Function
// 
//Definition:
// This function will add a Trash Can object to the scene at a specified x and z
//
//Parameters:
// obj - GObject to modify and add
// x - X-Position of the trash can
// z - Z-Position of the trash can
void AddBottle(GObject* obj, float x, float z, int pointValue)
{
	// Allocate and Initialize
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	// Assign Object's Mesh
	obj->mesh = AssetManager_LookupMesh("Bottle");
	// Set up the texture
	obj->texture = AssetManager_LookupTexture("Bottle");
	// mess around with colors
	//*Matrix_Index(obj->colorMatrix, 0, 0) = 0.0f;

	// Create a rigidbody
	obj->body = RigidBody_Allocate();
	// Initialize the rigidbody
	RigidBody_Initialize(obj->body, obj->frameOfReference, 2.0f);

	// Moment of Inertia
	obj->body->coefficientOfRestitution = 0.45f;

	// Initialize a collider
	obj->collider = Collider_Allocate();
	ConvexHullCollider_Initialize(obj->collider);
	//ConvexHullCollider_MakeCubeCollider(obj->collider->data->convexHullData, 2.0f);
	ConvexHullCollider_MakeRectangularCollider(obj->collider->data->convexHullData, 0.8f, 2.0f, 0.8f);

	// Hardcode Vector 
	Vector vector;
	// Initialize onto the stack
	Vector_INIT_ON_STACK(vector, 3);

	// alter cube X,Y,Z
	vector.components[0] = x;
	vector.components[1] = -5.0f;
	vector.components[2] = z;

	// Translate the vector 
	GObject_Translate(obj, &vector);

	Matrix identity;
	Matrix_INIT_ON_STACK(identity, 3, 3);

	State* state = State_Allocate();

	state = State_Allocate();
	State_Reset_Initialize(state, 5.0f, 1.0f, obj->frameOfReference->position, (Vector*)&Vector_ZERO, &identity);
	GObject_AddState(obj, state);

	state = State_Allocate();
	State_Score_Initialize(state, pointValue, 5.0f);
	GObject_AddState(obj, state);

	// add it 
	ObjectManager_AddObject(obj);
}

/// AddTrashCan Function
// 
//Definition:
// This function will add a Trash Can object to the scene at a specified x and z
//
//Parameters:
// obj - GObject to modify and add
// x - X-Position of the trash can
// z - Z-Position of the trash can
void AddTrashCan(GObject* obj, float x, float z)
{
	// Create a pillar to hold shooting object
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	obj->mesh = AssetManager_LookupMesh("Trash Can");
	obj->texture = AssetManager_LookupTexture("Trash Can");

	// Initialize a collider
	obj->collider = Collider_Allocate();
	AABBCollider_Initialize(obj->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	// Hardcode Vector 
	Vector vector;
	// Initialize onto the stack
	Vector_INIT_ON_STACK(vector, 3);

	vector.components[0] = x;
	vector.components[1] = -8.0f;
	vector.components[2] = z;

	GObject_Translate(obj, &vector);

	vector.components[0] = 1.0f;
	vector.components[2] = 1.0f;
	vector.components[1] = 2.0f;

	GObject_Scale(obj, &vector);

	ObjectManager_AddObject(obj);
}

void AddMovingTarget(GObject* obj, float ObjX, float ObjY, float ObjZ, int pointValue)
{
	//Moving Object
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	// Assign Object's Mesh
	obj->mesh = AssetManager_LookupMesh("Target");
	// Set up the texture
	obj->texture = AssetManager_LookupTexture("Target");
	// mess around with colors
	//*Matrix_Index(obj->colorMatrix, 0, 0) = 0.0f;

	// Create a rigidbody
	obj->body = RigidBody_Allocate();
	// Initialize the rigidbody
	RigidBody_Initialize(obj->body, obj->frameOfReference, 1.0f);

	obj->body->coefficientOfRestitution = 1.0f;

	// Initialize a collider
	obj->collider = Collider_Allocate();
	ConvexHullCollider_Initialize(obj->collider);
	ConvexHullCollider_MakeRectangularCollider(obj->collider->data->convexHullData, 2.0f, 2.0f, 0.2);

	//rotate the target around the x axis
	GObject_Rotate(obj, &Vector_E1, 3.14159f / 2.0f);

	// Hardcode Vector 
	Vector vector;
	// Initialize onto the stack
	Vector_INIT_ON_STACK(vector, 3);

	// alter cube X,Y,Z
	vector.components[0] = ObjX;
	vector.components[1] = ObjY;
	vector.components[2] = ObjZ;

	// Translate the vector 
	GObject_Translate(obj, &vector);

	Vector force;
	Vector_INIT_ON_STACK(force, 3);
	force.components[0] = -5.0f;

	State* state = State_Allocate();

	state = State_Allocate();
	State_Reset_Initialize(state, 5.0f, 1.0f, obj->frameOfReference->position, (Vector*)&force, obj->frameOfReference->rotation);
	GObject_AddState(obj, state);

	state = State_Allocate();
	State_Score_Initialize(state, pointValue, 5.0f);
	GObject_AddState(obj, state);

	state = State_Allocate();

	force.components[0] = 0.0f;
	force.components[1] = 9.81f;

	State_Force_Initialize(state, &force, (Vector*)&Vector_ZERO);

	GObject_AddState(obj, state);

	force.components[1] = 0.0f;
	force.components[0] = -5.0f;
	RigidBody_ApplyImpulse(obj->body, &force, &Vector_ZERO);

	// add it 
	ObjectManager_AddObject(obj);
}


///
//Initializes the scene within the engine,
//Must be done after all vital engine components are initialized.
//This is all components excluding the TimeManager.
void InitializeScene(void)
{
	///
	//Camera controller simulation
	GObject* cam = GObject_Allocate();
	GObject_Initialize(cam);

	State* state = State_Allocate();

	State_CharacterController_Initialize(state, 7.0f, 5.0f, 10.0f, 1.0f);

	GObject_AddState(cam,state);
	//cam->mesh = AssetManager_LookupMesh("Cube");
	cam->collider = Collider_Allocate();
	// Adds a AABB Collider to the camera. Gives it collision detection
	AABBCollider_Initialize(cam->collider,3.0f,3.0f,3.0f,&Vector_ZERO);
	//ConvexHullCollider_Initialize(cam->collider);
	//ConvexHullCollider_MakeCubeCollider(cam->collider->data->convexHullData, 3.0f);
	
	// Adds rigidbody, causes reaction.
	cam->body = RigidBody_Allocate();
	RigidBody_Initialize(cam->body, cam->frameOfReference, 5.0f);
	cam->body->coefficientOfRestitution = 0.3f;

	cam->body->freezeRotation = 1;

	// Hardcode Vector 
	Vector vector;
	// Initialize onto the stack
	Vector_INIT_ON_STACK(vector, 3);
	// alter cube X,Y,Z
	vector.components[0] = 0.0f;
	vector.components[1] = -5.0f;
	vector.components[2] = 10.0f;

	GObject_Translate(cam, &vector);

	// alter cube X,Y,Z
	vector.components[0] = 1.0f;
	vector.components[1] = 2.3f;
	vector.components[2] = 1.0f;

	GObject_Scale(cam, &vector);

	ObjectManager_AddObject(cam);

	///
	//Create Objects
	// Actually allocate space and initialize

	GObject* obj = GObject_Allocate();
	GObject_Initialize(obj);

	// Add Bottles to the scene
	AddBottle(obj, 0.0f, -18.0f, 10);
	AddBottle(obj, 15.0f, -30.0f, 20);
	AddBottle(obj, 30.0f, -42.0f, 30);
	AddBottle(obj, -15.0f, -30.0f, 20);
	AddBottle(obj, -30.0f, -42.0f, 30);

	// Add the Trash Cans to the scene
	AddTrashCan(obj, 0, -18.0f);
	AddTrashCan(obj, 15.0f, -30.0f);
	AddTrashCan(obj, 30.0f, -42.0f);
	AddTrashCan(obj, -15.0f, -30.0f);
	AddTrashCan(obj, -30.0f, -42.0f);

	// Add the Moving Tragets to the scene
	AddMovingTarget(obj, 20.0f, 5.0f, -18.0f, 50);
	//AddMovingTarget(obj, -20.0f, 5.0f, -30.0f, 100);

	///////////////////////////////////////
	//Create ground

	obj = GObject_Allocate();
	GObject_Initialize(obj);

	obj->mesh = AssetManager_LookupMesh("Cube");
	obj->texture = AssetManager_LookupTexture("Floor");

	// Initialize a collider
	obj->collider = Collider_Allocate();
	AABBCollider_Initialize(obj->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	vector.components[0] = 0.0f;
	vector.components[1] = -10.0f;
	vector.components[2] = 0.0f;

	GObject_Translate(obj, &vector);


	vector.components[0] = 48.0f;
	vector.components[2] = 48.0f;
	vector.components[1] = 1.0f;

	GObject_Scale(obj, &vector);
	ObjectManager_AddObject(obj);

	///
	//Create ceiling
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	obj->mesh = AssetManager_LookupMesh("Cube");
	obj->texture = AssetManager_LookupTexture("Wall");

	// Initialize a collider
	obj->collider = Collider_Allocate();
	AABBCollider_Initialize(obj->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	vector.components[0] = 0.0f;
	vector.components[1] = 50.0f;
	vector.components[2] = 0.0f;

	GObject_Translate(obj, &vector);


	vector.components[0] = 48.0f;
	vector.components[2] = 48.0f;
	vector.components[1] = 1.0f;

	GObject_Scale(obj, &vector);
	ObjectManager_AddObject(obj);

	///
	//Create Back Wall
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	obj->mesh = AssetManager_LookupMesh("Cube");
	obj->texture = AssetManager_LookupTexture("Wall");

	// Initialize a collider
	obj->collider = Collider_Allocate();
	AABBCollider_Initialize(obj->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	vector.components[0] = 0.0f;
	vector.components[1] = 0.0f;
	vector.components[2] = -49.0f;

	GObject_Translate(obj, &vector);


	vector.components[0] = 50.0f;
	vector.components[2] = 1.0f;
	vector.components[1] = 50.0f;

	GObject_Scale(obj, &vector);
	ObjectManager_AddObject(obj);

	///
	//Create front wall
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	obj->mesh = AssetManager_LookupMesh("Cube");
	obj->texture = AssetManager_LookupTexture("Wall");

	// Initialize a collider
	obj->collider = Collider_Allocate();
	AABBCollider_Initialize(obj->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);


	vector.components[0] = 0.0f;
	vector.components[1] = 0.0f;
	vector.components[2] = 49.0f;

	GObject_Translate(obj, &vector);


	vector.components[0] = 50.0f;
	vector.components[2] = 1.0f;
	vector.components[1] = 50.0f;

	GObject_Scale(obj, &vector);
	ObjectManager_AddObject(obj);

	///
	//Create left wall
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	obj->mesh = AssetManager_LookupMesh("Cube");
	obj->texture = AssetManager_LookupTexture("Wall");

	// Initialize a collider
	obj->collider = Collider_Allocate();
	AABBCollider_Initialize(obj->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	vector.components[0] = -49.0f;
	vector.components[1] = 0.0f;
	vector.components[2] = 0.0f;

	GObject_Translate(obj, &vector);


	vector.components[0] = 1.0f;
	vector.components[2] = 50.0f;
	vector.components[1] = 50.0f;

	GObject_Scale(obj, &vector);
	ObjectManager_AddObject(obj);

	///
	//Create right wall
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	obj->mesh = AssetManager_LookupMesh("Cube");
	obj->texture = AssetManager_LookupTexture("Wall");

	// Initialize a collider
	obj->collider = Collider_Allocate();
	AABBCollider_Initialize(obj->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	vector.components[0] = 49.0f;
	vector.components[1] = 0.0f;
	vector.components[2] = 0.0f;

	GObject_Translate(obj, &vector);


	vector.components[0] = 1.0f;
	vector.components[2] = 50.0f;
	vector.components[1] = 50.0f;

	GObject_Scale(obj, &vector);
	ObjectManager_AddObject(obj);

	// Create the top pillar of the shooting gallery
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	obj->mesh = AssetManager_LookupMesh("Cube");
	obj->texture = AssetManager_LookupTexture("Table");

	// Initialize a collider
	obj->collider = Collider_Allocate();
	AABBCollider_Initialize(obj->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	vector.components[0] = 0.0f; 
	vector.components[1] = 3.0f;
	vector.components[2] = 4.0f;

	GObject_Translate(obj, &vector);

	vector.components[0] = 45.0f;
	vector.components[2] = 5.0f;
	vector.components[1] = 1.0f;

	GObject_Scale(obj, &vector);
	
	// Add Wall into scene
	ObjectManager_AddObject(obj);

	// Create the bottom pillar of the shooting gallery
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	obj->mesh = AssetManager_LookupMesh("Cube");
	obj->texture = AssetManager_LookupTexture("Table");

	// Initialize a collider
	obj->collider = Collider_Allocate();
	AABBCollider_Initialize(obj->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	vector.components[0] = 0.0f;
	vector.components[1] = -9.4f;
	vector.components[2] = 0.0f;

	GObject_Translate(obj, &vector);

	vector.components[0] = 45.0f;
	vector.components[2] = 0.75f;
	vector.components[1] = 1.0f;

	GObject_Scale(obj, &vector);

	// Add Wall into scene
	ObjectManager_AddObject(obj);

	// Create the vertical pillars of the shooting gallery
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	obj->mesh = AssetManager_LookupMesh("Cube");
	obj->texture = AssetManager_LookupTexture("Table");

	// Initialize a collider
	obj->collider = Collider_Allocate();
	AABBCollider_Initialize(obj->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	vector.components[0] = 10.0f;
	vector.components[1] = -2.0f;
	vector.components[2] = 1.5f;

	GObject_Translate(obj, &vector);

	vector.components[0] = 1.0f;
	vector.components[2] = 2.0f;
	vector.components[1] = 7.0f;

	GObject_Scale(obj, &vector);

	// Add Wall into scene
	ObjectManager_AddObject(obj);

	// Create the vertical pillars of the shooting gallery
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	obj->mesh = AssetManager_LookupMesh("Cube");
	obj->texture = AssetManager_LookupTexture("Table");

	// Initialize a collider
	obj->collider = Collider_Allocate();
	AABBCollider_Initialize(obj->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	vector.components[0] = -10.0f;
	vector.components[1] = -2.0f;
	vector.components[2] = 1.5f;

	GObject_Translate(obj, &vector);

	vector.components[0] = 1.0f;
	vector.components[2] = 2.0f;
	vector.components[1] = 7.0f;

	GObject_Scale(obj, &vector);

	// Add Wall into scene
	ObjectManager_AddObject(obj);

	// Create the vertical pillars of the shooting gallery
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	obj->mesh = AssetManager_LookupMesh("Cube");
	obj->texture = AssetManager_LookupTexture("Table");

	// Initialize a collider
	obj->collider = Collider_Allocate();
	AABBCollider_Initialize(obj->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	vector.components[0] = -24.0f;
	vector.components[1] = -2.0f;
	vector.components[2] = 1.5f;

	GObject_Translate(obj, &vector);

	vector.components[0] = 1.0f;
	vector.components[2] = 2.0f;
	vector.components[1] = 7.0f;

	GObject_Scale(obj, &vector);


	// Add Wall into scene
	ObjectManager_AddObject(obj);

	// Create the vertical pillars of the shooting gallery
	obj = GObject_Allocate();
	GObject_Initialize(obj);

	obj->mesh = AssetManager_LookupMesh("Cube");
	obj->texture = AssetManager_LookupTexture("Table");

	// Initialize a collider
	obj->collider = Collider_Allocate();
	AABBCollider_Initialize(obj->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	vector.components[0] = 24.0f;
	vector.components[1] = -2.0f;
	vector.components[2] = 1.5f;

	GObject_Translate(obj, &vector);

	vector.components[0] = 1.0f;
	vector.components[2] = 2.0f;
	vector.components[1] = 7.0f;

	GObject_Scale(obj, &vector);


	// Add Wall into scene
	ObjectManager_AddObject(obj);

	//Set gravity
	Vector* gravity = Vector_Allocate();
	Vector_Initialize(gravity, 3);
	gravity->components[1] = -9.81f;
	
	PhysicsManager_AddGlobalAcceleration(gravity);
}

///
//Initializes all engine components
void Init(void)
{

	//Initialize managers
	InputManager_Initialize();
	RenderingManager_Initialize();
	AssetManager_Initialize();
	ObjectManager_Initialize();
	CollisionManager_Initialize();
	PhysicsManager_Initialize();

	//Load assets
	AssetManager_LoadAssets();


	//Cuda Testing
#ifdef ENABLE_CUDA
	//CUDA Test
	//Create Host variables
	int numVectors = 5;
	int vectorDim = 3;

	Vector* dotProd = Vector_Allocate();
	Vector_Initialize(dotProd, numVectors);

	Vector* scaledDotProd = Vector_Allocate();
	Vector_Initialize(scaledDotProd, numVectors);

	Vector* dests = Vector_Allocate();
	Vector_Initialize(dests, vectorDim);


	Vector** srcs = (Vector**)malloc(sizeof(Vector*)* numVectors);
	Vector** srcs2 = (Vector**)malloc(sizeof(Vector*)* numVectors);
	for (int i = 0; i < numVectors; i++)
	{


		srcs[i] = Vector_Allocate();
		srcs2[i] = Vector_Allocate();

		Vector_Initialize(srcs[i], vectorDim);
		Vector_Initialize(srcs2[i], vectorDim);

		//Set vectors value to 1
		for (int j = 0; j < vectorDim; j++)
		{	
			srcs[i]->components[j] = j;
			srcs2[i]->components[j] = 10;
		}

	}

	//Create device variables
	AcceleratedVector* aSrcs = AcceleratedVector_Allocate();
	AcceleratedVector* aSrcs2 = AcceleratedVector_Allocate();
	AcceleratedVector* aDests = AcceleratedVector_Allocate();
	AcceleratedVector* aDotProd = AcceleratedVector_Allocate();
	AcceleratedVector* aScaledDotProd = AcceleratedVector_Allocate();


	AcceleratedVector_Initialize(aSrcs, numVectors * vectorDim);
	AcceleratedVector_Initialize(aSrcs2, numVectors * vectorDim);
	AcceleratedVector_Initialize(aDests, vectorDim);
	AcceleratedVector_Initialize(aDotProd, numVectors);
	AcceleratedVector_Initialize(aScaledDotProd, numVectors);


	//Copy memory from host to device
	AcceleratedVector_CopyVector(aDests, dests);
	AcceleratedVector_CopyVector(aDotProd, dotProd);
	AcceleratedVector_CopyVector(aScaledDotProd, scaledDotProd);

	//Copy a metric fuckton of memory from the host to the device
	AcceleratedVector_CopyVectors(aSrcs, (const Vector**) srcs, vectorDim, numVectors);
	AcceleratedVector_CopyVectors(aSrcs2, (const Vector**) srcs2, vectorDim, numVectors);

	float scalar = 0.0625f;

	float* d_mag;
	float mag = 0.0f;
	cudaMalloc((void**)&d_mag, sizeof(float)* 1);

	//Launch kernel on GPU
	AcceleratedVector_LaunchAddAll(aDests->d_components, aSrcs->d_components, vectorDim, numVectors);
	AcceleratedVector_LaunchMagnitude(d_mag, aDests->d_components, vectorDim);
	AcceleratedVector_LaunchDotProductAll(aDotProd->d_components, aSrcs->d_components, aSrcs2->d_components, vectorDim, numVectors);
	AcceleratedVector_LaunchGetNormalize(aScaledDotProd->d_components, aDotProd->d_components, aDotProd->dimension);
	AcceleratedVector_LaunchProjectAll(aSrcs->d_components, aSrcs2->d_components, vectorDim, numVectors);

	//Copy memory from device to host	
	AcceleratedVector_PasteVector(dests, aDests);
	AcceleratedVector_PasteVector(dotProd, aDotProd);
	AcceleratedVector_PasteVector(scaledDotProd, aScaledDotProd);
	AcceleratedVector_PasteVectors(srcs, aSrcs, vectorDim, numVectors);
	AcceleratedVector_PasteVectors(srcs2, aSrcs2, vectorDim, numVectors);

	cudaMemcpy(&mag, d_mag, sizeof(float)* 1, cudaMemcpyDeviceToHost);

	//Print results
	Vector_Print(dests);
	Vector_Print(dotProd);
	Vector_Print(scaledDotProd);

	for (int i = 0; i < numVectors; i++)
	{
		Vector_Print(srcs[i]);
	}

	//Free device memory
	AcceleratedVector_Free(aDests);
	AcceleratedVector_Free(aSrcs);
	AcceleratedVector_Free(aSrcs2);
	AcceleratedVector_Free(aDotProd);
	AcceleratedVector_Free(aScaledDotProd);

	cudaFree(d_mag);

	//Free host memory

	for (int i = 0; i < numVectors; i++)
	{
		Vector_Free(srcs[i]);
		Vector_Free(srcs2[i]);
	}

	Vector_Free(dotProd);
	Vector_Free(dests);
	Vector_Free(scaledDotProd);

#endif

	//Initialize the scene
	InitializeScene();

	CheckGLErrors();

	//Time manager must always be initialized last
	TimeManager_Initialize();
}

void CalculateOctTreeCollisions(struct OctTree_Node* node)
{
	if(node->children != NULL)
	{
		for(int i = 0; i < 8; i++)
		{
			CalculateOctTreeCollisions(node->children+i);
		}
	}
	else
	{
		if(node->data->size != 0)
		{
			LinkedList* collisions = CollisionManager_UpdateArray((GObject**)node->data->data, node->data->size);
			PhysicsManager_ResolveCollisions(collisions);
		}
	}
}


///
//Updates engine
//
void Update(void)
{

	//Update time manager
	TimeManager_Update();

	/*
	long  dt = TimeManager_GetTimeBuffer().deltaTime->QuadPart;
	timer += dt;
	if (timer >= 100000)
	{
		printf("dt:\t%d\tmicroseconds\n", dt);
		timer = 0;

	}
	*/

	//Update objects.
	ObjectManager_Update();

	//Feature in development
	if(InputManager_IsKeyDown('g'))
	{
		TimeManager_SetTimeScale(0.0f);
	}
	if(InputManager_IsKeyDown('t'))
	{
		TimeManager_SetTimeScale(1.0f);
	}
	if (InputManager_IsKeyDown('r') || InputManager_IsKeyDown('y') || InputManager_IsKeyDown('o') || InputManager_IsKeyDown('p'))
	{
		if (keyTrigger == 0)
		{
			if (InputManager_IsKeyDown('r'))
			{
				TimeManager_ScaleTimeScale(0.9f);
			}
			else if (InputManager_IsKeyDown('y'))
			{
				TimeManager_ScaleTimeScale(1.1f);
			}
			else if (InputManager_IsKeyDown('o'))
			{
				RenderingManager_GetRenderingBuffer()->debugOctTree = 0;
			}
			else if (InputManager_IsKeyDown('p'))
			{
				RenderingManager_GetRenderingBuffer()->debugOctTree = 1;
			}
		}
		keyTrigger = 1;
	}
	else
	{
		keyTrigger = 0;
	}




	PhysicsManager_Update(ObjectManager_GetObjectBuffer().gameObjects);

	//Update the oct tree
	ObjectManager_UpdateOctTree();

	//LinkedList* collisions = CollisionManager_UpdateList(ObjectManager_GetObjectBuffer().gameObjects);

	//OctTree_Node* octTreeRoot = ObjectManager_GetObjectBuffer().octTree->root;
	//CalculateOctTreeCollisions(octTreeRoot);

	LinkedList* collisions = CollisionManager_UpdateOctTree(ObjectManager_GetObjectBuffer().octTree);


	//Pass collisions to physics manager to be resolved
	PhysicsManager_ResolveCollisions(collisions);

	//Update input
	InputManager_Update();

	CheckGLErrors();

}

void DrawLoop(int val)
{
	glutPostRedisplay();
	glutTimerFunc(val, DrawLoop, 16);


}

///
//Draws the current state of the engine
void Draw(void)
{
	RenderingManager_Render(ObjectManager_GetObjectBuffer().gameObjects);
}


///
//Program will begin here
//
//Parameters:
//	int argc - Number of arguments passed from cmd line
//	char* argv[] - Array of C strings of arguments passed from cmd line
int main(int argc, char* argv[])
{
	//Initialize glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(600, 0);
	glutInitContextVersion(4, 3);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	//Window creation
	int win = glutCreateWindow("NGenVS V3.8: Shooting Gallery");

	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK) { return -1; }


	//Check for errors
	CheckGLErrors();

	///
	//Set up callback registration
	//
	glutIdleFunc(Update);
	glutTimerFunc(16, DrawLoop, 16);
	glutDisplayFunc(Draw);


	//Callback registration for Input
	glutPassiveMotionFunc(InputManager_OnMouseMove);
	glutMotionFunc(InputManager_OnMouseDrag);
	glutMouseFunc(InputManager_OnMouseClick);
	glutKeyboardFunc(InputManager_OnKeyPress);
	glutKeyboardUpFunc(InputManager_OnKeyRelease);

	//Initialize engine
	Init();

	//Start the main loop
	glutMainLoop();

	//When the loop is over, release all memory
	glutDestroyWindow(win);

	InputManager_Free();
	RenderingManager_Free();
	ObjectManager_Free();
	AssetManager_Free();
	CollisionManager_Free();
	PhysicsManager_Free();
	TimeManager_Free();

	return 0;
}
