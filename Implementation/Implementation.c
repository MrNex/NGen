#include "Implementation.h"

#include "../State/ParkourController.h"
#include "../State/EnvironmentBuilder.h"

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
	RigidBody_Initialize(cam->body, cam->frameOfReference, 75.0f);
	cam->body->coefficientOfRestitution = 0.2f;

	Vector v;
	Vector_INIT_ON_STACK(v, 3);
	v.components[1] = -0.5f;
	
	cam->collider = Collider_Allocate();
	AABBCollider_Initialize(cam->collider, 2.6f, 3.0f, 2.6f, &v); 

	State* state = State_Allocate();

	State_ParkourController_Initialize(state, 1000.0f, 25.0f, 0.01f, 300.0f, 1.0f);

	GObject_AddState(cam,state);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[1] = 10.0f;

	GObject_Translate(cam, &v);

	ObjectManager_AddObject(cam);

	//Set directional light direction
	//RenderingManager_GetRenderingBuffer()->directionalLight->direction->components[1] = -1.0f;

	//Create floor
	/*
	GObject* block = GObject_Allocate();
	GObject_Initialize(block);

	block->mesh = AssetManager_LookupMesh("Cube");
	block->material = Material_Allocate();
	Material_Initialize(block->material, AssetManager_LookupTexture("Test"));
	block->material->tile->components[0] = 1.0f;
	block->material->tile->components[1] = 3.0f;


	*Matrix_Index(block->material->colorMatrix, 1, 1) = 0.0f;
	*Matrix_Index(block->material->colorMatrix, 2, 2) = 0.0f;

	block->collider = Collider_Allocate();
	AABBCollider_Initialize(block->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 0.0f);
	block->body->freezeTranslation = block->body->freezeRotation = 1;
	block->body->dynamicFriction = block->body->staticFriction = 0.5f;
	block->body->rollingResistance = 0.0f;

	v.components[0] = 10.0f;
	v.components[2] = 50.0f;
	v.components[1] = 1.0f;

	GObject_Scale(block, &v);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = 0.0f;
	v.components[1] = -0.5f;
	v.components[2] = -50.0f;

	GObject_Translate(block, &v);

	ObjectManager_AddObject(block);
	*/

	State* s = State_Allocate();
	State_EnvironmentBuilder_Initialize(s);

	GObject_AddState(cam, s);


	//Set gravity	
	Vector* gravity = Vector_Allocate();
	Vector_Initialize(gravity, 3);
	gravity->components[1] = -9.81f;
	
	PhysicsManager_AddGlobalAcceleration(gravity);		

}
