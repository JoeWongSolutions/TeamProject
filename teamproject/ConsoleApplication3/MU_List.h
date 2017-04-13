#ifndef MU_LIST_H
#define MU_LIST_H

#include <stdlib.h>

struct MU_Node {
	void* data;

	struct MU_Node* next;
	struct MU_Node* prev;
};

struct MU_List {
	struct MU_Node* head;
	struct MU_Node* tail;
};

struct MU_List* MU_List_create();
void MU_List_destroy(struct MU_List* list);

void MU_List_traverse(struct MU_List* list, void(*callback)(void*));

void MU_List_insert_front(struct MU_List* list, void* data);
void MU_List_insert_back(struct MU_List* list, void* data);
void MU_List_insert_after(struct MU_List* list, struct MU_Node* node, void* data);
void MU_List_insert_before(struct MU_List* list, struct MU_Node* node, void* data);

void MU_List_delete_from(struct MU_List* list, struct MU_Node* node);

#endif

