#ifndef LINKEDLIST_H
#define LINKEDLIST_H

struct LinkedList_Node
{
	void* data;
	struct LinkedList_Node* next;
	struct LinkedList_Node* previous;
};

typedef struct LinkedList
{
	struct LinkedList_Node* head;
	struct LinkedList_Node* tail;
	unsigned int size;
} LinkedList;

//Internals

///
//Allocates a Node for a Linked List
//
//Returns:
//	Newly allocated Pointer to linked list node
static struct LinkedList_Node* LinkedList_Node_Allocate(void);

///
//Initializes a new node for a linked list
//
//Parameters:
//	node: pointer to node to initialize
//	data: Data to initialize node with
static void LinkedList_Node_Initialize(struct LinkedList_Node* node, void* data);

///
//Frees resources being used by a Linked List node
//
//Parameters:
//	node: The node to free
static void LinkedList_Node_Free(struct LinkedList_Node* node);


//Functions

///
//Allocates a linked list
//Returns:
//	Pointer to newly allocated linked list
LinkedList* LinkedList_Allocate(void);

///
//Initializes a linked list
//Parameters:
//	list: The linked list to initialize
void LinkedList_Initialize(LinkedList* list);

///
//Frees resources taken by a linked list
//Parameters:
//	list: The linked list to free
void LinkedList_Free(LinkedList* list);


///
//Searches a linked list for a value
//
//Parameters:
//	list: Linked list to search
//	data: Data to find
//
//Returns:
//	Pointer to node containing the specified data
struct LinkedList_Node* LinkedList_SearchByValue(LinkedList* list, void* data);

///
//Searches a linked list for an index
//
//Parameters:
//	list: The linked list to search
//	index: The index to search for
//
//Returns:
//	Pointer to LinkedList_Node at the specified index
struct LinkedList_Node* LinkedList_SearchByIndex(LinkedList* list, unsigned int index);

///
//Appends a linked list with a new node containing specified data
//
//Parameters:
//	list: The list to append
//	data: The data to append with
void LinkedList_Append(LinkedList* list, void* data);

///
//Prependsa linked list with a new node containing specified data
//
//Parameters:
//	list: The list to prepend
//	data: The data to prepend with
void LinkedList_Prepend(LinkedList* list, void* data);

///
//Inserts a new node containing specified data into the linked list
//
//Parameters:
//	list: The list to insert something into
//	index: the index to insert at
//	data: the data to insert
void LinkedList_Insert(LinkedList* list, unsigned int index, void* data);

///
//Removes a node from a linked list at the specified index
//
//Parameters:
//	list: The list to remove something from
//	index: The index of the node to remove
void LinkedList_RemoveIndex(LinkedList* list, unsigned int index);

///
//Removes the first node from a linked list containing the specified data
//
//Parameters:
//	list: The linked list to remove from
//	data: The data to remove
void LinkedList_RemoveValue(LinkedList* list, void* data);

///
//Removes all nodes from a linked list which contain the specified data
//
//Parameters:
//	list: The linked list to remove values from
//	data: The data to remove
void LinkedList_RemoveAllValues(LinkedList* list, void* data);


///
//Removes a node from a linked list
//
//Parameters:
//	list: The list to rmeove from
//	node: The node to remove
void LinkedList_RemoveNode(LinkedList* list, struct LinkedList_Node* node);

///
//Clears a list of all nodes
//Does not delete the data in the nodes!
//
//Parameters:
//	list: The linked list to clear
void LinkedList_Clear(LinkedList* list);

#endif
