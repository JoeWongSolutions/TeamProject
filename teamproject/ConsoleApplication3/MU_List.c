#include "MU_List.h"

struct MU_List* MU_List_create()
{
	struct MU_List* list = (struct MU_List *) malloc(sizeof (struct MU_List));
	list->head = list->tail = NULL;

	return list;
}

void MU_List_destroy(struct MU_List* list)
{
	struct MU_Node* current = list->head;
	struct MU_Node* next = NULL;

	while (current) {
		next = current->next;
		free(current);
		current = next;
	}

	free(list);
}

void MU_List_traverse(struct MU_List* list, void(*callback)(void* data))
{
	struct MU_Node* current = list->head;

	while (current) {
		callback(current->data);
		current = current->next;
	}
}

/*
* When prev is NULL it inserts before the head
*/
void MU_List_insert_after(struct MU_List* list, struct MU_Node* prev, void* data)
{
	struct MU_Node* next = prev ? prev->next : list->head;

	struct MU_Node* newnode = (struct MU_Node *) malloc(sizeof (struct MU_Node));
	newnode->data = data;

	newnode->prev = prev;
	newnode->next = next;

	prev ? prev->next = newnode : 0;
	next ? next->prev = newnode : 0;

	list->head == NULL ? list->head = list->tail = newnode :
		prev == NULL ? list->head = newnode : 0;
}

/*
* When next is NULL it inserts after the tail
*/
void MU_List_insert_before(struct MU_List* list, struct MU_Node* next, void* data) 
{
	struct MU_Node* prev = next ? next->prev : list->tail;

	struct MU_Node* newnode = (struct MU_Node *) malloc(sizeof (struct MU_Node));
	newnode->data = data;

	newnode->prev = prev;
	newnode->next = next;

	next ? next->prev = newnode : 0;
	prev ? prev->next = newnode : 0;

	list->tail == NULL ? list->head = list->tail = newnode :
		next == NULL ? list->tail = newnode : 0;
}

void MU_List_insert_front(struct MU_List* list, void* data) 
{
	MU_List_insert_after(list, NULL, data);
}

void MU_List_insert_back(struct MU_List* list, void* data) 
{
	MU_List_insert_before(list, NULL, data);
}

void MU_List_delete_from(struct MU_List* list, struct MU_Node* node) 
{
	struct MU_Node* prev = node->prev;
	struct MU_Node* next = node->next;

	prev ? prev->next = next : 0;
	next ? next->prev = prev : 0;

	node == list->head ? list->head = next : 0;
	node == list->tail ? list->tail = prev : 0;

	free(node);
}
