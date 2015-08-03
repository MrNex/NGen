#include "CollisionManager.h"
#include <stdio.h>
#include <float.h>
#include <math.h>

///
//Declarations
CollisionBuffer* collisionBuffer;

///
//Allocates memory for a new collision buffer
//
//Returns:
//      pointer to a newly allocated collision buffer
static CollisionBuffer* CollisionManager_AllocateBuffer(void);

///
//Initializes a collision buffer
//
//Parameters:
//      buffer: The buffer to initialize
static void CollisionManager_InitializeBuffer(CollisionBuffer* buffer);

///
//Frees resources allocated by a collision buffer
//
//Parameters:
//      buffer: A pointer to The collision buffer to free
static void CollisionManager_FreeBuffer(CollisionBuffer* buffer);

///
//Allocates memory for a new collision
//
//Returns:
//      Pointer to a newly allocated collision
static struct Collision* CollisionManager_AllocateCollision(void);

///
//Initializes a collision
//All members of the collision are set to NULL with the exception of the MTV
//
//PArameters:
//      collision: The collision to initialize
static void CollisionManager_InitializeCollision(struct Collision* collision);

///
//Tests for collisions on all objects within an oct tree node appending to a list of collisions which occur
//
//Parameters:
//      node: A pointer to the node of the oct tree to check
static void CollisionManager_UpdateOctTreeNode(struct OctTree_Node* node);

///
//Tests for collisions on an array of objects within an oct tree node appending to a list of collisions which occur
//
//Parameters:
//      gameObjects: An array of pointers to game objects to check collisions
//      numObjects: The number of objects in the array
static void CollisionManager_UpdateOctTreeNodeArray(GObject** gameObjects, unsigned int numObjects);

///
//Performs the Separating Axis Theorem test with face normals
//
//Parameters:
//      dest: A pointer to a collision to store the results of the test in. Results will not be stored if the test detects no collision
//      orientedAxes1: An array of pointers to vectors representing the oriented axes of object 1 involved in the test
//      numAxes1: The number of axes belonging to object 1
//      orientedPoints1: An array of pointers to vectors representing the oriented points of object 1 involved in the test
//      numPoints1: The number of points belonging to object 1
//      orientedAxes2: An array of pointersto vectors representing the oriented axes of object 2 involved in the test
//      numAxes2: The number of axes belonging to object 2
//      orientedPoints2: An array of pointers to vectors representing the oriented points of object 2 involved in the test
//      numPoints2: The number of points belonging to object 2
//
//Returns:
//      0 if the test detects no collision
//      1 if the test detects a collision
static unsigned char CollisionManager_PerformSATFaces(struct Collision* dest,
	const Vector** orientedAxes1, const unsigned int numAxes1, const Vector** orientedPoints1, const unsigned int numPoints1,
	const Vector** orientedAxes2, const unsigned int numAxes2, const Vector** orientedPoints2, const unsigned int numPoints2);

///
//Performs the Separating axis theorem test with face normals
//
//Parameters:
//      dest: A pointer to the collision to store the results of the test in. Resultswill not be stored if the test detects no collisions
//      orientedEdges1: An array of pointers to vectors representing the oriented edges of object 1
//      numEdges1: The number of edges belonging to object 1
//      orientedPoints1: An array of pointers to vectors representing the oriented points of object 1
//      numPoints1: The number of points belonging to object 1
//      orientedEdges2: An array of pointers to vectors representing the oriented edges of object 2
//      numEdges2: The number of edges belonging to object 2
//  orientedPoints2: An array of pointers to vectors representing the oriented points of object 2
//      numPoints2: the number of points belonging to object 2
static unsigned char CollisionManager_PerformSATEdges(struct Collision* dest,
	const Vector** orientedEdges1, const unsigned int numEdges1, const Vector** orientedPoints1, const unsigned int numPoints1,
	const Vector** orientedEdges2, const unsigned int numEdges2, const Vector** orientedPoints2, const unsigned int numPoints2);

///
//Projects a set of points onto a normalized axis getting the squared magnitude of the projection vector.
//Finds the minimum and maximum values of the point set projected onto the axis.
//
//Parameters:
//      dest: The destination of the projection bounds
//      axis: The axis projecting onto
//      points: The set of points to project onto the axis
static void CollisionManager_GetProjectionBounds(struct ProjectionBounds* dest, const Vector* axis, const Vector** points, const int numPoints);


///
//Implementations

///
//Initializes the Collision Manager
void CollisionManager_Initialize(void)
{
	//Allocate & initialize collision buffer
	collisionBuffer = CollisionManager_AllocateBuffer();
	CollisionManager_InitializeBuffer(collisionBuffer);
}

///
//Frees any resources allocated by the collision manager
void CollisionManager_Free(void)
{
	CollisionManager_FreeBuffer(collisionBuffer);
}

///
//Tests for collisions on all objects which have colliders 
//compiling a list of game objects which test true
//
//Parameters:
//	gameObjects: THe list of gameObjects to test
//
//Returns:
//	A pointer to a linked list of collisions which occurred this frame.
LinkedList* CollisionManager_UpdateList(LinkedList* gameObjects)
{
	//Clear the current list of collisions
	struct LinkedList_Node* currentNode = collisionBuffer->collisions->head;
	struct LinkedList_Node* nextNode = NULL;
	while(currentNode != NULL)
	{
		nextNode = currentNode->next;
		CollisionManager_FreeCollision((struct Collision*)currentNode->data);
		currentNode = nextNode;
	}
	LinkedList_Clear(collisionBuffer->collisions);

	//Allocates a collision to store the first registered collision
	struct Collision* collision = CollisionManager_AllocateCollision();
	CollisionManager_InitializeCollision(collision);

	//Begin looping through gameObjects
	currentNode = gameObjects->head;
	nextNode = NULL;
	struct LinkedList_Node* iterator = NULL;
	while(currentNode != NULL)
	{
		//Store the next gameObject
		nextNode = currentNode->next;

		//Get current object & make sure it has a collider
		GObject* currentObj = (GObject*)currentNode->data;
		if(currentObj->collider != NULL)
		{

			//Begin looping through the rest of the list (Testing every other object with this object)
			iterator = nextNode;
			while(iterator != NULL)
			{
				//Get the next object & make sure it has a collider
				GObject* iteratorObj = (GObject*)iterator->data;
				if(iteratorObj->collider != NULL)
				{
					CollisionManager_TestCollision( 
						collision,
						currentObj,
						currentObj->body != NULL ? currentObj->body->frame : currentObj->frameOfReference,	//If there is a rigidbody use that frame of reference, else use the objects
						iteratorObj,
						iteratorObj->body != NULL ? iteratorObj->body->frame : iteratorObj->frameOfReference);	//If there is a rigidbody use that frame of reference, else use the objects



					//If they are not colliding continue to test next obj. Do not register collision or perform second pass
					if(collision->obj1 == NULL)
					{
						//Stop any further tests
						iterator = iterator->next;
						continue;
					}


					//If code reaches this point, all tests detected collision.
					//add to collided list
					LinkedList_Append(collisionBuffer->collisions, collision);

					//TODO: Remove
					//Change the color of colliders to red until they are drawn
					*Matrix_Index(currentObj->collider->colorMatrix, 0, 0) = 1.0f;
					*Matrix_Index(currentObj->collider->colorMatrix, 1, 1) = 0.0f;
					*Matrix_Index(currentObj->collider->colorMatrix, 2, 2) = 0.0f;

					*Matrix_Index(iteratorObj->collider->colorMatrix, 0, 0) = 1.0f;
					*Matrix_Index(iteratorObj->collider->colorMatrix, 1, 1) = 0.0f;
					*Matrix_Index(iteratorObj->collider->colorMatrix, 2, 2) = 0.0f;

					//Allocate a new collision for next collision detected
					collision = CollisionManager_AllocateCollision();
					CollisionManager_InitializeCollision(collision);

				}

				iterator = iterator->next;
			}
		}
		currentNode = nextNode;
	}

	//Delete the last unused allocated collision
	CollisionManager_FreeCollision(collision);

	return collisionBuffer->collisions;
}

///
//Tests for collisions on all objects in an oct tree compiling a list of collisions which occur
//
//Parameters:
//	tree: The oct tree holding the game objects to test
//
//Returns: A pointer to a linked list of collisions which occurred this frame
LinkedList* CollisionManager_UpdateOctTree(OctTree* tree)
{
	//Clear the current linked list of collisions
	struct LinkedList_Node* currentNode = collisionBuffer->collisions->head;
	struct LinkedList_Node* nextNode = NULL;
	while(currentNode != NULL)
	{
		nextNode = currentNode->next;
		CollisionManager_FreeCollision((struct Collision*)currentNode->data);
		currentNode = nextNode;
	}
	LinkedList_Clear(collisionBuffer->collisions);

	

	//Update root node to fill the list of collisions with all collisions in the oct tree
	CollisionManager_UpdateOctTreeNode(tree->root);

	//Return the list of collisions
	return collisionBuffer->collisions;
}

///
//Tests for collisions on all objects within an oct tree node appending to a list of collisions which occur
//
//Parameters:
//	node: A pointer to the node of the oct tree to check
static void CollisionManager_UpdateOctTreeNode(struct OctTree_Node* node)
{
	if(node->children != NULL)
	{
		for(int i = 0; i < 8; i++)
		{
			CollisionManager_UpdateOctTreeNode(node->children+i);
		}
	}
	else
	{
		if(node->data->size != 0)
		{
			CollisionManager_UpdateOctTreeNodeArray((GObject**)node->data->data, node->data->size);
			//PhysicsManager_ResolveCollisions(collisions);
		}
	}
}

///
//Tests for collisions on an array of objects within an oct tree node appending to a list of collisions which occur
//
//Parameters:
//	gameObjects: An array of pointers to game objects to check collisions
//	numObjects: The number of objects in the array
static void CollisionManager_UpdateOctTreeNodeArray(GObject** gameObjects, unsigned int numObjects)
{
	//Allocates a collision to store the first registered collision
	struct Collision* collision = CollisionManager_AllocateCollision();
	CollisionManager_InitializeCollision(collision);

	for(unsigned int i = 0; i < numObjects; i++)
	{
		if(gameObjects[i]->collider != NULL)
		{
			for(unsigned int j = i+1; j < numObjects; j++)
			{
				if(gameObjects[j]->collider != NULL)
				{
					CollisionManager_TestCollision( 
						collision,
						gameObjects[i],
						gameObjects[i]->body != NULL ? gameObjects[i]->body->frame : gameObjects[i]->frameOfReference,		//If there is a rigidbody use that frame of reference, else use the objects
						gameObjects[j],
						gameObjects[j]->body != NULL ? gameObjects[j]->body->frame : gameObjects[j]->frameOfReference);	//If there is a rigidbody use that frame of reference, else use the objects

					if(collision->obj1 == NULL)
					{
						continue;
					}

					unsigned char duplicate = 0;

					//Loop through the current collisions for one object
					struct LinkedList_Node* current = collision->obj1->collider->currentCollisions->head;
					while(current != NULL)
					{
						//Ensure that the registered collision is not a duplicate
						struct Collision* currentCollision = (struct Collision*)current->data;
						if(currentCollision->obj1 == collision->obj1 || currentCollision->obj2 == collision->obj1)
						{
							if(currentCollision->obj1 == collision->obj2 || currentCollision->obj2 == collision->obj2)
							{
								duplicate = 1;
							}
						}

						current = current->next;
					}

					//If it is a duplicate, don't add it- just keep looking for collisions
					if(duplicate)
					{
						collision->obj1 = NULL;
						collision->obj2 = NULL;
						collision->overlap = 0.0f;
						collision->obj1Frame = NULL;
						collision->obj2Frame = NULL;
						continue;
					}

					//If code reaches this point, all tests detected collision.
					//add to collided list
					LinkedList_Append(collisionBuffer->collisions, collision);
					

					LinkedList_Append(collision->obj1->collider->currentCollisions, collision);
					LinkedList_Append(collision->obj2->collider->currentCollisions, collision);

					//TODO: Remove
					//Change the color of colliders to red until they are drawn
					*Matrix_Index(gameObjects[i]->collider->colorMatrix, 0, 0) = 1.0f;
					*Matrix_Index(gameObjects[i]->collider->colorMatrix, 1, 1) = 0.0f;
					*Matrix_Index(gameObjects[i]->collider->colorMatrix, 2, 2) = 0.0f;

					*Matrix_Index(gameObjects[j]->collider->colorMatrix, 0, 0) = 1.0f;
					*Matrix_Index(gameObjects[j]->collider->colorMatrix, 1, 1) = 0.0f;
					*Matrix_Index(gameObjects[j]->collider->colorMatrix, 2, 2) = 0.0f;

					//Allocate a new collision for next collision detected
					collision = CollisionManager_AllocateCollision();
					CollisionManager_InitializeCollision(collision);
				}
			}
		}
	}

	//Delete the last unused allocated collision
	CollisionManager_FreeCollision(collision);
}


///
//Tests for collisions on all objects which have colliders
//compiling a list of collisions which occur
//
//Parameters:
//	gameObjects: An array of game objects to test
//
//Returns:
//	A pointer to a linked list of collisions which occurred this frame
LinkedList* CollisionManager_UpdateArray(GObject** gameObjects, unsigned int numObjects)
{
	//Clear the current list of collisions
	struct LinkedList_Node* currentNode = collisionBuffer->collisions->head;
	struct LinkedList_Node* nextNode = NULL;
	while(currentNode != NULL)
	{
		nextNode = currentNode->next;
		struct Collision* currentCollision = (struct Collision*)currentNode->data;
		CollisionManager_FreeCollision(currentCollision);
		currentNode = nextNode;
	}
	LinkedList_Clear(collisionBuffer->collisions);

	//Allocates a collision to store the first registered collision
	struct Collision* collision = CollisionManager_AllocateCollision();
	CollisionManager_InitializeCollision(collision);

	for(unsigned int i = 0; i < numObjects; i++)
	{
		if(gameObjects[i]->collider != NULL)
		{
			for(unsigned int j = i+1; j < numObjects; j++)
			{
				if(gameObjects[j]->collider != NULL)
				{
					CollisionManager_TestCollision( 
						collision,
						gameObjects[i],
						gameObjects[i]->body != NULL ? gameObjects[i]->body->frame : gameObjects[i]->frameOfReference,		//If there is a rigidbody use that frame of reference, else use the objects
						gameObjects[j],
						gameObjects[j]->body != NULL ? gameObjects[j]->body->frame : gameObjects[j]->frameOfReference);	//If there is a rigidbody use that frame of reference, else use the objects

					if(collision->obj1 == NULL)
					{
						continue;
					}

					//If code reaches this point, all tests detected collision.
					//add to collided list
					LinkedList_Append(collisionBuffer->collisions, collision);
					
					//Make copies of the collision to add to object's colliders
					struct Collision* objCollision = CollisionManager_AllocateCollision();
					CollisionManager_InitializeCollision(objCollision);

					//objCollision->minimumTranslationVector = collision->minimumTranslationVector;
					Vector_Copy(objCollision->minimumTranslationVector, collision->minimumTranslationVector);
					objCollision->obj1 = collision->obj1;
					objCollision->obj1Frame = collision->obj1Frame;
					objCollision->obj2 = collision->obj2;
					objCollision->obj2Frame = collision->obj2Frame;
					objCollision->overlap = collision->overlap;

					LinkedList_Append(collision->obj1->collider->currentCollisions, objCollision);

					 objCollision = CollisionManager_AllocateCollision();
					CollisionManager_InitializeCollision(objCollision);

					//objCollision->minimumTranslationVector = collision->minimumTranslationVector;
					Vector_Copy(objCollision->minimumTranslationVector, collision->minimumTranslationVector);
					objCollision->obj1 = collision->obj1;
					objCollision->obj1Frame = collision->obj1Frame;
					objCollision->obj2 = collision->obj2;
					objCollision->obj2Frame = collision->obj2Frame;
					objCollision->overlap = collision->overlap;

					LinkedList_Append(collision->obj2->collider->currentCollisions, objCollision);

					//TODO: Remove
					//Change the color of colliders to red until they are drawn
					*Matrix_Index(gameObjects[i]->collider->colorMatrix, 0, 0) = 1.0f;
					*Matrix_Index(gameObjects[i]->collider->colorMatrix, 1, 1) = 0.0f;
					*Matrix_Index(gameObjects[i]->collider->colorMatrix, 2, 2) = 0.0f;

					*Matrix_Index(gameObjects[j]->collider->colorMatrix, 0, 0) = 1.0f;
					*Matrix_Index(gameObjects[j]->collider->colorMatrix, 1, 1) = 0.0f;
					*Matrix_Index(gameObjects[j]->collider->colorMatrix, 2, 2) = 0.0f;

					//Allocate a new collision for next collision detected
					collision = CollisionManager_AllocateCollision();
					CollisionManager_InitializeCollision(collision);
				}
			}
		}
	}

	//Delete the last unused allocated collision
	CollisionManager_FreeCollision(collision);

	return collisionBuffer->collisions;
}

///
//Tests for a collision between two objects which have colliders
//
//Parameters:
//	dest: Collision to store the results of test in
//	obj1: First game object to test (Must have collider attached)
//	obj1FoR: Pointer to frame of reference to use to orient Object 1
//	obj2: Second game object to test (Must have collider attached)
//	obj2FoR: Pointer to frame of reference to use to orient Object 2
void CollisionManager_TestCollision(struct Collision* dest, GObject* obj1, FrameOfReference* obj1FoR, GObject* obj2, FrameOfReference* obj2FoR)
{
	//Test the types of the colliders
	switch(obj1->collider->type)
	{
	case COLLIDER_SPHERE:
		switch(obj2->collider->type)
		{
		case COLLIDER_SPHERE:						//Sphere on Sphere case
			CollisionManager_TestConvexCollision(
				dest,
				obj1, 
				obj1FoR,
				obj2,
				obj2FoR);

			break;
		case COLLIDER_AABB:							//Sphere on AABB case
			CollisionManager_TestAABBSphereCollision(
				dest,
				obj2,
				obj2FoR,
				obj1,
				obj1FoR);
			break;
		case COLLIDER_CONVEXHULL:					//Sphere on Convex Hull case
			CollisionManager_TestConvexSphereCollision(
				dest,
				obj2,
				obj2FoR,
				obj1,
				obj1FoR);

			break;
		}

		break;
	case COLLIDER_AABB:
		switch(obj2->collider->type)
		{
		case COLLIDER_SPHERE:						//AABB on Sphere case
			CollisionManager_TestAABBSphereCollision(
				dest,
				obj1,
				obj1FoR,
				obj2,
				obj2FoR);

			break;
		case COLLIDER_AABB:							//AABB on AABB case
			CollisionManager_TestAABBCollision(
				dest,
				obj1,
				obj1FoR,
				obj2,
				obj2FoR);

			break;
		case COLLIDER_CONVEXHULL:					//AABB on Convex Hull case
			CollisionManager_TestAABBConvexCollision(
				dest,
				obj1,
				obj1FoR,
				obj2,
				obj2FoR);

			break;
		}

		break;
	case COLLIDER_CONVEXHULL:
		switch(obj2->collider->type)
		{
		case COLLIDER_SPHERE:						//Convex Hull on  Sphere case
			CollisionManager_TestConvexSphereCollision(
				dest,
				obj1,
				obj1FoR,
				obj2,
				obj2FoR);

			break;
		case COLLIDER_AABB:							//Convex Hull on AABB case
			CollisionManager_TestAABBConvexCollision(
				dest,
				obj2,
				obj2FoR,
				obj1,
				obj1FoR);

			break;
		case COLLIDER_CONVEXHULL:					//Convex Hull on  Convex Hull case
			CollisionManager_TestConvexCollision(
				dest,
				obj1,
				obj1FoR,
				obj2,
				obj2FoR);

			break;
		}

		break;
	}
}

///
//TEsts if two game objects bounding spheres are colliding.
//If there is no collision, collision.obj1 and obj2 will be set to null upon
//The end of this function
//
//Parameters:
//	dest: Collision to store the results of test in
//	obj1: First object to test (with collider)
//	obj1FoR: The frame of reference to apply to obj1's scaleFactors for the collision test
//	obj2: Object to test against (with collider)
//	obj2FoR:  The frame of reference to apply obj2's scalefactors for the collision test
void CollisionManager_TestSphereCollision(struct Collision* dest, GObject* obj1, FrameOfReference* obj1FoR, GObject* obj2, FrameOfReference* obj2FoR)
{
	float minOverlap;

	//Grab the Sphere Collider Data to perform test
	struct ColliderData_Sphere* sphere1 = obj1->collider->data->sphereData;
	struct ColliderData_Sphere* sphere2 = obj2->collider->data->sphereData;

	//Before performing the test, we must scale the sphere radius by the max scale value in the frame of reference
	float obj1Radius = SphereCollider_GetScaledRadius(sphere1, obj1FoR);
	float obj2Radius = SphereCollider_GetScaledRadius(sphere2, obj2FoR);

	//Perform test
	Vector displacement;
	Vector_INIT_ON_STACK(displacement, 3);

	Vector_Subtract(&displacement, obj1FoR->position, obj2FoR->position);

	if(Vector_GetMag(&displacement) < obj1Radius + obj2Radius)
	{
		//There is a collision!
		dest->obj1 = obj1;
		dest->obj1Frame = obj1FoR;
		dest->obj2 = obj2;
		dest->obj2Frame = obj2FoR;

		//Get the normalized axis of collision
		//Displacement will always be facing towards obj1
		Vector_Normalize(&displacement);

		//Set MTV
		Vector_Copy(dest->minimumTranslationVector, &displacement);

		//Find the projection bounds
		Vector obj1MinPoint;
		Vector_INIT_ON_STACK(obj1MinPoint, 3);
		Vector obj1MaxPoint;
		Vector_INIT_ON_STACK(obj1MaxPoint, 3);
		Vector obj2MinPoint;
		Vector_INIT_ON_STACK(obj2MinPoint, 3);
		Vector obj2MaxPoint;
		Vector_INIT_ON_STACK(obj2MaxPoint, 3);

		Vector_GetScalarProduct(&obj1MinPoint, &displacement, -obj1Radius);
		Vector_GetScalarProduct(&obj1MaxPoint, &displacement, obj1Radius);
		Vector_GetScalarProduct(&obj2MinPoint, &displacement, -obj2Radius);
		Vector_GetScalarProduct(&obj2MaxPoint, &displacement, obj2Radius);

		Vector_Increment(&obj1MinPoint, obj1FoR->position);
		Vector_Increment(&obj1MaxPoint, obj1FoR->position);
		Vector_Increment(&obj2MinPoint, obj2FoR->position);
		Vector_Increment(&obj2MaxPoint, obj2FoR->position);

		//Get the overlaps in vector form
		Vector ov1;
		Vector_INIT_ON_STACK(ov1, 3);
		Vector ov2;
		Vector_INIT_ON_STACK(ov2, 3);

		Vector_Subtract(&ov1, &obj1MaxPoint, &obj2MinPoint);
		Vector_Subtract(&ov2, &obj2MaxPoint, &obj1MinPoint);

		//Determine & set smallest overlap
		float overlap1 = Vector_GetMag(&ov1);
		float overlap2 = Vector_GetMag(&ov2);

		minOverlap = overlap1 < overlap2 ? overlap1 : overlap2;

		dest->overlap = minOverlap;

		return;
	}
	else
	{
		//No collision, set the collision attributes to null
		dest->overlap = 0.0f;
		dest->obj1 = NULL;
		dest->obj2 = NULL;
		dest->obj1Frame = NULL;
		dest->obj2Frame = NULL;

	}
}

///
//Tests if an object with a AABB is colliding with an object with a bounding sphere
//
//Parameters:
//	dest: Collision to store the results of the test in
//	AABBObj: Pointer to the object which has an axis aligned bounding box
//	AABBFoR: Pointer to the frame of reference to use to test of the object with the AABB
//	sphereObj: Pointer to the object which has a bounding sphere
//	sphereFoR: Pointer to the frame of reference of the object with the bounding sphere
void CollisionManager_TestAABBSphereCollision(struct Collision* dest, GObject* AABBObj, FrameOfReference* AABBFoR, GObject* sphereObj, FrameOfReference* sphereFoR)
{
	//Grab the collider data from both objects
	struct ColliderData_AABB* AABB = AABBObj->collider->data->AABBData;
	struct ColliderData_Sphere* sphere = sphereObj->collider->data->sphereData;
	
	//Translate the objects to a system such that the AABB is centered at the origin
	Vector spherePos;
	Vector_INIT_ON_STACK(spherePos, 3);
	Vector_Copy(&spherePos, sphereFoR->position);
	Vector_Decrement(&spherePos, AABBFoR->position);
	Vector_Decrement(&spherePos, AABB->centroid);


	//Get the scaled collider data from both objects
	struct ColliderData_AABB scaledAABB;
	AABBCollider_GetScaledDimensions(&scaledAABB, AABB, AABBFoR);

	//Find the closest point on the box to the sphere
	//This is done by clamping the sphere's center to the extent of the box in all 3 dimensions
	float halfWidth = scaledAABB.width * 0.5f;
	float halfHeight = scaledAABB.height * 0.5f;
	float halfDepth = scaledAABB.depth * 0.5f;
	
	Vector closestPoint;
	Vector_INIT_ON_STACK(closestPoint, 3);

	closestPoint.components[0] = 
		spherePos.components[0] < -halfWidth ?	//If Condition
			-halfWidth 			//If true
		:					//Else
			spherePos.components[0] < halfWidth ?	//If Condition
				spherePos.components[0]		//If true
			:					//Else
				halfWidth;

	closestPoint.components[1] = 
		spherePos.components[1] < -halfHeight ?	//If Condition
			-halfHeight 			//If true
		:					//Else
			spherePos.components[1] < halfHeight ?	//If Condition
				spherePos.components[1]		//If true
			:					//Else
				halfHeight;

	closestPoint.components[2] = 
		spherePos.components[2] < -halfDepth ?	//If Condition
			-halfDepth 			//If true
		:					//Else
			spherePos.components[2] < halfDepth ?	//If Condition
				spherePos.components[2]		//If true
			:					//Else
				halfDepth;

	//Find the distance between the closest point on the sphere and the box
	Vector distance;
	Vector_INIT_ON_STACK(distance, 3);

	Vector_Subtract(&distance, &spherePos, &closestPoint);

	//Get the scaled radius of the sphere
	float rad = SphereCollider_GetScaledRadius(sphere, sphereFoR);

	//If the magnitude of the distance is smaller than the scaled radius of the sphere, the objects must be colliding
	float mag = Vector_GetMag(&distance);
	if(mag > rad)
	{
		//No collision
		dest->overlap = 0.0f;
		dest->obj1 = NULL;
		dest->obj2 = NULL;
		dest->obj1Frame = NULL;
		dest->obj2Frame = NULL;

		return;
	}

	//printf("Collided\n");

	//If code reaches this point we know there is a collision
	//The MTV will be the vector from the closest point on the box to the center of the sphere
	//Therefore the sphere must be Obj1 because the MTV always points toward OBJ1
	dest->obj1 = sphereObj;
	dest->obj2 = AABBObj;
	dest->obj1Frame = sphereFoR;
	dest->obj2Frame = AABBFoR;
	
	//The distance vector can serve as the MTV
	Vector_Normalize(&distance);
	Vector_Copy(dest->minimumTranslationVector, &distance);

	//The overlap will be the magnitude of the scaled radius - the magnitude of the distance from the closest point to the center
	dest->overlap = rad - mag;
	
}

///
//Tests if two objects with axis aligned bounding boxes are colliding
//
//Parameters:
//	dest: Collision to store the results of test in
//	obj1: Pointer to the first object with an axis aligned bounding box to test.
//	obj1Frame: pointer to Frame of reference to use to orient AABB of obj1
//	obj2: Pointer to the second object with an axis aligned bounding box to test
//	obj2Frame: Pointer to Frame of Reference to use to orient AABB of obj2
void CollisionManager_TestAABBCollision(struct Collision* dest, GObject* obj1, FrameOfReference* obj1Frame, GObject* obj2, FrameOfReference* obj2Frame)
{
	//Get the collider data from both objects
	struct ColliderData_AABB* AABB1 = obj1->collider->data->AABBData;
	struct ColliderData_AABB* AABB2 = obj2->collider->data->AABBData;

	//Scale the collider data from both objects
	struct ColliderData_AABB scaledAABB1;
	struct ColliderData_AABB scaledAABB2;

	AABBCollider_GetScaledDimensions(&scaledAABB1, AABB1, obj1Frame);
	AABBCollider_GetScaledDimensions(&scaledAABB2, AABB2, obj2Frame);

	//If there is a collision, there will be overlap of the opposite bounds of the bounding box on each axis
	//If any axis does not contain any overlap, there is no collision

	//X Axis test
	//Get overlap of Right face of obj1 with left face of obj2
	float overlapRight = (obj1Frame->position->components[0] + AABB1->centroid->components[0] + (scaledAABB1.width / 2.0f)) - 
		((obj2Frame->position->components[0] + AABB2->centroid->components[0]) - (scaledAABB2.width / 2.0f));
	if(overlapRight < 0.0f)
	{
		//No collision, set the collision attributes to null
		dest->overlap = 0.0f;
		dest->obj1 = NULL;
		dest->obj2 = NULL;
		dest->obj1Frame = NULL;
		dest->obj2Frame = NULL;

		return;
	}

	//Get overlap of left face of obj1 with right face of obj2
	float overlapLeft = (obj2Frame->position->components[0] + AABB2->centroid->components[0] + (scaledAABB2.width / 2.0f)) - 
		((obj1Frame->position->components[0] + AABB1->centroid->components[0]) - (scaledAABB1.width / 2.0f));
	if(overlapLeft < 0.0f)
	{
		//No collision, set the collision attributes to null
		dest->overlap = 0.0f;
		dest->obj1 = NULL;
		dest->obj2 = NULL;
		dest->obj1Frame = NULL;
		dest->obj2Frame = NULL;

		return;
	}

	//Y Axis Tests
	//Get overlap of top face of obj1 with bottom face of obj2
	float overlapTop = (obj1Frame->position->components[1] + AABB1->centroid->components[1] + (scaledAABB1.height / 2.0f)) - 
		((obj2Frame->position->components[1] + AABB2->centroid->components[1]) - (scaledAABB2.height / 2.0f));
	if(overlapTop < 0.0f)
	{
		//No collision, set the collision attributes to null
		dest->overlap = 0.0f;
		dest->obj1 = NULL;
		dest->obj2 = NULL;
		dest->obj1Frame = NULL;
		dest->obj2Frame = NULL;

		return;
	}

	//Get overlap of bottom face of obj1 with top face of obj2
	float overlapBottom = (obj2Frame->position->components[1] + AABB2->centroid->components[1] + (scaledAABB2.height / 2.0f)) - 
		((obj1Frame->position->components[1] + AABB1->centroid->components[1]) - (scaledAABB1.height / 2.0f));
	if(overlapBottom < 0.0f)
	{
		//No collision, set the collision attributes to null
		dest->overlap = 0.0f;
		dest->obj1 = NULL;
		dest->obj2 = NULL;
		dest->obj1Frame = NULL;
		dest->obj2Frame = NULL;

		return;
	}

	//Get overlap of front face of obj1 with back face of obj2
	float overlapFront = (obj1Frame->position->components[2] + AABB1->centroid->components[2] + (scaledAABB1.depth / 2.0f)) - 
		((obj2Frame->position->components[2] + AABB2->centroid->components[2]) - (scaledAABB2.depth / 2.0f));
	if(overlapFront < 0.0f)
	{
		//No collision, set the collision attributes to null
		dest->overlap = 0.0f;
		dest->obj1 = NULL;
		dest->obj2 = NULL;
		dest->obj1Frame = NULL;
		dest->obj2Frame = NULL;

		return;
	}

	//Get overlap of back face of obj1 with front face of obj2
	float overlapBack = (obj2Frame->position->components[2] + AABB2->centroid->components[2] + (scaledAABB2.depth / 2.0f)) - ((obj1Frame->position->components[2] + AABB1->centroid->components[2]) - (scaledAABB1.depth / 2.0f));
	if(overlapBack < 0.0f)
	{
		//No collision, set the collision attributes to null
		dest->overlap = 0.0f;
		dest->obj1 = NULL;
		dest->obj2 = NULL;
		dest->obj1Frame = NULL;
		dest->obj2Frame = NULL;

		return;
	}


	//If code reaches this point, there is a collision. 
	//The minimum translation vector ends up being a face normal of obj1
	//By convention the MTV must ALWAYS point towards collision->obj1. Therefore,
	//We must assign obj2 to collision->obj1, and obj1 to collision->obj2. This way, the face normal of collision->obj2
	//(And therefore the MTV) will be pointing towards obj1.
	dest->obj1 = obj2;
	dest->obj1Frame = obj2Frame;

	dest->obj2 = obj1;
	dest->obj2Frame = obj1Frame;

	//We must determine the minimum translation vector.
	//The MTV will be the axis with the minimum overlap
	//Righ face
	dest->overlap = overlapRight;
	Vector_Copy(dest->minimumTranslationVector, &Vector_E1);

	//Left Face
	if(overlapLeft < dest->overlap)
	{
		dest->overlap = overlapLeft;
		Vector_Copy(dest->minimumTranslationVector, &Vector_E1);
		Vector_Scale(dest->minimumTranslationVector, -1.0f);
	}

	//Top Face
	if(overlapTop < dest->overlap)
	{
		dest->overlap = overlapTop;
		Vector_Copy(dest->minimumTranslationVector, &Vector_E2);
	}

	//Bottom face
	if(overlapBottom < dest->overlap)
	{
		dest->overlap = overlapBottom;
		Vector_Copy(dest->minimumTranslationVector, &Vector_E2);
		Vector_Scale(dest->minimumTranslationVector, -1.0f);
	}

	//Front face
	if(overlapFront < dest->overlap)
	{
		dest->overlap = overlapFront;
		Vector_Copy(dest->minimumTranslationVector, &Vector_E3);
	}

	//Back face
	if(overlapBack < dest->overlap)
	{
		dest->overlap = overlapBack;
		Vector_Copy(dest->minimumTranslationVector, &Vector_E3);
		Vector_Scale(dest->minimumTranslationVector, -1.0f);
	}
}

///
//Tests if an object with an axis aligned bounding box is colliding with an object with a convex hull collider
//
//Parameters:
//	dest: Collision to store the results of test in
//	AABBObj: Pointer to the object which has an axis aligned bounding box
//	AABBObjFrame: Pointer to the frame of reference used to orient the object with the AABB
//	convexObj: Pointer to the object which has a convex hull collider
//	convexFrame: Pointer to the frame of reference used to orient the the object with the convex hull collider
void CollisionManager_TestAABBConvexCollision(struct Collision* dest, GObject* AABBObj, FrameOfReference* AABBObjFrame, GObject* convexObj, FrameOfReference* convexObjFrame)
{
	//GEt the collider data of both objects
	struct ColliderData_AABB* AABB = AABBObj->collider->data->AABBData;
	struct ColliderData_ConvexHull* convexHull = convexObj->collider->data->convexHullData;

	//Get the scaled dimensions of the AABB
	struct ColliderData_AABB scaledAABB;
	AABBCollider_GetScaledDimensions(&scaledAABB, AABB, AABBObjFrame);

	//We must convert the AABB to a convex hull and get the oriented axis and oriented points from both objects
	Vector** orientedPointsAABB = (Vector**)malloc(sizeof(Vector*) * 8);
	Vector** orientedPointsConvex = (Vector**)malloc(sizeof(Vector*) * convexHull->points->size);

	Vector** orientedAxesAABB = (Vector**)malloc(sizeof(Vector*) * 3);
	Vector** orientedAxesConvex = (Vector**)malloc(sizeof(Vector*) * convexHull->axes->size);

	Vector** orientedEdgesAABB = (Vector**)malloc(sizeof(Vector*) * 3);
	Vector** orientedEdgesConvex = (Vector**)malloc(sizeof(Vector*) * convexHull->edges->size);


	//Allocate an initialize individual vectors in array of oriented points for AABB
	for(int i = 0; i < 8; i++)
	{
		orientedPointsAABB[i] = Vector_Allocate();
		Vector_Initialize(orientedPointsAABB[i], 3);
	}

	//Allocate and initialize individual vectors in array of oriented points for convex hull
	for(unsigned int i = 0; i < convexHull->points->size; i++)
	{
		orientedPointsConvex[i] = Vector_Allocate();
		Vector_Initialize(orientedPointsConvex[i], 3);
	}

	//Allocate and initialize individual vectors in array of oriented axes & edges for AABB
	for(int i = 0; i < 3; i++)
	{
		orientedAxesAABB[i] = Vector_Allocate();
		Vector_Initialize(orientedAxesAABB[i], 3);

		orientedEdgesAABB[i] = Vector_Allocate();
		Vector_Initialize(orientedEdgesAABB[i], 3);
	}

	//Allocate and initialize individual vectors in array of oriented axes for convex hull
	for(unsigned int i = 0; i < convexHull->axes->size; i++)
	{
		orientedAxesConvex[i] = Vector_Allocate();
		Vector_Initialize(orientedAxesConvex[i], 3);
	}

	//Allocate and initialize individual vectors in array of oriented edges for convex hull
	for(unsigned int i = 0; i < convexHull->axes->size; i++)
	{
		orientedEdgesConvex[i] = Vector_Allocate();
		Vector_Initialize(orientedEdgesConvex[i], 3);
	}

	//Get oriented points of AABB
	//Right Bottom Front
	orientedPointsAABB[0]->components[0] = scaledAABB.width / 2.0f;
	orientedPointsAABB[0]->components[1] = scaledAABB.height / -2.0f;
	orientedPointsAABB[0]->components[2] = scaledAABB.depth / 2.0f;
	//Translate by centroid of AABB
	Vector_Increment(orientedPointsAABB[0], AABB->centroid);
	//Translate point to position of obj
	Vector_Increment(orientedPointsAABB[0], AABBObjFrame->position);

	//Right Bottom Back
	orientedPointsAABB[1]->components[0] = scaledAABB.width / 2.0f;
	orientedPointsAABB[1]->components[1] = scaledAABB.height / -2.0f;
	orientedPointsAABB[1]->components[2] = scaledAABB.depth / -2.0f;
	//Translate by centroid of AABB
	Vector_Increment(orientedPointsAABB[1], AABB->centroid);
	//Translate point to position of obj
	Vector_Increment(orientedPointsAABB[1], AABBObjFrame->position);

	//Left Bottom Back
	orientedPointsAABB[2]->components[0] = scaledAABB.width / -2.0f;
	orientedPointsAABB[2]->components[1] = scaledAABB.height / -2.0f;
	orientedPointsAABB[2]->components[2] = scaledAABB.depth / -2.0f;
	//Translate by centroid of AABB
	Vector_Increment(orientedPointsAABB[2], AABB->centroid);
	//Translate point to position of obj
	Vector_Increment(orientedPointsAABB[2], AABBObjFrame->position);

	//Left Bottom Front
	orientedPointsAABB[3]->components[0] = scaledAABB.width / -2.0f;
	orientedPointsAABB[3]->components[1] = scaledAABB.height / -2.0f;
	orientedPointsAABB[3]->components[2] = scaledAABB.depth / 2.0f;
	//Translate by centroid of AABB
	Vector_Increment(orientedPointsAABB[3], AABB->centroid);
	//Translate point to position of obj
	Vector_Increment(orientedPointsAABB[3], AABBObjFrame->position);

	//Right Top Front
	orientedPointsAABB[4]->components[0] = scaledAABB.width / 2.0f;
	orientedPointsAABB[4]->components[1] = scaledAABB.height / 2.0f;
	orientedPointsAABB[4]->components[2] = scaledAABB.depth / 2.0f;
	//Translate by centroid of AABB
	Vector_Increment(orientedPointsAABB[4], AABB->centroid);
	//Translate point to position of obj
	Vector_Increment(orientedPointsAABB[4], AABBObjFrame->position);

	//Right Top Back
	orientedPointsAABB[5]->components[0] = scaledAABB.width / 2.0f;
	orientedPointsAABB[5]->components[1] = scaledAABB.height / 2.0f;
	orientedPointsAABB[5]->components[2] = scaledAABB.depth / -2.0f;
	//Translate by centroid of AABB
	Vector_Increment(orientedPointsAABB[5], AABB->centroid);
	//Translate point to position of obj
	Vector_Increment(orientedPointsAABB[5], AABBObjFrame->position);

	//Left Top Back
	orientedPointsAABB[6]->components[0] = scaledAABB.width / -2.0f;
	orientedPointsAABB[6]->components[1] = scaledAABB.height / 2.0f;
	orientedPointsAABB[6]->components[2] = scaledAABB.depth / -2.0f;
	//Translate by centroid of AABB
	Vector_Increment(orientedPointsAABB[6], AABB->centroid);
	//Translate point to position of obj
	Vector_Increment(orientedPointsAABB[6], AABBObjFrame->position);

	//Left Top Front
	orientedPointsAABB[7]->components[0] = scaledAABB.width / -2.0f;
	orientedPointsAABB[7]->components[1] = scaledAABB.height / 2.0f;
	orientedPointsAABB[7]->components[2] = scaledAABB.depth / 2.0f;
	//Translate by centroid of AABB
	Vector_Increment(orientedPointsAABB[7], AABB->centroid);
	//Translate point to position of obj
	Vector_Increment(orientedPointsAABB[7], AABBObjFrame->position);

	//Get oriented points of Convex Hull
	ConvexHullCollider_GetOrientedWorldPoints(orientedPointsConvex, convexHull, convexObjFrame);

	//Get oriented axes of AABB
	Vector_Copy(orientedAxesAABB[0], &Vector_E1);
	Vector_Copy(orientedAxesAABB[1], &Vector_E2);
	Vector_Copy(orientedAxesAABB[2], &Vector_E3);

	//Get oriented axes of convex hull
	ConvexHullCollider_GetOrientedAxes(orientedAxesConvex, convexHull, convexObjFrame);

	//Get oriented edges of AABB
	Vector_Copy(orientedEdgesAABB[0], &Vector_E1);
	Vector_Copy(orientedEdgesAABB[1], &Vector_E2);
	Vector_Copy(orientedEdgesAABB[2], &Vector_E3);

	//Get oriented edges of convex hull
	ConvexHullCollider_GetOrientedEdges(orientedEdgesConvex, convexHull, convexObjFrame);


	//Perform SAT Alorithm for face normals
	unsigned char detected = CollisionManager_PerformSATFaces(dest,
		(const Vector**)orientedAxesAABB, 3, (const Vector**)orientedPointsAABB, 8,
		(const Vector**)orientedAxesConvex, convexHull->axes->size, (const Vector**)orientedPointsConvex, convexHull->points->size);

	//If a collisionn is detected, check edge normals
	if(detected)
	{
		detected = CollisionManager_PerformSATEdges(dest,
			(const Vector**)orientedEdgesAABB, 3, (const Vector**)orientedPointsAABB, 8,
			(const Vector**)orientedEdgesConvex, convexHull->edges->size, (const Vector**)orientedPointsConvex, convexHull->points->size);
	}

	//Delete oriented arrays
	for(int i = 0; i < 8; i++)
	{
		Vector_Free(orientedPointsAABB[i]);
	}
	free(orientedPointsAABB);
	for(unsigned int i = 0; i < convexHull->points->size; i++)
	{
		Vector_Free(orientedPointsConvex[i]);
	}
	free(orientedPointsConvex);
	for(int i = 0; i < 3; i++)
	{
		Vector_Free(orientedAxesAABB[i]);
	}
	free(orientedAxesAABB);
	for(unsigned int i = 0; i < convexHull->axes->size; i++)
	{
		Vector_Free(orientedAxesConvex[i]);
	}
	free(orientedAxesConvex);
	for(int i = 0; i < 3; i++)
	{
		Vector_Free(orientedEdgesAABB[i]);
	}
	free(orientedEdgesAABB);
	for(unsigned int i = 0; i < convexHull->edges->size; i++)
	{
		Vector_Free(orientedEdgesConvex[i]);
	}
	free(orientedEdgesConvex);


	if(detected)
	{
		//MTV must always face obj1
		Vector bToa;
		Vector_INIT_ON_STACK(bToa, 3);

		Vector_Subtract(&bToa, AABBObjFrame->position, convexObjFrame->position);

		//Check if MTV must be reversed to face Obj 1
		if(Vector_DotProduct(&bToa, dest->minimumTranslationVector) < 0.0f)
		{
			Vector_Scale(dest->minimumTranslationVector, -1.0f);
		}

		dest->obj1 = AABBObj;
		dest->obj1Frame = AABBObjFrame;
		dest->obj2 = convexObj;
		dest->obj2Frame = convexObjFrame;
	}
	else
	{
		//If there is no collision set collision atributes to null
		dest->overlap = 0.0f;
		dest->obj1 = NULL;
		dest->obj2 = NULL;
		dest->obj1Frame = NULL;
		dest->obj2Frame = NULL;

	}
}

///
//Tests if two game object's convex hulls are colliding
//If there is no collision, collision.obj1 and obj2 will be set to null upon
//The end of this function
//
//Parameters:
//	dest: Collision to store the results of test in
//	obj1:		First game object to test (Must have collider attached)
//	obj1FoR:	Pointer to frame of reference to use to orient Object 1 convex hull
//	obj2:		Second game object to test (Must have collider attached)
//	obj2FoR:	Pointer to frame of reference to use to orient Object 2 convex hull
void CollisionManager_TestConvexCollision(struct Collision* dest, GObject* obj1, FrameOfReference* obj1FoR, GObject* obj2, FrameOfReference* obj2FoR)
{
	//float minOverlap;
	unsigned char detected = 1;

	//Get the convex hull collider data
	struct ColliderData_ConvexHull* convexHull1 = obj1->collider->data->convexHullData;
	struct ColliderData_ConvexHull* convexHull2 = obj2->collider->data->convexHullData;

	//Create array of pointers to vectors to hold the oriented points of the colliders of objects in collision
	Vector** orientedPoints1 = (Vector**)malloc(sizeof(Vector*) * convexHull1->points->size);
	Vector** orientedPoints2 = (Vector**)malloc(sizeof(Vector*) * convexHull2->points->size);

	//Create array of pointers to vectors to hold the oriented axes of the colliders of objects in collision
	Vector** orientedAxes1 = (Vector**)malloc(sizeof(Vector*) * convexHull1->axes->size);
	Vector** orientedAxes2 = (Vector**)malloc(sizeof(Vector*) * convexHull2->axes->size);

	//Create array of pointers to hold oriented edges of colliders of objects in collision
	Vector** orientedEdges1 = (Vector**)malloc(sizeof(Vector*) * convexHull1->edges->size);
	Vector** orientedEdges2 = (Vector**)malloc(sizeof(Vector*) * convexHull2->edges->size);

	//Allocate & initialize individual vectors in array of point vectors for obj1
	for(unsigned int i = 0; i < convexHull1->points->size; i++)
	{
		orientedPoints1[i] = Vector_Allocate();
		Vector_Initialize(orientedPoints1[i], 3);
	}

	//Allocate & initialize individual vectors in array in array of point vectors for obj2
	for(unsigned int i = 0; i < convexHull2->points->size; i++)
	{
		orientedPoints2[i] = Vector_Allocate();
		Vector_Initialize(orientedPoints2[i], 3);
	}

	//Allocate & initialize individual vectors in array of axes vectors for obj1
	for(unsigned int i = 0; i < convexHull1->axes->size; i++)
	{
		orientedAxes1[i] = Vector_Allocate();
		Vector_Initialize(orientedAxes1[i], 3);
	}

	//Allocate & initialize individual vectors in array in array of axes vectors for obj2
	for(unsigned int i = 0; i < convexHull2->axes->size; i++)
	{
		orientedAxes2[i] = Vector_Allocate();
		Vector_Initialize(orientedAxes2[i], 3);
	}

	//Allocate & initialize individual vectors in array of edges vectors for obj1
	for(unsigned int i = 0; i < convexHull1->edges->size; i++)
	{
		orientedEdges1[i] = Vector_Allocate();
		Vector_Initialize(orientedEdges1[i], 3);
	}

	//Allocate & initialize individual vectors in array in array of axes vectors for obj2
	for(unsigned int i = 0; i < convexHull2->edges->size; i++)
	{
		orientedEdges2[i] = Vector_Allocate();
		Vector_Initialize(orientedEdges2[i], 3);
	}

	//Get oriented points of objects
	ConvexHullCollider_GetOrientedWorldPoints(orientedPoints1, convexHull1, obj1FoR);
	ConvexHullCollider_GetOrientedWorldPoints(orientedPoints2, convexHull2, obj2FoR);

	//Get oriented axes of objects
	ConvexHullCollider_GetOrientedAxes(orientedAxes1, convexHull1, obj1FoR);
	ConvexHullCollider_GetOrientedAxes(orientedAxes2, convexHull2, obj2FoR);

	//Get oriented edges of objects
	ConvexHullCollider_GetOrientedEdges(orientedEdges1, convexHull1, obj1FoR);
	ConvexHullCollider_GetOrientedEdges(orientedEdges2, convexHull2, obj2FoR);

	//Perform SAT Algorithm for face normals
	detected = CollisionManager_PerformSATFaces(dest, 
		(const Vector**)orientedAxes1, convexHull1->axes->size, (const Vector**)orientedPoints1, convexHull1->points->size,
		(const Vector**)orientedAxes2, convexHull2->axes->size, (const Vector**)orientedPoints2, convexHull2->points->size);

	//If there is a collision detection, test the edge normals
	if(detected)
	{
		detected = CollisionManager_PerformSATEdges(dest, 
			(const Vector**)orientedEdges1, convexHull1->edges->size, (const Vector**)orientedPoints1, convexHull1->points->size,
			(const Vector**)orientedEdges2, convexHull2->edges->size, (const Vector**)orientedPoints2, convexHull2->points->size);
	}



	//Delete oriented arrays
	for(unsigned int i = 0; i < convexHull1->points->size; i++)
	{
		Vector_Free(orientedPoints1[i]);
	}
	free(orientedPoints1);
	for(unsigned int i = 0; i < convexHull2->points->size; i++)
	{
		Vector_Free(orientedPoints2[i]);
	}
	free(orientedPoints2);
	for(unsigned int i = 0; i < convexHull1->axes->size; i++)
	{
		Vector_Free(orientedAxes1[i]);
	}
	free(orientedAxes1);
	for(unsigned int i = 0; i < convexHull2->axes->size; i++)
	{
		Vector_Free(orientedAxes2[i]);
	}
	free(orientedAxes2);
	for(unsigned int i = 0; i < convexHull1->edges->size; i++)
	{
		Vector_Free(orientedEdges1[i]);
	}
	free(orientedEdges1);
	for(unsigned int i = 0; i < convexHull2->edges->size; i++)
	{
		Vector_Free(orientedEdges2[i]);
	}
	free(orientedEdges2);

	if(detected)
	{
		//MTV must always face obj1
		Vector bToa;
		Vector_INIT_ON_STACK(bToa, 3);

		Vector_Subtract(&bToa, obj1FoR->position, obj2FoR->position);

		//Check if MTV must be reversed to face Obj 1
		if(Vector_DotProduct(&bToa, dest->minimumTranslationVector) < 0.0f)
		{
			Vector_Scale(dest->minimumTranslationVector, -1.0f);
		}

		dest->obj1 = obj1;
		dest->obj1Frame = obj1FoR;
		dest->obj2 = obj2;
		dest->obj2Frame = obj2FoR;
	}
	else
	{
		//If there is no collision set collision atributes to null
		dest->overlap = 0.0f;
		dest->obj1 = NULL;
		dest->obj2 = NULL;
		dest->obj1Frame = NULL;
		dest->obj2Frame = NULL;

	}
}

///
//Tests if a a game object's convex hull is colliding with a sphere
//If there is no collision, collision.obj1 and obj2 will be set to null upon
//The end of this function
//
//Parameters:
//	dest: Collision to store the results of test in
//	convexObj: The gameObject with the convex hull to test
//	convexFoR: the frame of reference to use to orient the convex hull collider
//	sphere: The gameobject with the sphere collider to test
//	sphereFoR: The frame of reference to use to orient the sphere collider
void CollisionManager_TestConvexSphereCollision(struct Collision* dest, GObject* convexObj, FrameOfReference* convexFoR, GObject* sphereObj, FrameOfReference* sphereFoR)
{
	float minOverlap = 0.0f;
	unsigned char detected = 1;

	//Get the convex hull data
	struct ColliderData_ConvexHull* convexHull = convexObj->collider->data->convexHullData;
	//Get the sphere data
	struct ColliderData_Sphere* sphere = sphereObj->collider->data->sphereData;

	//Create array of pointers to vectors to hold the oriented points of the collider of convex obj in collision
	Vector** orientedPoints = (Vector**)malloc(sizeof(Vector*) * convexHull->points->size);
	//Create array of pointers to vectors to hold the oriented axes of the collider of convex object in collision
	Vector** orientedAxes = (Vector**)malloc(sizeof(Vector*) * convexHull->axes->size);

	//Allocate & initialize individual vectors in array of point vectors for convex obj
	for(unsigned int i = 0; i < convexHull->points->size; i++)
	{
		orientedPoints[i] = Vector_Allocate();
		Vector_Initialize(orientedPoints[i], 3);
	}

	//Allocate & initialize individual vectors in array of axes vectors for convex obj
	for(unsigned int i = 0; i < convexHull->axes->size; i++)
	{
		orientedAxes[i] = Vector_Allocate();
		Vector_Initialize(orientedAxes[i], 3);
	}

	ConvexHullCollider_GetOrientedAxes(orientedAxes, convexHull, convexFoR);
	ConvexHullCollider_GetOrientedWorldPoints(orientedPoints, convexHull, convexFoR);

	struct ProjectionBounds bounds[2];

	Vector** sphereSurfacePoints = (Vector**)malloc(sizeof(Vector*) * 2);
	for(int i = 0; i < 2; i++)
	{
		sphereSurfacePoints[i] = Vector_Allocate();
		Vector_Initialize(sphereSurfacePoints[i], 3);
	}

	Vector normalizedAxis;
	Vector_INIT_ON_STACK(normalizedAxis, 3);
	for(unsigned int i = 0; i < convexHull->axes->size; i++)
	{
		Vector_Copy(&normalizedAxis, orientedAxes[i]);
		Vector_Normalize(&normalizedAxis);

		//Scale sphere surface points to maximum scale in sphere's Frame of reference
		float scaledRad = SphereCollider_GetScaledRadius(sphere, sphereFoR);
		Vector_Scale(&normalizedAxis, scaledRad);
		Vector_Add(sphereSurfacePoints[0], sphereFoR->position, &normalizedAxis);
		Vector_Subtract(sphereSurfacePoints[1], sphereFoR->position, &normalizedAxis);


		CollisionManager_GetProjectionBounds(bounds, orientedAxes[i],(const Vector**) orientedPoints, convexHull->points->size);
		CollisionManager_GetProjectionBounds(bounds + 1, orientedAxes[i],(const Vector**) sphereSurfacePoints, 2);

		//Check for overlap
		if(bounds[0].min <= bounds[1].max && bounds[0].max >= bounds[1].min)
		{
			//IF there is overlap we must keep track of the smallest overlap and the axis it occurs on
			//If an axis is overlapping we must track which has the smallest overlap!
			float overlap1 = bounds[1].max - bounds[0].min;
			float overlap2 = bounds[0].max - bounds[1].min;

			//Determine thesmallest overlap
			float smallerOverlap = overlap1 < overlap2 ? overlap1 : overlap2;


			//if the overlap is smaller than the current smallest overlap
			if(smallerOverlap < minOverlap || i == 0)
			{
				//Assign the minimum overlap to the smallest of the two overlaps
				minOverlap = smallerOverlap;
				//Assign the MTV to the current axis
				Vector_Copy(dest->minimumTranslationVector, orientedAxes[i]);
			}
		}
		else
		{
			//If any axis is not overlapping, there is no collision
			detected = 0;
			break;
		}

	}

	if(detected)
	{
		printf("Collision\n");

		//If there is a collision assign collision attributes
		dest->overlap = minOverlap;
		dest->obj1 = convexObj;
		dest->obj1Frame = convexFoR;
		dest->obj2 = sphereObj;
		dest->obj2Frame = sphereFoR;

		//Make sure MTV is normalized
		Vector_Normalize(dest->minimumTranslationVector);

		//By convention MTV must always point towards obj1
		Vector bToa;
		Vector_INIT_ON_STACK(bToa, 3);

		//Vector_Subtract(&bToa, sphereFoR->position, convexFoR->position);
		Vector_Subtract(&bToa, convexFoR->position, sphereFoR->position);
		//Check if MTV must be reversed to face Obj 1
		if(Vector_DotProduct(&bToa, dest->minimumTranslationVector) < 0.0f)
		{
			Vector_Scale(dest->minimumTranslationVector, -1.0f);
		}
	}
	else
	{
		//If there is no collision set the collision attributes to 0.
		dest->overlap = 0.0f;
		dest->obj1 = NULL;
		dest->obj1Frame = NULL;
		dest->obj2 = NULL;
		dest->obj2Frame = NULL;
	}

	//Delete heap allocated variables
	for(unsigned int i = 0; i < convexHull->points->size; i++)
	{
		Vector_Free(orientedPoints[i]);
	}
	free(orientedPoints);

	for(unsigned int i = 0; i < convexHull->axes->size; i++)
	{
		Vector_Free(orientedAxes[i]);
	}
	free(orientedAxes);

	for(unsigned int i = 0; i < 2; i++)
	{
		Vector_Free(sphereSurfacePoints[i]);
	}
	free(sphereSurfacePoints);


}

///
//Performs the Separating Axis Theorem test
//
//Parameters:
//	dest: A pointer to a collision to store the results of the test in. Results will not be stored if the test detects no collision
//	orientedAxes1: An array of pointers to vectors representing the oriented axes of object 1 involved in the test
//	numAxes1: The number of axes belonging to object 1
//	orientedPoints1: An array of pointers to vectors representing the oriented points of object 1 involved in the test
//	numPoints1: The number of points belonging to object 1
//	orientedAxes2: An array of pointersto vectors representing the oriented axes of object 2 involved in the test
//	numAxes2: The number of axes belonging to object 2
//	orientedPoints2: An array of pointers to vectors representing the oriented points of object 2 involved in the test
//	numPoints2: The number of points belonging to object 2
//
//Returns:
//	0 if the test detects no collision
//	1 if the test detects a collision
static unsigned char CollisionManager_PerformSATFaces(struct Collision* dest, 
	const Vector** orientedAxes1, unsigned int numAxes1, 
	const Vector** orientedPoints1, const unsigned int numPoints1, 
	const Vector** orientedAxes2, const unsigned int numAxes2, 
	const Vector** orientedPoints2, const unsigned int numPoints2)
{
	unsigned char detected = 1;		//Tracks if any axis does not detect a collision
	float minOverlap = 0.0f;		//Stores the minimum overlap of all axes

	//Stores the projection bounds on each axis of obj1's points and obj2's points
	struct ProjectionBounds bounds[2];

	//Test obj1's axis
	for(unsigned int i = 0; i < numAxes1; i++)
	{
		CollisionManager_GetProjectionBounds(bounds, orientedAxes1[i], orientedPoints1, numPoints1);
		CollisionManager_GetProjectionBounds(bounds + 1, orientedAxes1[i], orientedPoints2, numPoints2);

		//Check if the min and max projections overlap on the axis
		if(bounds[0].min < bounds[1].max && bounds[0].max > bounds[1].min)
		{
			//If an axis is overlapping we must track which has the smallest overlap!
			float overlap1 = bounds[1].max - bounds[0].min;
			float overlap2 = bounds[0].max - bounds[1].min;

			//Determine the smallest overlap
			float smallerOverlap = overlap1 < overlap2 ? overlap1 : overlap2;


			//If this is the first overlap or the overlap is smaller than the current smallest overlap
			if(i == 0 || smallerOverlap < minOverlap)
			{
				//Assign the minimum overlap to the smallest of the two overlaps
				minOverlap = smallerOverlap;
				//Assign the MTV to the current axis
				Vector_Copy(dest->minimumTranslationVector, orientedAxes1[i]);
			}

		}
		else
		{
			//If any axis is not overlapping, there is no collision
			detected = 0;
			break;
		}
	}

	//If obj1's axis all detected collision, try obj2's axis
	if(detected)
	{
		for(unsigned int i = 0; i < numAxes2; i++)
		{
			CollisionManager_GetProjectionBounds(bounds, orientedAxes2[i], orientedPoints1, numPoints1);
			CollisionManager_GetProjectionBounds(bounds + 1, orientedAxes2[i], orientedPoints2, numPoints2);


			//Check for overlap
			if(bounds[0].min <= bounds[1].max && bounds[0].max >= bounds[1].min)
			{
				//IF there is overlap we must keep track of the smallest overlap and the axis it occurs on
				//If an axis is overlapping we must track which has the smallest overlap!
				float overlap1 = bounds[1].max - bounds[0].min;
				float overlap2 = bounds[0].max - bounds[1].min;

				//Determine thesmallest overlap
				float smallerOverlap = overlap1 < overlap2 ? overlap1 : overlap2;


				//if the overlap is smaller than the current smallest overlap
				if(smallerOverlap < minOverlap)
				{
					//Assign the minimum overlap to the smallest of the two overlaps
					minOverlap = smallerOverlap;
					//Assign the MTV to the current axis
					Vector_Copy(dest->minimumTranslationVector, orientedAxes2[i]);
				}
			}
			else
			{
				//If any axis is not overlapping, there is no collision
				detected = 0;
				break;
			}
		}
	}

	//If a collision was detected, store the minimum overlap in the collision destination
	if(detected)
	{

		dest->overlap = minOverlap;
	}

	return detected;
}

///
//Performs the Separating axis theorem test with face normals
//
//Parameters:
//	dest: A pointer to the collision to store the results of the test in. Resultswill not be stored if the test detects no collisions
//	orientedEdges1: An array of pointers to vectors representing the oriented edges of object 1
//	numEdges1: The number of edges belonging to object 1
//	orientedPoints1: An array of pointers to vectors representing the oriented points of object 1
//	numPoints1: The number of points belonging to object 1
//	orientedEdges2: An array of pointers to vectors representing the oriented edges of object 2
//	numEdges2: The number of edges belonging to object 2
//  orientedPoints2: An array of pointers to vectors representing the oriented points of object 2
//	numPoints2: the number of points belonging to object 2
static unsigned char CollisionManager_PerformSATEdges(struct Collision* dest,
	const Vector** orientedEdges1, const unsigned int numEdges1,
	const Vector** orientedPoints1, const unsigned int numPoints1,
	const Vector** orientedEdges2, const unsigned int numEdges2,
	const Vector** orientedPoints2, const unsigned int numPoints2)
{
	float minOverlap = dest->overlap;	//Stores the minimum overlap of all axes

	//Stores the projection bounds on each axis of obj1's points and obj2's points
	struct ProjectionBounds bounds[2];

	//The normal from the cross product of each edge
	Vector normal;
	Vector_INIT_ON_STACK(normal, 3);

	//For each one of OBJ1's edges
	for(unsigned int i = 0; i < numEdges1; i++)
	{
		//And each of OBJ2's edges
		for(unsigned int j = 0; j < numEdges2; j++)
		{

			//Calculate the normal from the cross product of the two edges
			Vector_CrossProduct(&normal, orientedEdges1[i], orientedEdges2[j]);

			//Normalize it!
			Vector_Normalize(&normal);

			CollisionManager_GetProjectionBounds(bounds, &normal, orientedPoints1, numPoints1);
			CollisionManager_GetProjectionBounds(bounds + 1, &normal, orientedPoints2, numPoints2);

			//If the normal vector is the zero vector, skip this axis test
			if(Vector_GetMag(&normal) == 0)
				continue;

			//Check if the min and max projections overlap on the axis
			if(bounds[0].min <= bounds[1].max && bounds[0].max >= bounds[1].min)
			{
				//If an axis is overlapping we must track which has the smallest overlap!
				float overlap1 = bounds[1].max - bounds[0].min;
				float overlap2 = bounds[0].max - bounds[1].min;

				//Determine the smallest overlap
				float smallerOverlap = overlap1 < overlap2 ? overlap1 : overlap2;


				//If this is the first overlap or the overlap is smaller than the current smallest overlap
				if(smallerOverlap < minOverlap)
				{
					//Assign the minimum overlap to the smallest of the two overlaps
					minOverlap = smallerOverlap;
					//Assign the MTV to the current axis
					Vector_Copy(dest->minimumTranslationVector, &normal);
				}

			}
			else
			{
				//If any axis is not overlapping, there is no collision
				return 0;
			}

		}
	}

	//If this point is reached there must be a collision
	dest->overlap = minOverlap;
	return 1;
}

///
//Projects a set of points onto a normalized axis getting the squared magnitude of the projection vector.
//Finds the minimum and maximum values of the point set projected onto the axis.
//
//Parameters:
//	dest: The destination of the projection bounds
//	axis: The axis projecting onto
//	points: The set of points to project onto the axis
static void CollisionManager_GetProjectionBounds(struct ProjectionBounds* dest, const Vector* axis, const Vector** points, const int numPoints)
{
	float result = Vector_DotProduct(axis, points[0]);

	dest->min = result;
	dest->max = result;

	for(int i = 0; i < numPoints; i++)
	{
		result = Vector_DotProduct(axis, points[i]);
		if(result < dest->min) dest->min = result;
		else if(result > dest->max) dest->max = result;
	}
}


///
//Allocates memory for a new collision buffer
//
//Returns:
//	pointer to a newly allocated collision buffer
static CollisionBuffer* CollisionManager_AllocateBuffer(void)
{
	CollisionBuffer* buffer = (CollisionBuffer*)malloc(sizeof(CollisionBuffer));
	return buffer;
}

///
//Initializes a collision buffer
//
//Parameters:
//	buffer: The buffer to initialize
static void CollisionManager_InitializeBuffer(CollisionBuffer* buffer)
{
	buffer->collisions = LinkedList_Allocate();
	LinkedList_Initialize(buffer->collisions);
}


///
//Frees resources allocated by a collision buffer
//
//Parameters:
//	buffer: A pointer to The collision buffer to free
static void CollisionManager_FreeBuffer(CollisionBuffer* buffer)
{
	//Free all individual collisions in the list of collisions
	struct LinkedList_Node* currentNode = collisionBuffer->collisions->head;
	struct LinkedList_Node* nextNode = NULL;
	while(currentNode != NULL)
	{
		nextNode = currentNode->next;
		CollisionManager_FreeCollision((struct Collision*)currentNode->data);
		currentNode = nextNode;
	}

	LinkedList_Free(buffer->collisions);
	free(buffer);
}

///
//Allocates memory for a new collision
//
//Returns:
//	Pointer to a newly allocated collision
static struct Collision* CollisionManager_AllocateCollision(void)
{
	struct Collision* collision = (struct Collision*)malloc(sizeof(struct Collision));
	return collision;
}

///
//Initializes a collision
//
//PArameters:
//	collision: The collision to initialize
static void CollisionManager_InitializeCollision(struct Collision* collision)
{
	collision->obj1 = NULL;
	collision->obj1Frame = NULL;
	collision->obj2 = NULL;
	collision->obj2Frame = NULL;

	collision->minimumTranslationVector = Vector_Allocate();
	Vector_Initialize(collision->minimumTranslationVector, 3);
}

///
//Frees the memory allocated for a collision.
//Does not free the objects involved in the collision!
//
//Parameters:
//	collision: The collision being freed
void CollisionManager_FreeCollision(struct Collision* collision)
{
	Vector_Free(collision->minimumTranslationVector);
	free(collision);
}
