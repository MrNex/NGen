#include "OctTree.h"

#include <stdlib.h>
#include <math.h>
#include <stdio.h>

///
//Static declarations
static unsigned int defaultMaxOccupancy = 3;
static unsigned int defaultMaxDepth = 3;


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
//Implementations

///
//Allocates memory for an octtree node
//
//Returns:
//	A pointer to a newly allocated, uninitialized oct tree node
static struct OctTree_Node* OctTree_Node_Allocate()
{
	struct OctTree_Node* node = (struct OctTree_Node*)malloc(sizeof(struct OctTree_Node));
	return node;
}

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
static void OctTree_Node_Initialize(struct OctTree_Node* node, OctTree* tree, struct OctTree_Node* parent, unsigned int depth, float leftBound, float rightBound, float bottomBound, float topBound, float backBound, float frontBound)
{
	//Set relative nodes
	node->children = NULL;
	node->parent = parent;

	//Create data array
	node->data = DynamicArray_Allocate();
	//Set a max capacity of the dynamic array to the max occupancy of the oct tree
	node->data->capacity = tree->maxOccupancy;
	DynamicArray_Initialize(node->data, sizeof(GObject*));

	//Set depth
	node->depth = depth;

	//Set bounds
	node->left = leftBound;
	node->right = rightBound;
	node->bottom = bottomBound;
	node->top = topBound;
	node->back = backBound;
	node->front = frontBound;
}

///
//Frees data allocated by an oct tree node
//
//Parameters:
//	node: A pointer to the oct tree node to free
static void OctTree_Node_Free(struct OctTree_Node* node)
{
	//If this node has children
	if(node->children != NULL)
	{
		for(int i = 0; i < 8; i++)
		{
			//Free the children!
			OctTree_Node_Free(node->children + i);
		}
		//Free the children container...
		free(node->children);
	}

	//Free the data contained within this tree
	DynamicArray_Free(node->data);

	//Free this node
	//free(node);
}

///
//Allocates the children of an oct tree node
//
//Returns:
//	A pointer to an array of 8 newly allocated, uninitialized node children
static struct OctTree_Node* OctTree_Node_AllocateChildren()
{
	struct OctTree_Node* children = (struct OctTree_Node*)malloc(sizeof(struct OctTree_Node) * 8);
	return children;
}

///
//Initializes the children of an oct tree node
//
//Parameters:
//	tree: A pointer to the oct tree the children will be apart of
//	node: A pointer to the node to initialize the children of
static void OctTree_Node_InitializeChildren(OctTree* tree, struct OctTree_Node* parent)
{
	//Get the half width, half depth, and half height of the parent
	float halfWidth = (parent->right - parent->left)/2.0f;
	float halfHeight = (parent->top - parent->bottom)/2.0f;
	float halfDepth = (parent->front - parent->back)/2.0f;

	//Bottom back right octant
	OctTree_Node_Initialize(parent->children, tree, parent, parent->depth + 1, 
		parent->left + halfWidth, parent->right,		//Left / Right bounds
		parent->bottom, parent->bottom + halfHeight,		//Bottom / Top bounds
		parent->back, parent->back + halfDepth);		//Back / Front bounds

	//Bottom back left octant
	OctTree_Node_Initialize(parent->children + 1, tree, parent, parent->depth + 1, 
		parent->left, parent->left + halfWidth,			//Left / Right bounds
		parent->bottom, parent->bottom + halfHeight,		//Bottom / Top bounds
		parent->back, parent->back + halfDepth);		//Back / Front bounds

	//Bottom front left octant
	OctTree_Node_Initialize(parent->children + 2, tree, parent, parent->depth + 1, 
		parent->left, parent->left + halfWidth,			//Left / Right bounds
		parent->bottom, parent->bottom + halfHeight,		//Bottom / Top bounds
		parent->back + halfDepth, parent->front);		//Back / Front bounds

	//Bottom front right octant
	OctTree_Node_Initialize(parent->children + 3, tree, parent, parent->depth + 1, 
		parent->left + halfWidth, parent->right,		//Left / Right bounds
		parent->bottom, parent->bottom + halfHeight,		//Bottom / Top bounds
		parent->back + halfDepth, parent->front);		//Back / Front bounds

	//Top back right octant
	OctTree_Node_Initialize(parent->children + 4, tree, parent, parent->depth + 1, 
		parent->left + halfWidth, parent->right,		//Left / Right bounds
		parent->bottom + halfHeight, parent->top,		//Bottom / Top bounds
		parent->back, parent->back + halfDepth);		//Back / Front bounds

	//Top back left octant
	OctTree_Node_Initialize(parent->children + 5, tree, parent, parent->depth + 1, 
		parent->left, parent->left + halfWidth,			//Left / Right bounds
		parent->bottom + halfHeight, parent->top,		//Bottom / Top bounds
		parent->back, parent->back + halfDepth);		//Back / Front bounds

	//Top front left octant
	OctTree_Node_Initialize(parent->children + 6, tree, parent, parent->depth + 1, 
		parent->left, parent->left + halfWidth,			//Left / Right bounds
		parent->bottom + halfHeight, parent->top,		//Bottom / Top bounds
		parent->back + halfDepth, parent->front);		//Back / Front bounds

	//Top front right octant
	OctTree_Node_Initialize(parent->children + 7, tree, parent, parent->depth + 1, 
		parent->left + halfWidth, parent->right,		//Left / Right bounds
		parent->bottom + halfHeight, parent->top,		//Bottom / Top bounds
		parent->back + halfDepth, parent->front);		//Back / Front bounds

}

//Functions

///
//Allocates memory for an octtree
//
//Returns:
//	Pointer to a newly allocated uninitialized oct tree
OctTree* OctTree_Allocate()
{
	//Allocate
	OctTree* tree = (OctTree*)malloc(sizeof(OctTree));

	//Assign default values
	tree->maxDepth = defaultMaxDepth;
	tree->maxOccupancy = defaultMaxOccupancy;

	return tree;
}

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
void OctTree_Initialize(OctTree* tree, float leftBound, float rightBound, float bottomBound, float topBound, float backBound, float frontBound)
{
	//Allocate root
	tree->root = OctTree_Node_Allocate();
	//Initialize root
	OctTree_Node_Initialize(tree->root, tree, NULL, 0, leftBound, rightBound, bottomBound, topBound, backBound, frontBound);

	//Allocate hashmap
	tree->map = HashMap_Allocate();
	//Initialize map
	HashMap_Initialize(tree->map);
}

///
//Frees the data allocated by an octtree.
//Does not free any of the data contained within the octtree!
//
//Parameters:
//	tree: A pointer to the octtree to free
void OctTree_Free(OctTree* tree)
{
	//Free the nodes
	OctTree_Node_Free(tree->root);
	//Free the root
	free(tree->root);
	//Free the tree!
	free(tree);
}

///
//Updates the position of all gameobjects within the oct tree
//
//Parameters:
//	tree: A pointer to the oct tree to update
//	gameObjects: A linked list of all game objects currently in the simulation
void OctTree_Update(OctTree* tree, LinkedList* gameObjects)
{
	struct LinkedList_Node* current = gameObjects->head;

	while(current != NULL)
	{
		GObject* gameObj = (GObject*)current->data;
		//Find all gameObjects which have entries in the octtree (& treemap)
		if(gameObj->collider != NULL)
		{
			//Get the treemap entry
			DynamicArray* log = (DynamicArray*)HashMap_LookUp(tree->map, &gameObj, sizeof(GObject*))->data;

			if(log->size <= 0)
			{
				printf("Not contained in any tree..\n");
				//OctTree_AddAndLog(tree, gameObj);
			}

			//For each OctTree_Node the game object was in
			for(unsigned int i = 0; i < log->size; i++)
			{
				struct OctTree_NodeStatus* nodeStatus = (struct OctTree_NodeStatus*)DynamicArray_Index(log, i);
				//get it's current status for this node
				unsigned char currentStatus = OctTree_Node_DoesObjectCollide(nodeStatus->node, gameObj);

				//If the status has remained the same, continue to the next one
				if(nodeStatus->collisionStatus == currentStatus) continue;

				//If the current status says that the object is no longer in the node
				if(currentStatus == 0)
				{

					printf("Object left node\n");

					//Remove the object from this node
					OctTree_Node_Remove(nodeStatus->node, gameObj);
					//Find where it moved
					struct OctTree_Node* containingNode = OctTree_SearchUp(nodeStatus->node, gameObj);
					//If it is still in a node
					if(containingNode != NULL)
					{
						//Add it to that node!
						OctTree_Node_AddAndLog(tree, containingNode, gameObj);
					}
					else
					{
						printf("No node found to relocate\n");
					}
					//Remove the ith nodeStatus from the log
					DynamicArray_RemoveAndReposition(log, i);
				}
				//Object was fully contained, and now it is not
				else if(currentStatus == 1)
				{
					printf("Object leaving node\n");

					//Update the status
					nodeStatus->collisionStatus = currentStatus;
					//Find where it moved
					struct OctTree_Node* containingNode = OctTree_SearchUp(nodeStatus->node, gameObj);
					//If it is even in a node anymore
					if(containingNode != NULL)
					{
						//Add it to that node!
						OctTree_Node_AddAndLog(tree, containingNode, gameObj);
					}
					else
					{
						printf("No node found to relocate\n");
					}
				}
				//Object was partially contained and now it is fully contained!
				else
				{
					printf("Object entered node\n");

					//Update the status
					nodeStatus->collisionStatus = currentStatus;
				}
			}
		}
		//Move to next object in linked list
		current = current->next;
	}

}

///
//Adds a game object to the oct tree
//
//Parameters:
//	tree: A pointer to The oct tree to add a game object to
//	obj: A pointer to the game object to add
void OctTree_Add(OctTree* tree, GObject* obj)
{
	//Add the object to the root node
	OctTree_Node_Add(tree, tree->root, obj);
}

///
//Adds a game object to the oct tree logging all nodes in which it is contained
//
//Parameters:
//	tree: A pointer to the oct tree to add a game object to
//	obj: A pointer to the game object to add
void OctTree_AddAndLog(OctTree* tree, GObject* obj)
{
	//Add the object to the root node
	OctTree_Node_AddAndLog(tree, tree->root, obj);
}

///
//Removes a game object from the oct tree
//
//Parameters:
//	current: A pointer the current node that I'm working with
//  obj: A pointer to the object to be removed
void OctTree_Remove(OctTree* tree, GObject* obj)
{
	OctTree_Node_Remove(tree->root, obj);
}

///
//Removes a game object from the oct tree and the tree's log
//
//Parameters:
//	tree: The tree to remove the object from
//	obj: the object to remove
void OctTree_RemoveAndUnLog(OctTree* tree, GObject* obj)
{
	//Remove from the treemap
	DynamicArray* objLog = (DynamicArray*)HashMap_Remove(tree->map, &obj, sizeof(GObject*));
	
	//Delete the log
	DynamicArray_Free(objLog);

	//Remove the object from octtree
	OctTree_Remove(tree, obj);
}

///
//Removes a game object from an oct tree node
//
//Parameters:
//	current: A pointer to the node having the object removed
//	obj: A pointer to the game object being removed
void OctTree_Node_Remove(struct OctTree_Node* current, GObject* obj)
{
	if(current->children != NULL)
	{
		for(int i = 0; i < 8; i++)
		{
			unsigned char collisionStatus = OctTree_Node_DoesObjectCollide(current->children + i, obj);
			if(collisionStatus == 1)
			{
				OctTree_Node_Remove(current->children+i, obj);
			}
			else if(collisionStatus == 2)
			{
				OctTree_Node_Remove(current->children+i, obj);
				break;
			}
		}
	}
	else
	{
		if(current->data->size != 0)
		{

			if(DynamicArray_ContainsWithin(current->data, &obj, current->data->size) == 1)
			{
				DynamicArray_RemoveDataAndReposition(current->data, &obj);
			}
		}
	}
}

///
//Adds a game object to a node of the oct tree
//
//Parameters:
//	tree: THe oct tree to add the object to
//	node: The node to add the object to
//	obj: A pointer to the game object being added to the tree
void OctTree_Node_Add(OctTree* tree, struct OctTree_Node* node, GObject* obj)
{

	//If this node has children, determine which children the object collides with
	if(node->children != NULL)
	{
		for(int i = 0; i < 8; i++)
		{
			unsigned char collisionStatus = OctTree_Node_DoesObjectCollide(node->children + i, obj);
			//If the object is fully contained in this node
			if(collisionStatus == 2)
			{
				//Add the nobject to this node & stop looping
				OctTree_Node_Add(tree, node->children + i, obj);
				break;
			}
			//Else if the object is partially contained in this node
			else  if(collisionStatus == 1)
			{
				//Add the object to this node & keep looping
				OctTree_Node_Add(tree, node->children + i, obj);
			}
		}
	}
	//If this node has no children
	else
	{
		//Can we hold another object? or are we too deep to subdivide?
		if(node->data->size <= tree->maxOccupancy || node->depth >= tree->maxDepth)
		{
			//Make sure we aren't already holding a pointer to the object...
			if(DynamicArray_ContainsWithin(node->data, &obj, node->data->size) == 0)
			{
				//Add the object!
				DynamicArray_Append(node->data, &obj);
			}
		}
		//Else, we are out of room and can subdivide!
		else
		{
			OctTree_Node_Subdivide(tree, node);
			//Finally, recall this function to add the object to one of the children
			OctTree_Node_Add(tree, node, obj);
		}
	}
}

///
//adds a game object to a node of the oct tree and logs all nodes in which it is contained.
//
//Parameters:
//	tree: A pointer to the oct tree the object is being added to
//	node: A pointer to the node the object is being added to
//	obj: A pointer to the game object being added to the tree
void OctTree_Node_AddAndLog(OctTree* tree, struct OctTree_Node* node, GObject* obj)
{
	//If this node has children, determine which children the object collides with
	if(node->children != NULL)
	{
		for(int i = 0; i < 8; i++)
		{
			unsigned char collisionStatus = OctTree_Node_DoesObjectCollide(node->children + i, obj);
			//If the object is fully contained in this node
			if(collisionStatus == 2)
			{
				//Add the nobject to this node & stop looping
				OctTree_Node_AddAndLog(tree, node->children + i, obj);
				break;
			}
			//Else if the object is partially contained in this node
			else  if(collisionStatus == 1)
			{
				//Add the object to this node & keep looping
				OctTree_Node_AddAndLog(tree, node->children + i, obj);
			}
		}
	}
	//If this node has no children
	else
	{
		//Can we hold another object? or are we too deep to subdivide?
		if(node->data->size <= tree->maxOccupancy || node->depth >= tree->maxDepth)
		{
			//Make sure we aren't already holding a pointer to the object...
			if(DynamicArray_ContainsWithin(node->data, &obj, node->data->size) == 0)
			{
				//Add the object!
				DynamicArray_Append(node->data, &obj);

				//Find the entry for this object in the treemap
				DynamicArray* log = NULL;

				//Is this object already contained in the map?
				if(HashMap_Contains(tree->map, &obj, sizeof(GObject*)) == 1)
				{
					log = (DynamicArray*)HashMap_LookUp(tree->map, &obj, sizeof(GObject*))->data;
				}
				else
				{
					//Add this object to the tree map
					log = DynamicArray_Allocate();
					DynamicArray_Initialize(log, sizeof(struct OctTree_NodeStatus));

					HashMap_Add(tree->map, &obj, log, sizeof(GObject*));
				}

				//Add this node to the log of the object
				//Get the containment status!
				unsigned char status = OctTree_Node_DoesObjectCollide(node, obj);
				//create a Nodestatus struct
				struct OctTree_NodeStatus entry;
				entry.node = node;
				entry.collisionStatus = status;

				//Log the entry!
				DynamicArray_Append(log, &entry);
			}
			//Else, it is already contained
			else
			{
				//Check if the status of containment in this node has changed
				//Get the object's log
				DynamicArray* log = (DynamicArray*)HashMap_LookUp(tree->map, &obj, sizeof(GObject*))->data;
				//Find the entry of this node in the log
				struct OctTree_NodeStatus* entry = NULL;
				for(unsigned int i = 0; i < log->size; i++)
				{
					struct OctTree_NodeStatus* mightBeTheEntry = (struct OctTree_NodeStatus*)DynamicArray_Index(log, i);
					//If we find it
					if(mightBeTheEntry->node == node)
					{
						entry = mightBeTheEntry;
						break;		
					}
				}

				//If this object is logged as being in this node
				if(entry != NULL)
				{

					//Compare the status of the entry to the current status
					unsigned char status = OctTree_Node_DoesObjectCollide(node, obj);
					if(entry->collisionStatus !=  status)
					{
						//Update the status
						entry->collisionStatus = status;
					}
				}
				//This object is not logged as being in this node
				else
				{
					//Create an entry
					//Get the containment status!
					unsigned char status = OctTree_Node_DoesObjectCollide(node, obj);
					//create a Nodestatus struct
					struct OctTree_NodeStatus entry;
					entry.node = node;
					entry.collisionStatus = status;

					//Log the entry!
					DynamicArray_Append(log, &entry);
				}
			}
		}
		//Else, we are out of room and can subdivide!
		else
		{
			OctTree_Node_SubdivideAndLog(tree, node);
			//Finally, recall this function to add the object to one of the children
			OctTree_Node_AddAndLog(tree, node, obj);
		}
	}
}

///
//Subdivides an oct tree node into 8 child nodes, re-adding all occupants to the oct tree
//
//Parameters:
//	tree: A pointer to the oct tree in which this node lives
//	node: A pointer to the node being subdivided
static void OctTree_Node_Subdivide(OctTree* tree, struct OctTree_Node* node)
{

	//Allocate this nodes children
	node->children = OctTree_Node_AllocateChildren();

	//Initialize this nodes children
	OctTree_Node_InitializeChildren(tree, node);

	unsigned int numOccupants = node->data->size;
	//Create a temporary list of occupants
	GObject** occupants = (GObject**)malloc(sizeof(GObject**) * numOccupants);
	//Copy occupants from node into temporary list
	memcpy(occupants, node->data->data, sizeof(GObject**) * numOccupants);
	//Clear the node's data
	DynamicArray_Clear(node->data);
	GObject* current;
	//re-add all contents to the node
	for(unsigned int i = 0; i < numOccupants; i++)
	{
		//Get the GObject* at index i
		current = occupants[i];
		//Add the GObject* back into the node
		OctTree_Node_Add(tree, node, current);
	}

	//Free the temporary list of occupants
	free(occupants);
}

///
//Subdivides an oct tree node into 8 child nodes, re-adding all occupants to the oct tree
//Also removes the corresponding log entry for the parent node from the occupants,
//And adds the necessary child node log entries
//
//Parameters:
//	tree: A pointer to the oct tree inw hcihc this node lives
//	node: A pointer to the node being subdivided
static void OctTree_Node_SubdivideAndLog(OctTree*tree, struct OctTree_Node* node)
{
	//Allocate this nodes children
	node->children = OctTree_Node_AllocateChildren();

	//Initialize this nodes children
	OctTree_Node_InitializeChildren(tree, node);

	unsigned int numOccupants = node->data->size;
	//Create a temporary list of occupants
	GObject** occupants = (GObject**)malloc(sizeof(GObject**) * numOccupants);

	//Copy occupants from node into temporary list
	memcpy(occupants, node->data->data, sizeof(GObject**) * numOccupants);

	//Clear the node's data
	DynamicArray_Clear(node->data);

	//re-add all contents to the node
	GObject* current;
	for(unsigned int i = 0; i < numOccupants; i++)
	{
		//Get the GObject* at index i
		current = occupants[i];

		//Get this log of this object
		DynamicArray* log = (DynamicArray*)HashMap_LookUp(tree->map, &current, sizeof(GObject*))->data;
		//Find parent node in the log
		for(unsigned int j = 0; j < log->size; j++)
		{
			if(((struct OctTree_NodeStatus*)DynamicArray_Index(log, j))->node == node)
			{
				//And remove it
				DynamicArray_Remove(log, j);
				break;
			}
		}

		//Add the GObject* back into the node
		OctTree_Node_AddAndLog(tree, node, current);
	}

	//Free the temporary list of occupants
	free(occupants);
}

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
unsigned char OctTree_Node_DoesObjectCollide(struct OctTree_Node* node, GObject* obj)
{
	unsigned char collisionStatus = 0;
	//Determine which frame of reference we will be using to orient the object
	FrameOfReference* primaryFrame;
	if(obj->body != NULL)
	{
		primaryFrame = obj->body->frame;
	}
	else
	{
		primaryFrame = obj->frameOfReference;
	}

	//Determine the type of collider the object has
	switch(obj->collider->type)
	{
	case COLLIDER_SPHERE:
		collisionStatus = OctTree_Node_DoesSphereCollide(node, obj->collider->data->sphereData, primaryFrame);
		break;
	case COLLIDER_AABB:
		collisionStatus = OctTree_Node_DoesAABBCollide(node, obj->collider->data->AABBData, primaryFrame);
		break;
	case COLLIDER_CONVEXHULL:
		collisionStatus = OctTree_Node_DoesConvexHullCollide(node, obj->collider->data->convexHullData, primaryFrame);
		break;
	}

	return collisionStatus;
}


///
//Determines if and how a sphere collider is colliding with an oct tree node.
//
//Parameters:
//	node: The node to check if the game object is colliding with
//	sphere: The sphere to test for
//	frame: The frame of reference to orient the sphere when checking
//
//Returns:
//	0 if the sphere does not collide with the octent
//	1 if the sphere intersects the octent but is not contained within the octent
//	2 if the sphere is completely contained within the octent
static unsigned char OctTree_Node_DoesSphereCollide(struct OctTree_Node* node, struct ColliderData_Sphere* sphere, FrameOfReference* frame)
{
	unsigned char collisionStatus = 0;


	//Find the scaled radius
	float scaledRadius = SphereCollider_GetScaledRadius(sphere, frame);


	float bounds[6] = 
	{
		frame->position->components[0] - scaledRadius,
		frame->position->components[0] + scaledRadius,
		frame->position->components[1] - scaledRadius,
		frame->position->components[1] + scaledRadius,
		frame->position->components[2] - scaledRadius,
		frame->position->components[2] + scaledRadius
	};

	//Determine if the bounds overlap
	unsigned char overlap = 0;

	if(node->left <= bounds[1] && node->right >= bounds[0])
	{
		if(node->bottom <= bounds[3] && node->top >= bounds[2])
		{
			if(node->back <= bounds[5] && node->front >= bounds[4])
			{
				overlap = 1;
			}
		}
	}
	//Set the collision status
	collisionStatus = overlap;

	//If we found that the bounds do overlap, we must check if the node contains the sphere
	if(collisionStatus == 1)
	{
		overlap = 0;
		if(node->left <= bounds[0] && node->right >= bounds[1])
		{
			if(node->bottom <= bounds[2] && node->top >= bounds[3])
			{
				if(node->back <= bounds[4] && node->front >= bounds[5])
				{
					overlap = 1;
				}
			}
		}
		//Update collision status
		collisionStatus += overlap;
	}

	return collisionStatus;
}

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
static unsigned char OctTree_Node_DoesAABBCollide(struct OctTree_Node* node, struct ColliderData_AABB* AABB, FrameOfReference* frame)
{
	unsigned char collisionStatus = 0;

	//Get centroid in world space to have the real center of the AABB
	Vector pos;
	Vector_INIT_ON_STACK(pos, 3);
	Vector_Add(&pos, AABB->centroid, frame->position);

	//Get the scaled dimensions of AABB
	struct ColliderData_AABB scaled;
	AABBCollider_GetScaledDimensions(&scaled, AABB, frame);

	float bounds[6] = 
	{
		pos.components[0] - scaled.width / 2.0f,
		pos.components[0] + scaled.width / 2.0f,
		pos.components[1] - scaled.height / 2.0f,
		pos.components[1] + scaled.height / 2.0f,
		pos.components[2] - scaled.depth / 2.0f,
		pos.components[2] + scaled.depth / 2.0f
	};

	unsigned char overlap = 0;
	if(node->left <= bounds[1] && node->right >= bounds[0])
	{
		if(node->bottom <= bounds[3] && node->top >= bounds[2])
		{
			if(node->back <= bounds[5] && node->front >= bounds[4])
			{
				overlap = 1;
			}
		}
	}
	//Set the collision status
	collisionStatus = overlap;

	//If we found that the bounds do overlap, we must check if the node contains the sphere
	if(collisionStatus == 1)
	{

		overlap = 0;
		if(node->left <= bounds[0] && node->right >= bounds[1])
		{
			if(node->bottom <= bounds[2] && node->top >= bounds[3])
			{
				if(node->back <= bounds[4] && node->front >= bounds[5])
				{
					overlap = 1;
				}
			}
		}
		//Update collision status
		collisionStatus += overlap;

	}

	return collisionStatus;

}

///
//Determines if and how a convex hull is colliding with an oct tree node.
//Uses the minimum AABB as a test, not the convex hull itself.
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
static unsigned char OctTree_Node_DoesConvexHullCollide(struct OctTree_Node* node, struct ColliderData_ConvexHull* convexHull, FrameOfReference* frame)
{
	//Generate the minimum AABB from the convex hull
	struct ColliderData_AABB AABB;
	Vector AABBCentroid;
	Vector_INIT_ON_STACK(AABBCentroid, 3);
	AABB.centroid = &AABBCentroid;

	ConvexHullCollider_GenerateMinimumAABB(&AABB, convexHull, frame);

	//Then use the AABB test
	return OctTree_Node_DoesAABBCollide(node, &AABB, frame);
}


///
//Definition:
//	Recurses through all Nodes and Children of an octree
//	to clean up all unneeded nodes
//
//Parameters:
//	node: the base node to check
void OctTree_Node_CleanAll(struct OctTree_Node* node)
{
	// Does the current node have children?
	if(node->children != NULL)
	{
		// Create a 'bool' to check if the children has children
		unsigned char isGrandfather = 0;

		// Loop through all children of current node
		for(int i = 0; i < 8; i++)
		{
			// If they have children, current node is a 'Grandfather'
			if (node->children[i].children == NULL)
			{
				isGrandfather = 1;
			}
		}

		// If the current node is a 'Grandfather'...
		if(isGrandfather == 1)
		{
			// Recurse through all the children of this node
			for(int i = 0; i < 8; i++)
			{
				OctTree_Node_CleanAll(node->children+i);
			}
		}

		// If the current node isn't a 'Grandfather'
		else
		{
			// Does the current node have occupants in it's children?
			unsigned char hasOccupants = 0;
			for(int i = 0; i < 8; i++)
			{
				if(node->children[i].data->size == 0)
				{
					// if so, se hasOccupants to true
					hasOccupants = 1;
				}
			}

			// If the current node has no occupants
			if(hasOccupants == 0)
			{
				// Clean out all the children
				for(int i = 0; i < 8; i++)
				{
					OctTree_Node_Free(node->children+i);
				}
				free(node->children);
				node->children = 0;
			}
		}
	}
}

///
//Searches up from a leaf node to find the lowest node which fully contains this object
//
//Parameters:
//	node: A pointer to The node to search up from
//	obj: A pointer to the object searching for
//Returns:
//	The lowest octTreeNode which fully contains the object
//	or null if no nodes do
struct OctTree_Node* OctTree_SearchUp(struct OctTree_Node* node, GObject* obj)
{
	struct OctTree_Node* fullyContainedWithin = node;
	while(fullyContainedWithin != NULL)
	{
		if(OctTree_Node_DoesObjectCollide(fullyContainedWithin, obj) == 2) break;
		else fullyContainedWithin = fullyContainedWithin->parent;
	}
	return fullyContainedWithin;
}
