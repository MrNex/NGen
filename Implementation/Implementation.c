#include "Implementation.h"

#include "../State/FirstPersonCamera.h"


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

	State_FirstPersonCamera_Initialize(state, 3.0f, 50.0f, 0.05f);

	GObject_AddState(cam,state);

	ObjectManager_AddObject(cam);

	//Set directional light direction
	RenderingManager_GetRenderingBuffer()->directionalLight->direction->components[1] = -1.0f;
	RenderingManager_GetRenderingBuffer()->directionalLight->base->diffuseIntensity = 0.01f;
	RenderingManager_GetRenderingBuffer()->directionalLight->base->ambientIntensity = 0.00f;

	//Create floor
	GObject* block = GObject_Allocate();
	GObject_Initialize(block);

	block->mesh = AssetManager_LookupMesh("Cube");
	block->material = Material_Allocate();
	Material_Initialize(block->material, AssetManager_LookupTexture("White"));
	block->material->tile->components[0] = 1.0f;
	block->material->tile->components[1] = 3.0f;


	//*Matrix_Index(block->material->colorMatrix, 1, 1) = 0.0f;
	//*Matrix_Index(block->material->colorMatrix, 2, 2) = 0.0f;

	block->collider = Collider_Allocate();
	AABBCollider_Initialize(block->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 0.0f);
	block->body->freezeTranslation = block->body->freezeRotation = 1;
	block->body->dynamicFriction = block->body->staticFriction = 0.5f;
	block->body->rollingResistance = 0.0f;

	Vector v;
	Vector_INIT_ON_STACK(v, 3);
	v.components[0] = 20.0f;
	v.components[2] = 60.0f;
	v.components[1] = 1.0f;

	GObject_Scale(block, &v);

	Vector_Copy(&v, &Vector_ZERO);
	//v.components[0] = -15.0f;
	v.components[1] = -10.0f;

	GObject_Translate(block, &v);

	ObjectManager_AddObject(block);
	
	//Create Large sphere

	block = GObject_Allocate();
	GObject_Initialize(block);

	block->mesh = AssetManager_LookupMesh("Sphere");
	block->material = Material_Allocate();
	Material_Initialize(block->material, AssetManager_LookupTexture("Test"));

	block->collider = Collider_Allocate();
	SphereCollider_Initialize(block->collider, 1.0f);

	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 2.0f);
	block->body->dynamicFriction = block->body->staticFriction = 0.1f;

	block->body->coefficientOfRestitution = 0.9f;
	block->body->rollingResistance = 0.1f;

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = 0.0f;
	v.components[1] = 0.0f;
	v.components[2] = -0.0f;

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
	Material_Initialize(block->material, AssetManager_LookupTexture("Test"));
	*Matrix_Index(block->material->colorMatrix, 0, 0) = 0.0f;
	//*Matrix_Index(block->material->colorMatrix, 1, 1) = 0.5f;

	block->collider = Collider_Allocate();
	SphereCollider_Initialize(block->collider, 1.0f);

	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 1.0f);
	block->body->dynamicFriction = block->body->staticFriction = 0.08f;

	block->body->coefficientOfRestitution = 0.8f;
	block->body->rollingResistance = 0.1f;

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = -3.0f;
	v.components[1] = -2.0f;
	v.components[2] = -6.8f;

	GObject_Translate(block, &v);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = v.components[1] = v.components[2] = 1.8f;
	GObject_Scale(block, &v);
	
	ObjectManager_AddObject(block);


	//Create light
	block = GObject_Allocate();
	GObject_Initialize(block);

	block->light = PointLight_Allocate();
	v.components[0] = v.components[1] = 0.0f;
	v.components[2] = 1.0f;
	PointLight_Initialize(block->light, &Vector_ZERO, &v, 0.3f, 1.0f, 0.05f, -0.1f, 0.05f);

	//v.components[0] = -5.0f;
	v.components[1] = -10.0f;
	//v.components[2] = -10.0f;
	GObject_Translate(block, &v);

	ObjectManager_AddObject(block);

	//Create light 2
	block = GObject_Allocate();
	GObject_Initialize(block);

	block->light = PointLight_Allocate();
	v.components[0] = 1.0f;
	v.components[1] = v.components[2] = 0.0f;
	PointLight_Initialize(block->light, &Vector_ZERO, &v, 0.3f, 1.0f, 0.05f, -0.1f, 0.05f);

	v.components[0] = -3.0f;
	v.components[1] = -10.0f;
	v.components[2] = -3.0f;
	GObject_Translate(block, &v);

	ObjectManager_AddObject(block);

	//Create light 3
	block = GObject_Allocate();
	GObject_Initialize(block);

	block->light = PointLight_Allocate();
	v.components[0] = 0.0f;
	v.components[1] = 1.0f;
	v.components[2] = 0.0f;
	PointLight_Initialize(block->light, &Vector_ZERO, &v, 0.3f, 1.0f, 0.1f, -0.1f, 0.05f);

	v.components[0] = 3.0f;
	v.components[1] = -10.0f;
	v.components[2] = -3.0f;
	GObject_Translate(block, &v);

	ObjectManager_AddObject(block);



		

	
	//Set gravity
		
	Vector* gravity = Vector_Allocate();
	Vector_Initialize(gravity, 3);
	gravity->components[1] = -9.81f;
	
	PhysicsManager_AddGlobalAcceleration(gravity);	
	

}
