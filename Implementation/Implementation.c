#include "Implementation.h"

#include "../State/Score.h"
#include "../State/Reset.h"
#include "../State/ApplyForce.h"
#include "../State/CharacterController.h"

/// Add Bottle Function
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
//Initializes the scene within the engine.
//This must be done after all engine components are initialized.
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
	//AABBCollider_Initialize(cam->collider,3.0f,3.0f,3.0f,&Vector_ZERO);
	ConvexHullCollider_Initialize(cam->collider);
	ConvexHullCollider_MakeCubeCollider(cam->collider->data->convexHullData, 3.0f);
	
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
	//AddMovingTarget(obj, -20.0f, 5.0f, -30.0f);

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
