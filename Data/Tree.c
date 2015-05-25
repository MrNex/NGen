#include "Tree.h"

#include <stdlib.h>
#include <string.h>

///
//Allocates a tree in memory
//
//Returns:
//	Pointer to newly allocated tree
Tree* Tree_Allocate(void)
{
	Tree* tree = (Tree*)malloc(sizeof(Tree));
	return tree;
}

///
//Initializes a tree
//
//Parameters:
//	tree: The tree being initialized
void Tree_Initialize(Tree* tree)
{
	tree->root = NULL;
}

///
//Frees a tree
//
//Parameters:
//	treeFree: The tree to free
void Tree_Free(Tree* treeFree)
{
	//Depth first tree deletion
	struct Tree_Node* current = treeFree->root;
	
}

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
Tree_Node* Tree_BreadthFirstDataSearch(Tree* tree, void* data)
{
	if(tree->root == NULL) return NULL;

	return Tree_Node_BreadthFirstDataSearch(tree->root, data);
}

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
Tree_Node* Tree_Node_BreadthFirstDataSearch(Tree_Node* treenode, void* data)
{
	LinkedList* toVisit = LinkedList_Allocate();
	LinkedList_Initialize(toVisit);


	Tree_Node* current = NULL;
	
	LinkedList_Append(toVisit, treenode);

	while(toVisit->size > 0)
	{
		current = (Tree_Node*)toVisit->head->data;
		LinkedList_RemoveIndex(toVisit, 0);

		if(current->data == data)
		{
			return current;
		}

		LinkedList_Node* currentChild = current->children->head;
		LinkedList_Node* next = NULL;
		while(currentChild != NULL)
		{
			next = currentChild->next;
			LinkedList_Append(toVisit, (Tree_Node*)currentChild->data);
			currentChild = next;
		}
	
	}

	//If loop ends data was not found.
	return NULL;
}

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
Tree_Node* Tree_BreadthFirstSearch(Tree* tree, char* id)
{
	if(tree->root == NULL) return NULL;
	return Tree_Node_BreadthFirstSearch(tree->root, id);
}

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
Tree_Node* Tree_Node_BreadthFirstSearch(Tree_Node* treenode, char* id)
{
	LinkedList* toVisit = LinkedList_Allocate();
	LinkedList_Initialize(toVisit);

	struct Tree_Node* current = NULL;
	LinkedList_Append(toVisit, treenode);

	while(toVisit->size > 0)
	{
		current = (struct Tree_Node*)toVisit->head->data;
		LinkedList_RemoveIndex(toVisit, 0);

		if(strcmp(current->id, id) == 0)
		{
			return current;
		}

		LinkedList_Node* currentChild = current->children->head;
		LinkedList_Node* next = NULL;
		while(currentChild != NULL)
		{
			next = currentChild->next;
			LinkedList_Append(toVisit, (struct Tree_Node*)currentChild->data);
			currentChild = next;
		}
	}

	return NULL;
}



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
Tree_Node* Tree_DepthFirstDataSearch(Tree* tree, void* data)
{
	return Tree_Node_DepthFirstDataSearch(tree->root, data);
}




///folf
//Searches a node in a depth first search manner for data
//
//Parameters:
//	current: The current node being searched
//	data: Pointer to The data searching for
//
//Returns:
//	Pointer to tree node containing requested data
//	NuLL if data wasn't found
struct Tree_Node* Tree_Node_DepthFirstDataSearch(struct Tree_Node* current, void* data)
{
	LinkedList_Node* currentChild = current->children->head;
	LinkedList_Node* next = NULL;
	while(current != NULL)
	{
		next = currentChild->next;
		struct Tree_Node* result = Tree_Node_DepthFirstDataSearch((struct Tree_Node*)currentChild->data, data);
		if(result != NULL) return result;
		currentChild = next;
	}

	//All children checked, check self
	if(current->data == data) return current;
	//Not self, return null
	return NULL;
}


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
Tree_Node* Tree_DepthFirstSearch(Tree* tree, char* id)
{
	return Tree_Node_DepthFirstSearch(tree->root, id);
}

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
Tree_Node* Tree_Node_DepthFirstSearch(Tree_Node* current, char* id)
{
	LinkedList_Node* currentChild = current->children->head;
	LinkedList_Node* next = NULL;

	while(current != NULL)
	{
		next = currentChild->next;
		struct Tree_Node* result = Tree_Node_DepthFirstSearch((struct Tree_Node*)currentChild->data, id);
		if(result != NULL)return result;
		currentChild = next;
	}

	if(strcmp(current->id, id) == 0) return current;
	return NULL;
}

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
struct Tree_Node* Tree_SequenceDataSearch(Tree* tree, void** data, unsigned int length)
{
	if(tree->root == NULL) return NULL;

	Tree_Node* current = tree->root;
	
	for(unsigned int i = 0; i < length; i++)
	{
		current = Tree_Node_BreadthFirstDataSearch(current, data[i]);
		if(current == NULL) return NULL;
	}

	return current;
}

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
Tree_Node* Tree_SequenceSearch(Tree* tree, char** ids, unsigned int len)
{
	if(tree->root == NULL) return NULL;

	Tree_Node* current = tree->root;

	for(unsigned int i = 0; i < len; i++)
	{
		current = Tree_Node_BreadthFirstSearch(current, ids[i]);
		if(current == NULL) return NULL;
	}

	return current;
}

///
//Allocates a Tree_Node
//
//Returns:
//	Pointer to a newly allocated tree node
struct Tree_Node* Tree_Node_Allocate(void)
{
	struct Tree_Node* treenode = (struct Tree_Node*)malloc(sizeof(struct Tree_Node));
	return treenode;
}

///
//Initializes a Tree node
//
//Parameters:
//	node: The node to initialize
//	id: The id to find the node with later
void Tree_Node_Initialize(Tree_Node* node, const char* id, void* data)
{
	if(id == NULL)	//If no ID was passed
	{
		node->id = NULL;
	}
	else
	{
		int len = (strlen(id) + 1);
		node->id = (char*)malloc(sizeof(char) * len);	//Leave room for null terminator
		strcpy_s(node->id, len - 1, id);
		node->id[len] = '\0';
	}
	node->data = data;
	node->children = LinkedList_Allocate();
	LinkedList_Initialize(node->children);
}

///
//Frees memory allocated by a tree node
//Frees all children first.
//
//Does not free data!!
void Tree_Node_Free(struct Tree_Node* node)
{
	LinkedList_Node* currentChild = node->children->head;
	LinkedList_Node* next = NULL;
	while(currentChild != NULL)
	{
		next = currentChild->next;
		Tree_Node_Free((struct Tree_Node*)currentChild->data);
		currentChild = next;
	}

	//Free self
	LinkedList_Free(node->children);
	free(node);
}

///
//Adds a new child node to a treenode
//To add an existing node as a child see AttachChild
//
//Parameters:
//	node: The node to add a child to
//	data: The data of the child to add
void Tree_Node_AddChild(Tree_Node* node, const char* id, void* data)
{
	struct Tree_Node* newNode = Tree_Node_Allocate();
	Tree_Node_Initialize(newNode, id, data);

	Tree_Node_AttachChild(node, newNode);
}

///
//Attaches an already existing treenode as a child of another tree node
//
//Parameters:
//	node: The tree node to attach a child to
//	child: The child node to attach to the tree node
void Tree_Node_AttachChild(Tree_Node* node, Tree_Node* child)
{
	LinkedList_Append(node->children, child);

}
