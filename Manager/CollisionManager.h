#ifndef COLLISIONMANAGER_H
#define COLLISIONMANAGER_H

#include "../GObject/GObject.h"
#include "../Data/LinkedList.h"

#include "../Data/OctTree.h"

struct Collision
{
	GObject* obj1;
	FrameOfReference* obj1Frame;
	GObject* obj2;
	FrameOfReference* obj2Frame;
	Vector* minimumTranslationVector;	//Normalized vector describing the collision / contact normal
	float overlap;						//The magnitude of the overlap on the minimum translation axis
};

typedef struct CollisionBuffer
{
	LinkedList* collisions;		//Contains the list of registered collisions for each frame
} CollisionBuffer;

///
//Internals
//Used to track the minimum and maximum bounds of the projection of a collider onto a specific axis
struct ProjectionBounds
{
	float min, max;
};

static CollisionBuffer* collisionBuffer;

///
//Allocates memory for a new collision buffer
//
//Returns:
//	pointer to a newly allocated collision buffer
static CollisionBuffer* CollisionManager_AllocateBuffer(void);

///
//Initializes a collision buffer
//
//Parameters:
//	buffer: The buffer to initialize
static void CollisionManager_InitializeBuffer(CollisionBuffer* buffer);

///
//Frees resources allocated by a collision buffer
//
//Parameters:
//	buffer: A pointer to The collision buffer to free
static void CollisionManager_FreeBuffer(CollisionBuffer* buffer);

///
//Allocates memory for a new collision
//
//Returns:
//	Pointer to a newly allocated collision
static struct Collision* CollisionManager_AllocateCollision(void);

///
//Initializes a collision
//All members of the collision are set to NULL with the exception of the MTV
//
//PArameters:
//	collision: The collision to initialize
static void CollisionManager_InitializeCollision(struct Collision* collision);

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
//Tests for collisions on all objects within an oct tree node appending to a list of collisions which occur
//
//Parameters:
//	node: A pointer to the node of the oct tree to check
static void CollisionManager_UpdateOctTreeNode(struct OctTree_Node* node);

///
//Tests for collisions on an array of objects within an oct tree node appending to a list of collisions which occur
//
//Parameters:
//	gameObjects: An array of pointers to game objects to check collisions
//	numObjects: The number of objects in the array
static void CollisionManager_UpdateOctTreeNodeArray(GObject** gameObjects, unsigned int numObjects);

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
//Performs the Separating Axis Theorem test with face normals
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
										const Vector** orientedAxes1, const unsigned int numAxes1, 
										const Vector** orientedPoints1, const unsigned int numPoints1, 
										const Vector** orientedAxes2, const unsigned int numAxes2, 
										const Vector** orientedPoints2, const unsigned int numPoints2);

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
										const Vector** orientedPoints2, const unsigned int numPoints2);

///
//Projects a set of points onto a normalized axis getting the squared magnitude of the projection vector.
//Finds the minimum and maximum values of the point set projected onto the axis.
//
//Parameters:
//	dest: The destination of the projection bounds
//	axis: The axis projecting onto
//	points: The set of points to project onto the axis
static void CollisionManager_GetProjectionBounds(struct ProjectionBounds* dest, const Vector* axis, const Vector** points, const int numPoints);

#endif
