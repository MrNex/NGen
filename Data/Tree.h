#ifndef TREE_H
#define TREE_H

#include "LinkedList.h"

struct Tree_Node
{
	char* id;
	void* data;
	LinkedList* children;
} ;

typedef struct Tree
{
	struct Tree_Node* root;
} Tree;

///
//Allocates a tree in memory
//
//Returns:
//	Pointer to newly allocated tree
Tree* Tree_Allocate(void);

///
//Initializes a tree
//
//Parameters:
//	tree: The tree being initialized
void Tree_Initialize(Tree* tree);

///
//Frees a tree
//
//Parameters:
//	treeFree: The tree to free
void Tree_Free(Tree* treeFree);

///
//Searches a tree for data
//In a breadth first manner
//
//Parameters:
//	tree: The tree to search
//	data: The data to search for
//
//Returns:
//	A pointer to the node containing the requested data
//	NULL if data wasn't found
Tree_Node* Tree_BreadthFirstDataSearch(Tree* tree, void* data);

///
//Searches a tree for an ID in a breadth first manner
//
//Parameters:
//	tree: The treeto search
//	id: The ID of the node being searched for
//
//Returns:
//	Pointer to tree node with the requested ID
//	or NULL if ID was not found.
Tree_Node* Tree_BreadthFirstSearch(Tree* tree, char* id);

///
//Searches breadth first down from a node for data
//
//Parameters:
//	treenode: The treenode to search down from
//	data: The data to search for
//
//Returns:
//	Pointer to tree node containing the data requested
//	Or null if the data wasn't found
Tree_Node* Tree_Node_BreadthFirstDataSearch(Tree_Node* treenode, void* data);

///
//Searches breadth first down from a node for an ID
//
//PArameters:
//	treenode: The node to search from
//	id: The ID searching for
//
//Returns:
//	Pointer to tree node containing the requested ID
//	or NULL if node wasn't found
Tree_Node* Tree_Node_BreadthFirstSearch(Tree_Node* treenode, char* id);


///
//Searches a tree for data
//In a depth first manner
//
//Parameters:
//	tree: The tree to search
//	data: The data to search for
//
//Returns:
//	A pointer to the node containing the requested data
//	NULL if data wasn't found
Tree_Node* Tree_DepthFirstDataSearch(Tree* tree, void* data);

///
//Searches a tree depth first for an ID
//
//Parameters:
//	tree: Tree to search
//	id: ID to search for
//
//Returns:
//	Pointer to Node with requested ID
//	or NULL if the ID wasn't found
Tree_Node* Tree_DepthFirstSearch(Tree* tree, char* id);

///
//Searches a node in a depth first search manner for data
//
//Parameters:
//	current: The current node being searched
//	data: Pointer to The data searching for
Tree_Node* Tree_Node_DepthFirstDataSearch(Tree_Node* current, void* data);

///
//Searches a node in a depth first search manner for the data
//
//Parameters:
//	current: The current node being searched
//	id: The ID to search for
//
//Returns:
//	A pointer to the Tree Node with the proper ID
//	OR NuLL if the ID was not found.
Tree_Node* Tree_Node_DepthFirstSearch(Tree_Node* current, char* id);

///
//Searches the tree for a sequence of data
//Follows the sequence through the tree returningthe node containing the last 
//data in the sequence.
//Uses a breadth first search.
//
//Parameters:
//	tree: The tree to search
//	data: An array of pointers to data to follow
//
//Returns:
//	Pointer to tree node containing the last data in the sequence.
Tree_Node* Tree_SequenceDataSearch(Tree* tree, void** data, unsigned int len);

///
//Searches a tree for a sequence of ID's
//Follows the sequence through the tree returning the node
//Containing the last ID in the list
//Uses breadth first search method
//
//Parameters:
//	tree: The tree to search
//	ids: The sequence of IDs to track
//	len: The length of the sequence
//
//PArameters:
//	Returns the tree node containing the last ID in the list
//	Or NULL if an ID could not be found.
Tree_Node* Tree_SequenceSearch(Tree* tree, char** ids, unsigned int len);


///
//Allocates a Tree_Node
//
//Returns:
//	Pointer to a newly allocated tree node
struct Tree_Node* Tree_Node_Allocate(void);

///
//Initializes a Tree node
//
//Parameters:
//	node: The node to initialize
void Tree_Node_Initialize(Tree_Node* node, const char* id, void* data);

///
//Frees memory allocated by a tree node
//
//Does not free data!!
void Tree_Node_Free(Tree_Node* node);

///
//Adds a new child node to a treenode
//To add an existing node as a child see AttachChild
//
//Parameters:
//	node: The node to add a child to
//	data: The data of the child to add
void Tree_Node_AddChild(Tree_Node* node, const char* id, void* data);

///
//Attaches an already existing treenode as a child of another tree node
//
//Parameters:
//	node: The tree node to attach a child to
//	child: The child node to attach to the tree node
void Tree_Node_AttachChild(Tree_Node* node, Tree_Node* child);

#endif
