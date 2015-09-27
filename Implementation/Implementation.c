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

	State* state = State_Allocate();

	cam->body = RigidBody_Allocate();
	RigidBody_Initialize(cam->body, cam->frameOfReference, 1.0f);
	cam->body->coefficientOfRestitution = 0.1f;


	cam->collider = Collider_Allocate();
	AABBCollider_Initialize(cam->collider, 2.5f, 3.0f, 2.5f, &Vector_ZERO);

	State_ParkourController_Initialize(state, 7.0f, 10.0f, 0.05f, 50.0f, 0.1f);

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
	block->body->rollingResistance = 0.25f;

	Vector v;
	Vector_INIT_ON_STACK(v, 3);
	v.components[0] = v.components[2] = 40.0f;
	v.components[1] = 1.0f;

	GObject_Scale(block, &v);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[1] = -10.0f;

	GObject_Translate(block, &v);

	ObjectManager_AddObject(block);
	
	//Create sphere

	block = GObject_Allocate();
	GObject_Initialize(block);

	block->mesh = AssetManager_LookupMesh("Sphere");
	block->material = Material_Allocate();
	Material_Initialize(block->material, AssetManager_LookupTexture("Earth"));

	//*Matrix_Index(block->material->colorMatrix, 2, 2) = 0.0f;
	//*Matrix_Index(block->material->colorMatrix, 1, 1) = 0.0f;


	block->collider = Collider_Allocate();
	SphereCollider_Initialize(block->collider, 1.0f);

	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 0.0f);
	block->body->dynamicFriction = block->body->staticFriction = 1.0f;

	block->body->coefficientOfRestitution = 0.9f;

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = -3.0f;
	v.components[1] = 5.0f;
	v.components[2] = -10.0f;

	GObject_Translate(block, &v);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = v.components[1] = v.components[2] = 10.0f;
	GObject_Scale(block, &v);
	
	ObjectManager_AddObject(block);
	
	
	
	//Set gravity
	
	Vector* gravity = Vector_Allocate();
	Vector_Initialize(gravity, 3);
	gravity->components[1] = -9.81f;
	
	PhysicsManager_AddGlobalAcceleration(gravity);	
}
