#if defined __linux__

//Enable POSIX definitions for timespec
#if __STDC_VERSION__ >= 199901L
#define _XOPEN_SOURCE 600
#else
#define _XOPEN_SOURCE 500
#endif

#include <time.h>

#endif


#include "EnvironmentBuilder.h"

#include <math.h>
#include <float.h>

#include "../Data/LinkedList.h"

#include "../Manager/AssetManager.h"
#include "../Manager/EnvironmentManager.h"
#include "../Manager/RenderingManager.h"
#include "../Manager/TimeManager.h"

#include "ProximityRemove.h"

struct State_EnvironmentBuilder_Members
{
	LinkedList* objs;
	float currentOffset;
	float offsetToSpawn;
	float currentSize;
	unsigned short floorPartitions;

	float loseRate;
	float limit;

	unsigned char hard;
};

enum State_EnvironmentBuilder_ObstacleType
{
	State_EnvironmentBuilder_ObstacleType_HURDLE,
	State_EnvironmentBuilder_ObstacleType_WALLCLIMB,
	State_EnvironmentBuilder_ObstacleType_WALLRUN,
	State_EnvironmentBuilder_ObstacleType_CEILING,
	State_EnvironmentBuilder_ObstacleType_PILLAR,
	State_EnvironmentBuilder_ObstacleType_NUMOBSTACLES
};

///
//Randomly alters the EnvironmentBuilder's current set of objects
//to modify the environment
//
//Parameters:
//	members: The state members struct of the EnvironmentBuilder state being modified
static void State_EnvironmentBuilder_Mutate(struct State_EnvironmentBuilder_Members* members);

///
//Spawns another set of the EnvironmentBuilder's object set at the current offset + the current size
//
//Parameters:
//	members: The state members struct of the EnvironmentBuilder state being modified
//	attached: A pointer to the gameObject this state is attached to
static void State_EnvironmentBuilder_Spawn(struct State_EnvironmentBuilder_Members* members, GObject* attached);

///
//Adds a hurdle obstacle to the list of objects in the set
//at the specified z offset
//
//Parameters:
//	members: A pointer to the members of the environmentBuilder state to which the hudle is being added
//	zOffset: the z position of the new hurdle within the set
//	insertionIndex: The index in thelinked list to insert the hurdle at
static void State_EnvironmentBuilder_AddHurdle(struct State_EnvironmentBuilder_Members* members, float zOffset, int insertionIndex);

///
//Adds a wall climb obstacle to the list of objects in the set
//at the specified z-offset inserted at the correct index in the list
//
//Parameters:
//	members: A pointer to the members of the environmentBuilder state to which the hurdle is being added
//	zOffset: the z position of the new wall climb within the set
//	insertionIndex: the index in the linked list to insert the wallclimb obstacle at
static void State_EnvironmentBuilder_AddWallClimb(struct State_EnvironmentBuilder_Members* members, float zOffset, int insertionIndex);

///
//Adds a wall run obstacle to the list of objects in the set
//at the specified z-offset inserted at the correct index in the list
//
//Parameters:
//	members: A pointer to the members of the environmentbuilder state to which the wall run obstacle is being added
//	zOffset: the z position of the new wall climb within the set
//	insertionIndex: the index in the linked list to insert the wallrun obstacle at
static void State_EnvironmentBuilder_AddWallRun(struct State_EnvironmentBuilder_Members* members, float zOffset, int insertionIndex);

///
//Adds a obstacle to the list of objects in the set
//at the specified z-offset inserted at the correct index in the list
//
//Parameters:
//	members: A pointer to the members of the environmentbuilder state to which the obstacle is being added
//	zOffset: the z position of the new wall climb within the set
//	insertionIndex: the index in the linked list to insert the obstacle at
static void State_EnvironmentBuilder_AddCeiling(struct State_EnvironmentBuilder_Members* members, float zOffset, int insertionIndex);

///
//Adds a obstacle to the list of objects in the set
//at the specified z-offset inserted at the correct index in the list
//
//Parameters:
//	members: A pointer to the members of the environmentbuilder state to which the obstacle is being added
//	zOffset: the z position of the new wall climb within the set
//	insertionIndex: the index in the linked list to insert the obstacle at
static void State_EnvironmentBuilder_AddPillar(struct State_EnvironmentBuilder_Members* members, float zOffset, int insertionIndex);


///
//Allocates and initializes a new GObject in the environment set at the specified location
//
//Parameters:
//	x: x coordinate within the set
//	y: y coordinate within the set
//	z: z coordinate within the set
//	sX: x scale
//	sY: y scale
//	sZ: z scale
static GObject* State_EnvironmentBuilder_GenerateBlock(float x, float y, float z, float sX, float sY, float sZ);

///
//Determines a suitable index in the environment builder's linked list of objects
//where it would make the most sense to insert the next obstacle
//
//Parameters:
//	members: A pointer to the eNvironmentBuilder members which contains the linked list to search
//
//Returns:
//	An integer containing the index in the linked list that the new obstacle should be added
//	this index, i, indicates that the obstacle belongs between the objects at i - 1 and i
static unsigned int State_EnvironmentBuilder_DetermineSuitableIndex(struct State_EnvironmentBuilder_Members* members);

///
//Initializes a EnvironmentBuilder state
//
//Parameters:
//	s: A pointer to the state to initialize
void State_EnvironmentBuilder_Initialize(State* state)
{
	struct State_EnvironmentBuilder_Members* members = malloc(sizeof(struct State_EnvironmentBuilder_Members));
	state->members = members;

	members->objs = LinkedList_Allocate();
	LinkedList_Initialize(members->objs);

	members->currentOffset = -50.0f;
	members->offsetToSpawn = 150.0f;

	members->currentSize = 100.0f;

	members->floorPartitions = 1;


	members->loseRate = 20.0f;
	members->limit = -100.0f;
	members->hard = 1;

	state->State_Update = State_EnvironmentBuilder_Update;
	state->State_Members_Free = State_EnvironmentBuilder_Free;

	//Create floor
	GObject* block = GObject_Allocate();
	GObject_Initialize(block);

	block->mesh = AssetManager_LookupMesh("Cube");
	block->material = Material_Allocate();
	Material_Initialize(block->material, AssetManager_LookupTexture("Test"));



	block->collider = Collider_Allocate();
	AABBCollider_Initialize(block->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 0.0f);
	block->body->freezeTranslation = block->body->freezeRotation = 1;
	block->body->dynamicFriction = block->body->staticFriction = 0.5f;
	block->body->rollingResistance = 0.0f;

	Vector v;
	Vector_INIT_ON_STACK(v, 3);

	v.components[0] = 10.0f;
	v.components[2] = 50.0f;
	v.components[1] = 1.0f;

	GObject_Scale(block, &v);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = 0.0f;
	v.components[1] = -0.5f;
	//v.components[2] = -50.0f;

	GObject_Translate(block, &v);

	LinkedList_Append(members->objs, block);
}

///
//Updates a GObject using a EnvironmentBuilder state
//
//Parameters:
//	obj: A pointer to the GObject being updated
//	state: A pointer to the state updating the GObject
void State_EnvironmentBuilder_Update(GObject* obj, State* state)
{

	//Get reference to camera & state members
	//Camera* cam = RenderingManager_GetRenderingBuffer()->camera;
	struct State_EnvironmentBuilder_Members* members = (struct State_EnvironmentBuilder_Members*)state->members;

	if(members->floorPartitions != 0 && members->objs->size > 10)
	{
		LinkedList_RemoveIndex(members->objs, 0);
		members->floorPartitions--;
	}

	float dt = TimeManager_GetDeltaSec();

	members->limit += members->loseRate * dt * members->hard;

	//BEcause forward is the negative Z axis
	float playerDist = fabs(obj->frameOfReference->position->components[2]);
	if(playerDist < members->limit)
	{	
		//lose
		printf("\n\nLosing\n\n");
	}
	

	//Determine if it is time to spawn the next set of objects
	float dist =  members->currentOffset + members->currentSize - playerDist;
	if(dist <= members->offsetToSpawn)
	{
		printf("Spawning\n\n");

		State_EnvironmentBuilder_Mutate(members);
		State_EnvironmentBuilder_Spawn(members, obj);

		members->currentOffset += members->currentSize;
	}
}

///
//Frees memory allocated by a EnvironmentBuilder state
//
//Parameters:
//	state: The Template state to free
void State_EnvironmentBuilder_Free(State* state)
{
	struct State_EnvironmentBuilder_Members* members = (struct State_EnvironmentBuilder_Members*)state->members;
	
	//Free GObjects in LinkedList
	struct LinkedList_Node* current = members->objs->head;
	struct LinkedList_Node* next = NULL;
	GObject* obj = NULL;
	while(current != NULL)
	{
		next = current->next;
		obj = (GObject*)current->data;
		GObject_Free(obj);

		current = next;
	}

	LinkedList_Free(members->objs);

	free(members);
}

///
//Randomly alters the EnvironmentBuilder's current set of objects
//to modify the environment
//
//Parameters:
//	members: The state members struct of the EnvironmentBuilder state being modified
static void State_EnvironmentBuilder_Mutate(struct State_EnvironmentBuilder_Members* members)
{
	int insertIndex = State_EnvironmentBuilder_DetermineSuitableIndex(members);

	//Step 2: Determine the central Z location of new obstacle
	//For now we will just take the average of the bounds
	float minBound, maxBound;

	struct LinkedList_Node* node = LinkedList_SearchByIndex(members->objs, insertIndex - 1);
	GObject* obj = node->data;
	if(insertIndex <= members->floorPartitions)
	{
		minBound = 0.0f;
	}
	else
	{
		minBound = obj->frameOfReference->position->components[2];
	}

	node = node->next;
	if(node != NULL)
	{
		obj = node->data;
		maxBound = obj->frameOfReference->position->components[2];
	}
	else
	{
		struct LinkedList_Node* firstObstacle = LinkedList_SearchByIndex(members->objs, members->floorPartitions);
		if(firstObstacle != NULL)
		{
			GObject* first = firstObstacle->data;
			maxBound = -members->currentSize + first->frameOfReference->position->components[2];
		}
		else
		{
			maxBound = -members->currentSize;
		}
	}

	float zPos = (maxBound + minBound) / 2.0f;
	//zPos = EnvironmentManager_Randf(minBound, maxBound);

	if(zPos < -members->currentSize) zPos += members->currentSize;

	printf("\n%f\t%d\n", zPos, members->objs->size);

	//Step 3: Choose an obstacle to place
	int random = EnvironmentManager_Rand(0, State_EnvironmentBuilder_ObstacleType_NUMOBSTACLES);

	//TODO: If segment is less than certain distance force extension obstacle

	switch(random)
	{
	case State_EnvironmentBuilder_ObstacleType_HURDLE:
		State_EnvironmentBuilder_AddHurdle(members, zPos, insertIndex);
		break;
	case State_EnvironmentBuilder_ObstacleType_WALLCLIMB:
		State_EnvironmentBuilder_AddWallClimb(members, zPos, insertIndex);
		break;
	case State_EnvironmentBuilder_ObstacleType_WALLRUN:
		State_EnvironmentBuilder_AddWallRun(members, zPos, insertIndex);
		break;
	case State_EnvironmentBuilder_ObstacleType_CEILING:
		State_EnvironmentBuilder_AddCeiling(members, zPos, insertIndex);
		break;
	case State_EnvironmentBuilder_ObstacleType_PILLAR:
		State_EnvironmentBuilder_AddPillar(members, zPos, insertIndex);
		break;
	default:
		break;
	}
}

///
//Determines a suitable index in the environment builder's linked list of objects
//where it would make the most sense to insert the next obstacle
//
//Parameters:
//	members: A pointer to the eNvironmentBuilder members which contains the linked list to search
//
//Returns:
//	An integer containing the index in the linked list that the new obstacle should be added
//	this index, i, indicates that the obstacle belongs between the objects at i - 1 and i
static unsigned int State_EnvironmentBuilder_DetermineSuitableIndex(struct State_EnvironmentBuilder_Members* members)
{
	//Step 1: Determine how many obstacles there are
	int numObstacles = members->objs->size - members->floorPartitions;

	if(numObstacles == 0)
	{
		return members->floorPartitions;	//If there are no obstacles, put the object after the final floor partition
	}
	else if(numObstacles == 1)
	{

		//Get the first object
		struct LinkedList_Node* node = LinkedList_SearchByIndex(members->objs, members->floorPartitions);
		GObject* obj = node->data;

		//get the z offset of object
		float zOff = fabs(obj->frameOfReference->position->components[2]);

		//Determine if the larger gap is between the set start and the single object
		//or the single object and the set end
		float d1 = zOff;
		float d2 = members->currentSize - zOff;

		if(d1 > d2)	return members->floorPartitions;
		else 		return members->floorPartitions + 1;
	}
	else
	{
		//Determine which gap between any two adjacent obstacles is the largest and return the index of
		//the "larger" bound
		int maxIndex = members->floorPartitions;
		int currentIndex = maxIndex;

		struct LinkedList_Node* currentNode = LinkedList_SearchByIndex(members->objs, members->floorPartitions);
		GObject* obj = currentNode->data;

		float zOff1 = 0.0f;
		float zOff2 = fabs(obj->frameOfReference->position->components[2]);
		float first = zOff2;

		float maxDistance = 0.0f;//(zOff2 - zOff1);		//Current maximum distance found between two obstacles
		float currentDistance = maxDistance;

		currentNode = currentNode->next;
		currentIndex++;

		zOff1 = zOff2;

		while(currentNode != NULL)
		{
			obj = currentNode->data;
			
			zOff2 = fabs(obj->frameOfReference->position->components[2]);
			currentDistance = zOff2 - zOff1;

			if(currentDistance > maxDistance)
			{
				maxIndex = currentIndex;
				maxDistance = currentDistance;
			}

			currentNode = currentNode->next;
			currentIndex++;

			zOff1 = zOff2;
		}

		zOff2 = members->currentSize + first;
		currentDistance = (zOff2 - fabs(zOff1));

		if(currentDistance > maxDistance)
		{
			maxIndex = currentIndex;
		}

		return maxIndex;

	}

}


///
//Allocates and initializes a new GObject in the environment set at the specified location
//
//Parameters:
//	x: x coordinate within the set
//	y: y coordinate within the set
//	z: z coordinate within the set
//	sX: x sca
//	sY: y scale
//	sZ: z scale
static GObject* State_EnvironmentBuilder_GenerateBlock(float x, float y, float z, float sX, float sY, float sZ)
{
	GObject* block = GObject_Allocate();
	GObject_Initialize(block);

	block->mesh = AssetManager_LookupMesh("Cube");
	block->material = Material_Allocate();
	Material_Initialize(block->material, AssetManager_LookupTexture("Test"));


	float yaw = EnvironmentManager_Randf(0.0f, 3.14159f * 2.0f);
	float pitch = EnvironmentManager_Randf(0.0f, 3.14159f * 2.0f);
		
	
	*Matrix_Index(block->material->colorMatrix, 0, 0) = cosf(yaw) * sinf(yaw) + .4f;
	*Matrix_Index(block->material->colorMatrix, 1, 1) = cosf(yaw) * sinf(pitch) + .4f;
	*Matrix_Index(block->material->colorMatrix, 2, 2) = cosf(pitch) + .4;	


	block->collider = Collider_Allocate();
	AABBCollider_Initialize(block->collider, 2.0f, 2.0f, 2.0f, &Vector_ZERO);

	block->body = RigidBody_Allocate();
	RigidBody_Initialize(block->body, block->frameOfReference, 0.0f);
	block->body->freezeTranslation = block->body->freezeRotation = 1;
	block->body->dynamicFriction = block->body->staticFriction = 0.5f;
	block->body->rollingResistance = 0.0f;

	block->body->coefficientOfRestitution = 0.01f;

	Vector v;
	Vector_INIT_ON_STACK(v, 3);

	v.components[0] = sX;
	v.components[1] = sY;
	v.components[2] = sZ;

	GObject_Scale(block, &v);

	Vector_Copy(&v, &Vector_ZERO);
	v.components[0] = x;
	v.components[1] = y;
	v.components[2] = z;

	GObject_Translate(block, &v);

	return block;
}

///
//Adds a hurdle obstacle to the list of objects in the set
//at the specified z offset
//
//Parameters:
//	members: A pointer to the members of the environmentBuilder state to which the hudle is being added
//	zOffset: the z position of the new hurdle within the set
//	index: the index to insert the new obstacle at
static void State_EnvironmentBuilder_AddHurdle(struct State_EnvironmentBuilder_Members* members, float zOffset, int index)
{	
	GObject* block = State_EnvironmentBuilder_GenerateBlock(0.0f, 1.0f, zOffset, 10.0f, 2.0f, 1.0f);
	LinkedList_Insert(members->objs, index, block);
}

///
//Adds a wall climb obstacle to the list of objects in the set
//at the specified z-offset inserted at the correct index in the list
//
//Parameters:
//	members: A pointer to the members of the environmentBuilder state to which the hurdle is being added
//	zOffset: the z position of the new wall climb within the set
//	insertionIndex: the index in the linked list to insert the wallclimb obstacle at
static void State_EnvironmentBuilder_AddWallClimb(struct State_EnvironmentBuilder_Members* members, float zOffset, int insertionIndex)
{
	float height = EnvironmentManager_Randf(5.0f, 10.0f);
	float y = EnvironmentManager_Randf(height, 50.0f + height);
	GObject* block = State_EnvironmentBuilder_GenerateBlock(0.0f, y, zOffset, 10.0f, height, 1.0f);

	LinkedList_Insert(members->objs, insertionIndex, block);
}

///
//Adds a wall run obstacle to the list of objects in the set
//at the specified z-offset inserted at the correct index in the list
//
//Parameters:
//	members: A pointer to the members of the environmentbuilder state to which the wall run obstacle is being added
//	zOffset: the z position of the new wall climb within the set
//	insertionIndex: the index in the linked list to insert the wallrun obstacle at
static void State_EnvironmentBuilder_AddWallRun(struct State_EnvironmentBuilder_Members* members, float zOffset, int insertionIndex)
{
	float x = EnvironmentManager_Randf(-18.0f, 18.0f);

	float y = EnvironmentManager_Randf(5.0f, 50.0f);
	float len = EnvironmentManager_Randf(10.0f, 15.0f);

	GObject* block = State_EnvironmentBuilder_GenerateBlock(x, y, zOffset, 1.0f, 6.0f, len);

	block->body->staticFriction = block->body->dynamicFriction = 0.0f;

	LinkedList_Insert(members->objs, insertionIndex, block);

}

///
//Adds a obstacle to the list of objects in the set
//at the specified z-offset inserted at the correct index in the list
//
//Parameters:
//	members: A pointer to the members of the environmentbuilder state to which the obstacle is being added
//	zOffset: the z position of the new wall climb within the set
//	insertionIndex: the index in the linked list to insert the obstacle at
static void State_EnvironmentBuilder_AddCeiling(struct State_EnvironmentBuilder_Members* members, float zOffset, int insertionIndex)
{
	float length = EnvironmentManager_Randf(10.0f, 20.0f);
	float height = EnvironmentManager_Randf(10.0f, 50.0f);
	GObject* block = State_EnvironmentBuilder_GenerateBlock(0.0f, height, zOffset, 10.0f, 1.0f, length);
	LinkedList_Insert(members->objs, insertionIndex, block);
}

///
//Adds a obstacle to the list of objects in the set
//at the specified z-offset inserted at the correct index in the list
//
//Parameters:
//	members: A pointer to the members of the environmentbuilder state to which the obstacle is being added
//	zOffset: the z position of the new wall climb within the set
//	insertionIndex: the index in the linked list to insert the obstacle at
static void State_EnvironmentBuilder_AddPillar(struct State_EnvironmentBuilder_Members* members, float zOffset, int insertionIndex)
{
	float x = EnvironmentManager_Randf(-10.0f, 10.0f);
	float height = EnvironmentManager_Randf(10.0f, 50.0f);
	float y = height;

	GObject* block = State_EnvironmentBuilder_GenerateBlock(x, y, zOffset, 1.0f, height, 1.0f);
	LinkedList_Insert(members->objs, insertionIndex, block);
}

///
//Spawns another set of the EnvironmentBuilder's object set at the current offset + the current size
//
//Parameters:
//	members: The state members struct of the EnvironmentBuilder state being modified
static void State_EnvironmentBuilder_Spawn(struct State_EnvironmentBuilder_Members* members, GObject* attached)
{
	printf("MyPos:\t%f\n", attached->frameOfReference->position->components[2]);

	//for every object in the current set
	struct LinkedList_Node* current = members->objs->head;
	GObject* obj = NULL;
	while(current != NULL)
	{
		obj = (GObject*)current->data;

		//Make a copy
		GObject* copy = GObject_Allocate();
		GObject_InitializeDeepCopy(copy, obj);

		Vector c;
		Vector_INIT_ON_STACK(c, 3);



		float yaw = EnvironmentManager_Randf(0.0f, 3.14159f * 2.0f);
		float pitch = EnvironmentManager_Randf(0.0f, 3.14159f * 2.0f);
		
		c.components[0] = cosf(yaw) * sinf(yaw) + .3f;
		c.components[1] = cosf(yaw) * sinf(pitch) + .3f;
		c.components[2] = cosf(pitch) + .2f;

		Vector_Scale(&c, 3);


		*Matrix_Index(copy->material->colorMatrix, 0, 0) = c.components[0];// > 1.0f ? 1.0f : c.components[0];
		*Matrix_Index(copy->material->colorMatrix, 1, 1) = c.components[1];// > 1.0f ? 1.0f : c.components[1];
		*Matrix_Index(copy->material->colorMatrix, 2, 2) = c.components[2];// > 1.0f ? 1.0f : c.components[2];

		//Vector_PrintTranspose(&c);

		//Translate to current offset + the current set size
		Vector trans;
		Vector_INIT_ON_STACK(trans, 3);
		trans.components[2] -= (members->currentOffset + members->currentSize - 50.0f);

		GObject_Translate(copy, &trans);

		printf("SamplePos:\t%f\n", copy->frameOfReference->position->components[2]);
		
		State* s = State_Allocate();
		State_ProximityRemove_Initialize(s, attached->frameOfReference->position, members->currentSize * 2.5f);

		GObject_AddState(copy, s);

		//Add new copy to object manager
		ObjectManager_AddObject(copy);

		current = current->next;
	}
}

