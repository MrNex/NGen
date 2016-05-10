#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include "../GObject/GObject.h"
#include "../Data/LinkedList.h"

#include "../Data/OctTree.h"
#include "../Data/MemoryPool.h"

struct Collision
{
	GObject* obj1;
	FrameOfReference* obj1Frame;
	GObject* obj2;
	FrameOfReference* obj2Frame;
	Vector* minimumTranslationVector;	//Normalized vector describing the collision / contact normal
	float overlap;				//The magnitude of the overlap on the minimum translation axis
	float resolutionImpulse;		//The magnitude of the impulse which resolved the collision
};

typedef struct CollisionBuffer
{
	MemoryPool* sphereData;
	MemoryPool* worldSphereData;
	MemoryPool* sphereTransformations;
	MemoryPool* aabbData;
	MemoryPool* worldAABBData;
	LinkedList* collisions;		//Contains the list of registered collisions for each frame
} CollisionBuffer;

///
//Internals
//Used to track the minimum and maximum bounds of the projection of a collider onto a specific axis
struct ProjectionBounds
{
	float min, max;
};

extern CollisionBuffer* collisionBuffer;

///
//Frees the memory allocated for a collision.
//Does not free the objects involved in the collision!
//
//Parameters:
//	collision: The collision being freed
void CollisionManager_FreeCollision(struct Collision* collision);

///
//Initializes the Collision Manager
void CollisionManager_Initialize(void);

///
//Frees any resources allocated by the collision manager
void CollisionManager_Free(void);

///
//Gets a pointer to the memory pool which contains a certain type of collider
//
//Parameters:
//	type: 		The type of the collider
//	worldData: 	0 if the collider is desired in object space
//			1 if the collider should be oriented in world space
//
//Returns:
//	A pointer to the memory pool containing the desired data
MemoryPool* CollisionManager_RetrieveMemoryPool(ColliderType type, unsigned char worldData);

///
//Tests for collisions on all objects which have colliders 
//compiling a list of game objects which test true
//
//Parameters:
//	gameObjects: THe list of gameObjects to test
//
//Returns:
//	A pointer to a linked list of collisions which occurred this frame.
LinkedList* CollisionManager_UpdateList(LinkedList* gameObjects);

///
//Tests for collisions on all objects which have colliders
//compiling a list of collisions which occur
//
//Parameters:
//	gameObjects: An array of game objects to test
//
//Returns:
//	A pointer to a linked list of collisions which occurred this frame
LinkedList* CollisionManager_UpdateArray(GObject** gameObjects, unsigned int numObjects);

///
//Tests for collisions on all objects in an oct tree compiling a list of collisions which occur
//
//Parameters:
//	tree: The oct tree holding the game objects to test
//
//Returns: A pointer to a linked list of collisions which occurred this frame
LinkedList* CollisionManager_UpdateOctTree(OctTree* tree);

///
//Tests for a collision between two objects which have colliders
//
//Parameters:
//	dest: Collision to store the results of test in
//	obj1: First game object to test (Must have collider attached)
//	obj1FoR: Pointer to frame of reference to use to orient Object 1
//	obj2: Second game object to test (Must have collider attached)
//	obj2FoR: Pointer to frame of reference to use to orient Object 2
void CollisionManager_TestCollision(struct Collision* dest, GObject* obj1, FrameOfReference* obj1FoR, GObject* obj2, FrameOfReference* obj2FoR);

///
//TEsts if two game objects bounding spheres are colliding.
//
//Parameters:
//	dest: Collision to store the results of test in
//	obj1: First object to test (with collider)
//	obj1FoR: The frame of reference to apply to obj1's scaleFactors for the collision test
//	obj2: Object to test against (with collider)
//	obj2FoR:  The frame of reference to apply obj2's scalefactors for the collision test
//
//Returns:
//	0 vector if no collision was detected
//	Minimal translation vector if collision was detected
void CollisionManager_TestSphereCollision(struct Collision* dest, GObject* obj1, FrameOfReference* obj1FoR, GObject* obj2, FrameOfReference* obj2FoR);

///
//Tests for collision between an object with a sphere collider and a ray
//
//Parameters:
//	dest: Collision to store the results of the test in
//	ray: A pointer to the ray to test for collision with
//	sphereObj: A pointer to an object with a sphere collider to test
//	sphereFoR: A pointer to the frame of reference of the sphere object
void CollisionManager_TestRaySphereCollision(struct Collision* dest, GObject* rayObj, FrameOfReference* rayFoR, GObject* sphereObj, FrameOfReference* sphereFoR); 

///
//Tests if an object with a AABB is colliding with an object with a bounding sphere
//
//Parameters:
//	dest: Collision to store the results of the test in
//	AABBObj: Pointer to the object which has an axis aligned bounding box
//	AABBFoR: Pointer to the frame of reference to use to test of the object with the AABB
//	sphereObj: Pointer to the object which has a bounding sphere
//	sphereFoR: Pointer to the frame of reference of the object with the bounding sphere
void CollisionManager_TestAABBSphereCollision(struct Collision* dest, GObject* AABBObj, FrameOfReference* AABBFoR, GObject* sphereObj, FrameOfReference* sphereFoR);

///
//Tests if two objects with axis aligned bounding boxes are colliding
//
//Parameters:
//	dest: Collision to store the results of test in
//	obj1: Pointer to the first object with an axis aligned bounding box to test.
//	obj1Frame: pointer to Frame of reference to use to orient AABB of obj1
//	obj2: Pointer to the second object with an axis aligned bounding box to test
//	obj2Frame: Pointer to Frame of Reference to use to orient AABB of obj2
void CollisionManager_TestAABBCollision(struct Collision* dest, GObject* obj1, FrameOfReference* obj1Frame, GObject* obj2, FrameOfReference* obj2Frame);

///
//Tests if an object with an axis aligned bounding box is colliding with an object with a convex hull collider
//
//Parameters:
//	dest: Collision to store the results of test in
//	AABBObj: Pointer to the object which has an axis aligned bounding box
//	AABBObjFrame: Pointer to the frame of reference used to orient the object with the AABB
//	convexObj: Pointer to the object which has a convex hull collider
//	convexFrame: Pointer to the frame of reference used to orient the the object with the convex hull collider
void CollisionManager_TestAABBConvexCollision(struct Collision* dest, GObject* AABBObj, FrameOfReference* AABBObjFrame, GObject* convexObj, FrameOfReference* convexObjFrame);

///
//Tests if an object with an axis aligned bounding box is colliding with an object with a ray collider
//
//Parameters:
//	dest: Collision to store the results of the test in
//	AABBObj: A pointer to the object which has an AABBCollider
//	AABBObjFrame: Pointer to the frame of reference used to orient the object with the AABB
//	rayObj: A pointer to the object which has a ray collider
//	rayFrame: Pointer to the frame of reference used to orient the ray
void CollisionManager_TestAABBRayCollision(struct Collision* dest, GObject* AABBObj, FrameOfReference* AABBObjFrame, GObject* rayObj, FrameOfReference* rayFrame);

///
//Tests if two game object's convex hulls are colliding
//Utilizes the Separating axis theorem
//
//Parameters:
//	dest: Collision to store the results of test in
//	obj1:		First game object to test (Must have collider attached)
//	obj1FoR:	Pointer to frame of reference to use to orient Object 1 convex hull
//	obj2:		Second game object to test (Must have collider attached)
//	obj2FoR:	Pointer to frame of reference to use to orient Object 2 convex hull
void CollisionManager_TestConvexCollision(struct Collision* dest, GObject* obj1, FrameOfReference* obj1FoR, GObject* obj2, FrameOfReference* obj2FoR);

///
//Tests if a game objects convex hull is colliding with a ray
//
//Parameters:
//	dest: pointer to Collision to store the results of the test in
//	convexObj: pointer to THe gameobject with the convex hull being tested
//	convexFrame: pointer to The frame of reference used to orient the convex hull collider
//	rayObj: pointer to The gameobject with the ray collider being tested
//	rayFrame: pointer to The frame of reference used to orient the ray
void CollisionManager_TestConvexRayCollision(struct Collision* dest, GObject* convexObj, FrameOfReference* convexFrame, GObject* rayObj, FrameOfReference* rayFrame);

///
//Tests if a a game object's convex hull is colliding with a bounding sphere
//
//Parameters:
//	dest: Collision to store the results of test in
//	convexObj: The gameObject with the convex hull to test
//	convexFoR: the frame of reference to use to orient the convex hull collider
//	sphere: The gameobject with the sphere collider to test
//	sphereFoR: The frame of reference to use to orient the sphere collider
void CollisionManager_TestConvexSphereCollision(struct Collision* dest, GObject* convexObj, FrameOfReference* convexFoR, GObject* sphere, FrameOfReference* sphereFoR);

///
//Tests if two gameobject's ray colliders are colliding
//
//Parameters:
//	dest: A pointer to a collision to store the results in
//	obj1: A pointer to the first object with a ray collider to test collision with
//	obj1Frame: A pointer to the frame of reference used to orient the first object
//	obj2: A pointer to the second object with a ray collider to test collision wit
//	obj2Frame: A pointer to the frame of referenc eused to orient the second object
void CollisionManager_TestRayCollision(struct Collision* dest, GObject* obj1, FrameOfReference* obj1Frame, GObject* obj2, FrameOfReference* obj2Frame);

///
//Performs a ray cast against a specific object
//
//Parameters:
//	worldRay: a pointer to the ray to raycast with oriented in worldspace
//	gObj: A pointer to the GObject to test with
//
//Returns:
//	0 if there is no collision between the ray and the GObject
//	1 if there is a collision between the ray and the GObject
unsigned char CollisionManager_RayCastGObject(struct ColliderData_Ray* worldRay, GObject* gObj);

///
//Performs a ray cast against a sphere collider
//
//Parameters:
//	worldRay: A pointer to the ray to raycast with oriented in worldSpace
//	sphere: A pointer to the sphere collider to raycast against
//	sphereFoR: A pointer to the frame of reference with which to orient the sphere
unsigned char CollisionManager_RayCastSphere(struct ColliderData_Ray* worldRay, struct ColliderData_Sphere* sphere, FrameOfReference* sphereFoR);

///
//Gets the parametric value of the intersection point between a ray and a sphere
//returns NaN if no intersection point exists.
//
//Parameters:
//	worldRay: a pointer to the ray to test oriented in worldspace
//	worldSphere: a pointer to the sphere to test oriented in worldspace
//
//returns:
//	a float indicating the parametric value along the ray where first the intersection point of the ray and the sphere lay
//	will return NaN if no such point exists
float CollisionManager_GetRaySphereIntersection(struct ColliderData_Ray* worldRay, struct ColliderData_Sphere* sphere, FrameOfReference* sphereFoR);

///
//Gets the parametric value of the intersection point between a ray and an AABB
//
//Parameters:
//	worldRay: A pointer to the ray to test oriented in worldspace
//	aabb: A pointer to the AABB to test oriented in worldspace
float CollisionManager_GetRayAABBIntersection(struct ColliderData_Ray* worldRay, struct ColliderData_AABB* aabb, FrameOfReference* aabbFoR);

#endif
