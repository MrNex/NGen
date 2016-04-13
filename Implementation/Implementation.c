#include "Implementation.h"

#include "../State/FirstPersonCamera.h"
#include "../State/HorizontalController.h"


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

	State_FirstPersonCamera_Initialize(state, 3.0f, 5.0f, 0.05f);

	GObject_AddState(cam,state);

	ObjectManager_AddObject(cam);

	//Set directional light direction
	DirectionalLight* l = RenderingManager_GetRenderingBuffer()->directionalLight;
	l->direction->components[1] = -1.0f;
	l->direction->components[2] = -1.0f;

	Vector_Normalize(l->direction);

	//Create floor
	GObject* block = GObject_Allocate();
	GObject_Initialize(block);

	block->mesh = AssetManager_LookupMesh("Cube");
	block->material = Material_Allocate();
	Material_Initialize(block->material, AssetManager_LookupTexture("Granite"));
	block->material->tile->components[0] = 16.0f;
	block->material->tile->components[1] = 24.0f;
	block->material->specularCoefficient = 1.0f;
	block->material->specularPower = 32.0f;
	block->material->ambientCoefficient = 0.4f;


	//*Matrix_Index(block->material->colorMatrix, 1, 1) = 0.0f;
	//*Matrix_Index(block->material->colorMatrix, 2, 2) = 0.0f;

	block->collider = Collider_Allocate();

	float min[3] = { -1.0f, -1.0f, -1.0f };
	float max[3] = {1.0f, 1.0f, 1.0f};
	AABBCollider_Initialize(block->collider, min, max);

	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 0.0f);
	block->body->freezeTranslation = block->body->freezeRotation = 1;
	block->body->dynamicFriction = block->body->staticFriction = 0.5f;
	block->body->rollingResistance = 0.0f;

	Vector v;
	Vector_INIT_ON_STACK(v, 3);
	v.components[0] = 20.0f;
	v.components[2] = 30.0f;
	v.components[1] = 1.0f;

	GObject_Scale(block, &v);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = -15.0f;
	v.components[1] = -10.0f;
	v.components[2] = -25.0f;

	GObject_Translate(block, &v);

	ObjectManager_AddObject(block);
	
	//Create Large sphere

	block = GObject_Allocate();
	GObject_Initialize(block);

	block->mesh = AssetManager_LookupMesh("Sphere");
	block->material = Material_Allocate();
	Material_Initialize(block->material, AssetManager_LookupTexture("Granite"));
	*Matrix_Index(block->material->colorMatrix, 0, 0) = 0.4f;
	*Matrix_Index(block->material->colorMatrix, 1, 0) = 0.0f;
	*Matrix_Index(block->material->colorMatrix, 1, 1) = 0.4f;
	*Matrix_Index(block->material->colorMatrix, 2, 0) = 0.4f;
	*Matrix_Index(block->material->colorMatrix, 2, 2) = 0.4f;
	//Matrix_Scale(block->material->colorMatrix, 0.4f);
	block->material->specularCoefficient = 2.0f;
	block->material->specularPower = 16.0f;

	block->collider = Collider_Allocate();
	SphereCollider_Initialize(block->collider, 1.0f, &Vector_ZERO);

	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 2.0f);
	block->body->dynamicFriction = block->body->staticFriction = 0.1f;

	block->body->coefficientOfRestitution = 0.9f;
	block->body->rollingResistance = 0.1f;

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = 0.0f;
	v.components[1] = 0.0f;
	v.components[2] = -5.0f;

	GObject_Translate(block, &v);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = v.components[1] = v.components[2] = 2.0f;
	GObject_Scale(block, &v);
	
	ObjectManager_AddObject(block);
	

	//Create small sphere
	
	block = GObject_Allocate();
	GObject_Initialize(block);

	block->mesh = AssetManager_LookupMesh("Sphere");
	block->material = Material_Allocate();
	Material_Initialize(block->material, AssetManager_LookupTexture("Granite"));
	*Matrix_Index(block->material->colorMatrix, 0, 0) = 1.0f;
	*Matrix_Index(block->material->colorMatrix, 1, 0) = 1.0f;
	*Matrix_Index(block->material->colorMatrix, 1, 1) = 1.0f;
	*Matrix_Index(block->material->colorMatrix, 2, 0) = 1.0f;
	*Matrix_Index(block->material->colorMatrix, 2, 2) = 1.0f;
	//Matrix_Scale(block->material->colorMatrix, 0.7f);
	block->material->specularPower = 2.0f;
	block->material->specularCoefficient = 0.4f;

	block->collider = Collider_Allocate();
	SphereCollider_Initialize(block->collider, 1.0f, &Vector_ZERO);

	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 1.0f);
	block->body->dynamicFriction = block->body->staticFriction = 0.08f;

	block->body->coefficientOfRestitution = 0.8f;
	block->body->rollingResistance = 0.1f;

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = -3.0f;
	v.components[1] = -2.0f;
	v.components[2] = -6.6f;

	GObject_Translate(block, &v);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = v.components[1] = v.components[2] = 1.8f;
	GObject_Scale(block, &v);
	
	ObjectManager_AddObject(block);

	//Create box
	block = GObject_Allocate();
	GObject_Initialize(block);

	state = State_Allocate();
	State_HorizontalController_Initialize(state, 0.2f);

	GObject_AddState(block, state);

	block->mesh = AssetManager_LookupMesh("Cube");
	block->material = Material_Allocate();
	Material_Initialize(block->material, AssetManager_LookupTexture("White"));

	block->collider = Collider_Allocate();
	float s[3] = { -1.0f, -1.0f, -1.0f };
	float b[3] = { 1.0f, 1.0f, 1.0f };
	AABBCollider_Initialize(block->collider, s, b);

	v.components[0] = 0.0f;
	v.components[1] = 0.0f;
	v.components[2] = 0.0f;
	GObject_Translate(block, &v);

	ObjectManager_AddObject(block);
	

	
	//Set gravity	
	Vector* gravity = Vector_Allocate();
	Vector_Initialize(gravity, 3);
	gravity->components[1] = -9.81f;
	
	PhysicsManager_AddGlobalAcceleration(gravity);	

}
