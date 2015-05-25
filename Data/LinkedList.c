#include "LinkedList.h"

#include <stdlib.h>

//Functions

///
//Allocates a linked list
//Returns:
//	Pointer to newly allocated linked list
LinkedList* LinkedList_Allocate(void)
{
	LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
	return list;
}

///
//Initializes a linked list
//Parameters:
//	list: The linked list to initialize
void LinkedList_Initialize(LinkedList* list)
{
	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
}


///
//Frees resources taken by a linked list
//Parameters:
//	list: The linked list to free
void LinkedList_Free(LinkedList* list)
{
	struct LinkedList_Node* current = list->head;

	while (current != NULL)
	{
		struct LinkedList_Node* next = current->next;
		LinkedList_Node_Free(current);
		current = next;
	}

	free(list);
}

///
//Searches a linked list for a value
//
//Parameters:
//	list: Linked list to search
//	data: Data to find
//
//Returns:
//	Pointer to node containing the specified data
//	Or NULL if data is not found in list
struct LinkedList_Node* LinkedList_SearchByValue(LinkedList* list, void* data)
{
	if (list->head == NULL) return NULL;
	struct LinkedList_Node* current = list->head;

	while (current->data != data)
	{
		current = current->next;
		if (current == NULL)
		{
			return NULL;
		}
	}

	return current;
}

///
//Searches a linked list for an index
//
//Parameters:
//	list: The linked list to search
//	index: The index to search for
//
//Returns:
//	Pointer to LinkedList_Node at the specified index
//	Or NULL if index is out of bounds of list
struct LinkedList_Node* LinkedList_SearchByIndex(LinkedList* list, unsigned int index)
{
	if (index >= list->size) return NULL;

	struct LinkedList_Node* current = list->head;
	for (unsigned int i = 0; i < index; i++)
	{
		current = current->next;
	}

	return current;
}

///
//Appends a linked list with a new node containing specified data
//
//Parameters:
//	list: The list to append
//	data: The data to append with
void LinkedList_Append(LinkedList* list, void* data)
{
	struct LinkedList_Node* node = LinkedList_Node_Allocate();
	LinkedList_Node_Initialize(node, data);

	//Case 1: There is no head
	if (list->size == 0)
	{
		list->head = node;
		list->tail = node;
	}
	else
	{
		list->tail->next = node;
		node->previous = list->tail;
		list->tail = list->tail->next;
	}

	list->size++;
}

///
//Prependsa linked list with a new node containing specified data
//
//Parameters:
//	list: The list to prepend
//	data: The data to prepend with
void LinkedList_Prepend(LinkedList* list, void* data)
{
	struct LinkedList_Node* node = LinkedList_Node_Allocate();
	LinkedList_Node_Initialize(node, data);

	if (list->size == 0)
	{
		list->head = node;
		list->tail = node;
	}
	else
	{
		node->next = list->head;
		list->head->previous = node;
		list->head = node;
	}

	list->size++;
}

///
//Inserts a new node containing specified data into the linked list
//
//Parameters:
//	list: The list to insert something into
//	index: the index to insert at
//	data: the data to insert
void LinkedList_Insert(LinkedList* list, unsigned int index, void* data)
{
	if (index >= list->size)
		LinkedList_Append(list, data);
	else if (index <= 0)
		LinkedList_Prepend(list, data);
	else
	{
		struct LinkedList_Node* node = LinkedList_Node_Allocate();
		LinkedList_Node_Initialize(node, data);

		struct LinkedList_Node* current = list->head;

		for (unsigned int i = 0; i < index; i++)
		{
			current = current->next;
		}

		current->previous->next = node;
		node->previous = current->previous;
		node->next = current;
		current->previous = node;

		list->size++;
	}
}

///
//Removes a node from a linked list at the specified index
//
//Parameters:
//	list: The list to remove something from
//	index: The index of the node to remove
void LinkedList_RemoveIndex(LinkedList* list, unsigned int index)
{
	//Get the node at index
	struct LinkedList_Node* nodeToRemove = LinkedList_SearchByIndex(list, index);

	if (nodeToRemove != NULL) LinkedList_RemoveNode(list, nodeToRemove);
}

///
//Removes the first node from a linked list containing the specified data
//
//Parameters:
//	list: The linked list to remove from
//	data: The data to remove
void LinkedList_RemoveValue(LinkedList* list, void* data)
{
	struct LinkedList_Node* nodeToRemove = LinkedList_SearchByValue(list, data);

	if (nodeToRemove != NULL) LinkedList_RemoveNode(list, nodeToRemove);
}

///
//Removes all nodes from a linked list which contain the specified data
//
//Parameters:
//	list: The linked list to remove values from
//	data: The data to remove
void LinkedList_RemoveAllValues(LinkedList* list, void* data)
{
	struct LinkedList_Node* current = list->head;
	struct LinkedList_Node* next;
	while (current != NULL)
	{
		next = current->next;
		if (current->data == data)
		{
			LinkedList_RemoveNode(list, current);
		}
		current = next;
	}
}

///
//Removes a node from a linked list
//
//Parameters:
//	list: The list to rmeove from
//	node: The node to remove
void LinkedList_RemoveNode(LinkedList* list, struct LinkedList_Node* nodeToRemove)
{
	//Case 1: Removing head and it is the only node in list
	if (nodeToRemove == list->head && nodeToRemove == list->tail)
	{
		list->head = NULL;
		list->tail = NULL;
		LinkedList_Node_Free(nodeToRemove);
	}
	else
	{
		if (nodeToRemove == list->head)
		{
			list->head = nodeToRemove->next;
			list->head->previous = NULL;
			LinkedList_Node_Free(nodeToRemove);
		}
		else if (nodeToRemove == list->tail)
		{
			list->tail = nodeToRemove->previous;
			list->tail->next = NULL;
			LinkedList_Node_Free(nodeToRemove);
		}
		else
		{
			nodeToRemove->next->previous = nodeToRemove->previous;
			nodeToRemove->previous->next = nodeToRemove->next;
			LinkedList_Node_Free(nodeToRemove);
		}
	}
	list->size--;
}

///
//Clears a list of all nodes
//Does not delete the data in the nodes!
//
//Parameters:
//	list: The linked list to clear
void LinkedList_Clear(LinkedList* list)
{
	struct LinkedList_Node* current = list->head;
	struct LinkedList_Node* next = NULL;
	while(current != NULL)
	{
		next = current->next;

		LinkedList_Node_Free(current);
		current = next;
	}

	list->head = NULL;
	list->tail = NULL;
	list->size = 0;
}

//Internals

///
//Allocates a Node for a Linked List
//
//Returns:
//	Newly allocated Pointer to linked list node
static struct LinkedList_Node* LinkedList_Node_Allocate(void)
{
	struct LinkedList_Node* node = (struct LinkedList_Node*)malloc(sizeof(struct LinkedList_Node));
	return node;
}

///
//Initializes a new node for a linked list
//
//Parameters:
//	node: pointer to node to initialize
//	data: Data to initialize node with
static void LinkedList_Node_Initialize(struct LinkedList_Node* node, void* data)
{
	node->data = data;
	node->next = NULL;
	node->previous = NULL;
}

///
//Frees resources being used by a Linked List node
//Does NOT free the data in the node. If the data is on the heap, delete before this!
//
//Parameters:
//	node: The node to free
static void LinkedList_Node_Free(struct LinkedList_Node* node)
{
	free(node);
}

