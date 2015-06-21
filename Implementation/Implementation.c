#include "Implementation.h"

#include "../State/ParkourController.h"


///
//Initializes the scene within the engine.
//This must be done after all engine components are initialized.
void InitializeScene(void)
{
	///
	//Camera controller simulation
	GObject* cam = GObject_Allocate();
	GObject_Initialize(cam);

	cam->body = RigidBody_Allocate();
	RigidBody_Initialize(cam->body, cam->frameOfReference, 1.0f);

	cam->body->freezeRotation = 1;
	cam->body->coefficientOfRestitution = 0.0f;

	cam->collider = Collider_Allocate();
	Vector trans;
	Vector_INIT_ON_STACK(trans, 3);
	trans.components[1] = -1.0f;
	AABBCollider_Initialize(cam->collider, 2.0f, 2.0f, 2.0f, &trans);
	//cam->collider->debug = 1;

	State* state = State_Allocate();

	State_ParkourController_Initialize(state, 1.0f, 7.0f, 100.0f, 3.0f);

	GObject_AddState(cam,state);

	ObjectManager_AddObject(cam);

	//Create floor
	GObject* block = GObject_Allocate();
	GObject_Initialize(block);

	block->mesh = AssetManager_LookupMesh("Cube");

	block->collider = Collider_Allocate();
	AABBCollider_Initialize(block->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 0.0f);
	block->body->freezeTranslation = block->body->freezeRotation = 1;
	block->body->dynamicFriction = block->body->staticFriction = 0.1f;

	Vector v;
	Vector_INIT_ON_STACK(v, 3);
	v.components[0] = v.components[2] = 40.0f;
	v.components[1] = 1.0f;

	GObject_Scale(block, &v);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[1] = -10.0f;

	GObject_Translate(block, &v);

	ObjectManager_AddObject(block);

	
	//Set gravity
	Vector* gravity = Vector_Allocate();
	Vector_Initialize(gravity, 3);
	gravity->components[1] = -9.81f;
	
	PhysicsManager_AddGlobalAcceleration(gravity);

}