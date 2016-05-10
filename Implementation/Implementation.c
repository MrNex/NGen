#include "Implementation.h"

#include "../State/FirstPersonCamera.h"
#include "../State/HorizontalController.h"
#include "../State/MultivectorRevolution.h"
#include "../State/Shoot.h"

///
//Initializes the scene within the engine.
//This must be done after all engine components are initialized.
void InitializeScene(void)
{

	State* state;

	///
	//Camera controller simulation
	//GObject* cam = GObject_Allocate();

	
	unsigned int camID = GObject_Request();
	GObject* cam = ObjectManager_LookupObject(camID);
	GObject_Initialize(cam);

	state = State_Allocate();

	State_FirstPersonCamera_Initialize(state, 10.0f, 0.01f, 0.05f);

	GObject_AddState(cam,state);

	state = State_Allocate();
	State_Shoot_Initialize(state, 5.0f, 5.0f, 0);
	GObject_AddState(cam, state);
	
	
	//ObjectManager_AddObject(cam);

	//Set directional light direction
	DirectionalLight* l = RenderingManager_GetRenderingBuffer()->directionalLight;
	l->direction->components[1] = -1.0f;
	l->direction->components[2] = -0.3f;
	l->direction->components[0] = -1.0f;

	Vector_Normalize(l->direction);

	//l->base->color->components[2] = 0.0f;

	//l->base->ambientIntensity = 0.0f;
	//l->base->diffuseIntensity = 0.0f;

	//Create floor
	//GObject* block = GObject_Allocate();
	unsigned int blockID = GObject_Request();
	GObject* block = ObjectManager_LookupObject(blockID);
	GObject_Initialize(block);

	block->mesh = AssetManager_LookupMesh("Cube");
	block->materialID = Material_Allocate();
	GLuint texID = AssetManager_LookupTextureID("Wood");
	Material_Initialize(block->materialID, texID);

	Material* material = MemoryPool_RequestAddress(assetBuffer->materialPool, block->materialID);

	material->tile[0] = 2.0f;
	material->tile[1] = 3.0f;
	material->specularCoefficient = 0.0f;
	material->specularPower = 32.0f;
	material->ambientCoefficient = 0.4f;
	material->reflectedCoefficient = 0.0f;
	material->transmittedCoefficient = 0.0f;
	material->localCoefficient = 1.0f;

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
	v.components[0] = -14.0f;
	v.components[1] = -5.5f;
	v.components[2] = -20.0f;

	GObject_Translate(block, &v);

	//ObjectManager_AddObject(block);
	ObjectManager_RegisterObject(blockID);
	
	//Create Large sphere
	//block = GObject_Allocate();
	blockID = GObject_Request();
	block = ObjectManager_LookupObject(blockID);
	GObject_Initialize(block);

	/*
	state = State_Allocate();
	State_HorizontalController_Initialize(state, 0.0001f);
	GObject_AddState(block, state);
	*/

	block->mesh = AssetManager_LookupMesh("Sphere");
	block->materialID = Material_Allocate();
	Material_Initialize(block->materialID, AssetManager_LookupTextureID("White"));
	
	material = MemoryPool_RequestAddress(assetBuffer->materialPool, block->materialID);


	material->specularCoefficient = 0.2f;
	material->specularPower = 20.0f;
	material->diffuseCoefficient = 0.075f;
	material->ambientCoefficient = 0.075f;
	material->localCoefficient = 1.0f;
	material->reflectedCoefficient = 0.01f;
	material->transmittedCoefficient = 0.8f;
	material->indexOfRefraction = 0.95f;


	block->collider = Collider_Allocate();
	SphereCollider_Initialize(block->collider, 1.0f, &Vector_ZERO);

	
	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 2.0f);
	block->body->dynamicFriction = block->body->staticFriction = 0.1f;

	block->body->coefficientOfRestitution = 1.0f;
	block->body->rollingResistance = 0.1f;

	//Vector torque;
	//Vector_INIT_ON_STACK(torque, 3);
	//torque.components[0] = 20.0f;
	//RigidBody_ApplyInstantaneousTorque(block->body, &torque);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = 0.0f;
	v.components[1] = 5.0f;
	v.components[2] = -6.0f;

	GObject_Translate(block, &v);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = v.components[1] = v.components[2] = 2.0f;
	GObject_Scale(block, &v);

		
	//ObjectManager_AddObject(block);
	ObjectManager_RegisterObject(blockID);
	
	//Create small sphere
	//block = GObject_Allocate();
	blockID = GObject_Request();
	block = ObjectManager_LookupObject(blockID);
	GObject_Initialize(block);




	block->mesh = AssetManager_LookupMesh("Sphere");
	block->materialID = Material_Allocate();
	unsigned int temp = AssetManager_LookupTextureID("Checkered");
	Material_Initialize(block->materialID, temp);
/*
	*Matrix_Index(block->material->colorMatrix, 0, 0) = 1.0f;
	*Matrix_Index(block->material->colorMatrix, 1, 0) = 1.0f;
	*Matrix_Index(block->material->colorMatrix, 1, 1) = 1.0f;
	*Matrix_Index(block->material->colorMatrix, 2, 0) = 1.0f;
	*Matrix_Index(block->material->colorMatrix, 2, 2) = 1.0f;
*/
	//Matrix_Scale(block->material->colorMatrix, 0.7f);
	material = MemoryPool_RequestAddress(assetBuffer->materialPool, block->materialID);
	material->specularPower = 20.0f;
	material->ambientCoefficient = 0.2f;
	material->diffuseCoefficient = 0.90f;
	material->specularCoefficient = 1.0f;
	material->localCoefficient = 1.0f;
	material->reflectedCoefficient = 0.85f;
	material->transmittedCoefficient = 0.0f;

	block->collider = Collider_Allocate();
	SphereCollider_Initialize(block->collider, 1.0f, &Vector_ZERO);

	/*
	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 1.0f);
	block->body->dynamicFriction = block->body->staticFriction = 0.08f;
	
	block->body->coefficientOfRestitution = 0.8f;
	block->body->rollingResistance = 0.1f;
	*/
	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = -2.8f;
	v.components[1] = -2.0f;
	v.components[2] = -13.0f;

	GObject_Translate(block, &v);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = v.components[1] = v.components[2] = 1.8f;
	GObject_Scale(block, &v);

	state = State_Allocate();
	State_MultivectorRevolution_Initialize(state, 0.00005f);
	GObject_AddState(block, state);
	
	//ObjectManager_AddObject(block);
	ObjectManager_RegisterObject(blockID);



	//Create sizanne
	/*	
	//block = GObject_Allocate();
	blockID = GObject_Request();
	block = ObjectManager_LookupObject(blockID);
	GObject_Initialize(block);

	block->mesh = AssetManager_LookupMesh("Suzanne");
	block->materialID = Material_Allocate();
	Material_Initialize(block->materialID, AssetManager_LookupTextureID("White"));
	material = MemoryPool_RequestAddress(assetBuffer->materialPool, block->materialID);
	
	//Matrix_Scale(block->material->colorMatrix, 0.7f);
	material->specularPower = 2.0f;
	material->specularCoefficient = 0.4f;
	material->localCoefficient = 0.5f;
	material->reflectedCoefficient = 0.6f;

	material->tile[0] = 2.0f;
	material->tile[1] = 0.5f;
	*/
	/*
	block->collider = Collider_Allocate();
	//SphereCollider_Initialize(block->collider, 1.0f, &Vector_ZERO);
	AABBCollider_Initialize(block->collider, min, max);


	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 1.0f);
	block->body->dynamicFriction = block->body->staticFriction = 0.08f;

	block->body->coefficientOfRestitution = 0.3f;
	block->body->rollingResistance = 0.1f;
	*/
	/*
	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = -10.0f;
	v.components[1] = 0.0f;
	v.components[2] = -10.0f;

	GObject_Translate(block, &v);


	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = 4.0f;
	v.components[1] = 4.0f; 
	v.components[2] = 4.0f;
	GObject_Scale(block, &v);
	
	//ObjectManager_AddObject(block);
	ObjectManager_RegisterObject(blockID);
	*/
	//Create box
	/*	
	block = GObject_Allocate();
	GObject_Initialize(block);

	state = State_Allocate();
	State_HorizontalController_Initialize(state, 0.2f);

	GObject_AddState(block, state);

	block->mesh = AssetManager_LookupMesh("Cube");
	block->material = Material_Allocate();
	Material_Initialize(block->material, AssetManager_LookupTextureID("White"));
	*/
	/*
	block->collider = Collider_Allocate();
	float s[3] = { -1.0f, -1.0f, -1.0f };
	float b[3] = { 1.0f, 1.0f, 1.0f };
	AABBCollider_Initialize(block->collider, s, b);
	*/
	/*
	v.components[0] = v.components[1] = v.components[2] = 1.0f;
	block->light = PointLight_Allocate();
	PointLight_Initialize(block->light, &Vector_ZERO, &v, 0.3f, 5.0f, 0.2f, 0.1f, 0.2f);

	v.components[0] = -5.0f;
	v.components[1] = 3.0f;
	v.components[2] = -5.0f;
	GObject_Translate(block, &v);

	ObjectManager_AddObject(block);
	*/

	
	//Set gravity	
	Vector* gravity = Vector_Allocate();
	Vector_Initialize(gravity, 3);
	gravity->components[1] = -9.81f;
	
	PhysicsManager_AddGlobalAcceleration(gravity);	

}
