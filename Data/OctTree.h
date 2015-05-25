#ifndef OCTTREE_H
#define OCTTREE_H

#include "../GObject/GObject.h"		//The data the oct tree will contain
#include "DynamicArray.h"
#include "HashMap.h"

struct OctTree_Node
{
	//Pointer to the parent of this node
	struct OctTree_Node* parent;
	//Pointer to array of children of this node
	struct OctTree_Node* children;

	//The data contained in this node
	DynamicArray* data;

	//The depth of this node from the root of the tree
	//The root has a depth of 0.
	unsigned int depth;

	//Bounds of this oct tree node
	float left, right;		//Width
	float bottom, top;		//Height
	float back, front;		//Depth
};

struct OctTree_NodeStatus
{
	struct OctTree_Node* node;
	unsigned char collisionStatus;
};

typedef struct OctTree
{
	//Pointer to the root of the tree
	struct OctTree_Node* root;

	//Attributes of the tree
	unsigned int maxDepth;		//How many subdivisions can exist
	unsigned int maxOccupancy;	//How many occupants can an octtree have before trying to subdivide
								//This number will be exceeded if maxDepth is reached.

	//Hashmap used to update tree
	HashMap* map;
} OctTree;

//Internal members
static unsigned int defaultMaxOccupancy = 3;
static unsigned int defaultMaxDepth = 3;

//Internal functions

///
//Allocates memory for an octtree node
//
//Returns:
//	A pointer to a newly allocated, uninitialized oct tree node
static struct OctTree_Node* OctTree_Node_Allocate();

///
//Initializes an oct tree node to the given specifications
//
//Parameters:
//	node: A pointer to the oct tree node to initialize
//	tree: A pointer to the oct tree this node is part of
//	parent: A pointer to the parent node of this oct tree node
//	depth: The depth from the root node of this node in the oct tree
//	leftBound: The left bound of the octtree
//	rightBound: The right bound of the octtree
//	bottomBound: The bottom bound of the octtree
//	topBound: The top bound of the octtree
//	backBound: The back bound of the octtree
//	frontBound: The front bound of the octtree
static void OctTree_Node_Initialize(struct OctTree_Node* node, OctTree* tree,  struct OctTree_Node* parent, unsigned int depth, float leftBound, float rightBound, float bottomBound, float topBound, float backBound, float frontBound);

///
//Frees data allocated by an oct tree node
//
//Parameters:
//	node: A pointer to the oct tree node to free
static void OctTree_Node_Free(struct OctTree_Node* node);

///
//Allocates the children of an oct tree node
//
//Returns:
//	A pointer to an array of 8 newly allocated, uninitialized node children
static struct OctTree_Node* OctTree_Node_AllocateChildren();

///
//Initializes the children of an oct tree node
//
//Parameters:
//	tree: A pointer to the oct tree the children will be apart of
//	node: A pointer to the node to initialize the children of
static void OctTree_Node_InitializeChildren(OctTree* tree, struct OctTree_Node* parent);

//Functions

///
//Allocates memory for an octtree
//
//Returns:
//	Pointer to a newly allocated uninitialized oct tree
OctTree* OctTree_Allocate();

///
//Initializes an oct tree and creates a root node with the given dimensions
//
//Parameters:
//	tree: A pointer to the oct tree to initialize
//	leftBound: The left bound of the octtree
//	rightBound: The right bound of the octtree
//	bottomBound: The bottom bound of the octtree
//	topBound: The top bound of the octtree
//	backBound: The back bound of the octtree
//	frontBound: The front bound of the octtree
void OctTree_Initialize(OctTree* tree, float leftBound, float rightBound, float bottomBound, float topBound, float backBound, float frontBound);

///
//Frees the data allocated by an octtree.
//Does not free any of the data contained within the octtree!
//
//Parameters:
//	tree: A pointer to the octtree to free
void OctTree_Free(OctTree* tree);

///
//Updates the position of all gameobjects within the oct tree
//
//Parameters:
//	tree: A pointer to the oct tree to update
//	gameObjects: A linked list of all game objects currently in the simulation
void OctTree_Update(OctTree* tree, LinkedList* gameObjects);

///
//Adds a game object to the oct tree
//
//Parameters:
//	tree: A pointer to The oct tree to add a game object to
//	obj: A pointer to the game object to add
void OctTree_Add(OctTree* tree, GObject* obj);

///
//Adds a game object to the oct tree logging all nodes in which it is contained
//
//Parameters:
//	tree: A pointer to the oct tree to add a game object to
//	obj: A pointer to the game object to add
void OctTree_AddAndLog(OctTree* tree, GObject* obj);

///
//Removes a game object from the oct tree
//
//Parameters:
//	tree: A pointer the tree that I'm working with
// obj: A pointer to the object to be removed
void OctTree_Remove(OctTree* tree, GObject* obj);

///
//Removes a game object from the oct tree and the tree's log
//
//Parameters:
//	tree: The tree to remove the object from
//	obj: the object to remove
void OctTree_RemoveAndUnLog(OctTree* tree, GObject* obj);


///
//Adds a game object to a node of the oct tree
//
//Parameters:
//	tree: THe oct tree to add the object to
//	node: The node to add the object to
//	obj: A pointer to the game object being added to the tree
static void OctTree_Node_Add(OctTree* tree, struct OctTree_Node* node, GObject* obj);

///
//adds a game object to a node of the oct tree and logs all nodes in which it is contained.
//
//Parameters:
//	tree: A pointer to the oct tree the object is being added to
//	node: A pointer to the node the object is being added to
//	obj: A pointer to the game object being added to the tree
void OctTree_Node_AddAndLog(OctTree* tree, struct OctTree_Node* node, GObject* obj);

///
//Removes a game object from an oct tree node
//
//Parameters:
//	current: A pointer to the node having the object removed
//	obj: A pointer to the game object being removed
void OctTree_Node_Remove(struct OctTree_Node* current, GObject* obj);

///
//Subdivides an oct tree node into 8 child nodes, re-adding all occupants to the oct tree
//
//Parameters:
//	tree: A pointer to the oct tree in which this node lives
//	node: A pointer to the node being subdivided
static void OctTree_Node_Subdivide(OctTree* tree, struct OctTree_Node* node);

///
//Subdivides an oct tree node into 8 child nodes, re-adding all occupants to the oct tree
//Also removes the corresponding log entry for the parent node from the occupants,
//And adds the necessary child node log entries
//
//Parameters:
//	tree: A pointer to the oct tree inw hcihc this node lives
//	node: A pointer to the node being subdivided
static void OctTree_Node_SubdivideAndLog(OctTree*tree, struct OctTree_Node* node);

///
//Determines if and how a game object is colliding with an oct tree node.
//
//Parameters:
//	node: The node to check if the game object is colliding with
//	obj: The game object to test for
//
//Returns:
//	0 if the object does not collide with the octent
//	1 if the object intersects the octent but is not contained within the octent
//	2 if the object is completely contained within the octent
unsigned char OctTree_Node_DoesObjectCollide(struct OctTree_Node* node, GObject* obj);

///
//Determines if and how a sphere collider is colliding with an oct tree node.
//
//Parameters:
//	node: The node to check if the game object is colliding with
//	sphere: The sphere to test for
//	frame: The frame of reference with which to orient the sphere
//
//Returns:
//	0 if the sphere does not collide with the octent
//	1 if the sphere intersects the octent but is not contained within the octent
//	2 if the sphere is completely contained within the octent
static unsigned char OctTree_Node_DoesSphereCollide(struct OctTree_Node* node, struct ColliderData_Sphere* sphere, FrameOfReference* frame);

///
//Determines if and how an AABB is colliding with an oct tree node.
//
//Parameters:
//	node: The node to check if the game object is colliding with
//	AABB: The AABB to test for
//	frame: The frame of reference with which to orient the AABB
//
//Returns:
//	0 if the AABB does not collide with the octent
//	1 if the AABB intersects the octent but is not contained within the octent
//	2 if the AABB is completely contained within the octent
static unsigned char OctTree_Node_DoesAABBCollide(struct OctTree_Node* node, struct ColliderData_AABB* AABB, FrameOfReference* frame);

///
//Determines if and how a convex hull is colliding with an oct tree node.
//
//Parameters:
//	node: The node to check if the game object is colliding with
//	convexHull: The convex hull to test for
//	frame: The frame of reference with which to orient the convex hull
//
//Returns:
//	0 if the convexHull does not collide with the octent
//	1 if the convexHull intersects the octent but is not contained within the octent
//	2 if the convexHull is completely contained within the octent
static unsigned char OctTree_Node_DoesConvexHullCollide(struct OctTree_Node* node, struct ColliderData_ConvexHull* convexHull, FrameOfReference* frame);

///
//Definition:
//	Recurses through all Nodes and Children of an octree
//	to clean up all unneeded nodes
//
//Parameters:
//	node: the base node to check
void OctTree_Node_CleanAll(struct OctTree_Node* node);

///
//Searches up from a leaf node to find the lowest node which fully contains this object
//
//Parameters:
//	node: A pointer to The node to search up from
//	obj: A pointer to the object searching for
//
//Returns:
//	The lowest octTreeNode which fully contains the object
//	or null if no nodes do
struct OctTree_Node* OctTree_SearchUp(struct OctTree_Node* node, GObject* obj);

#endif
