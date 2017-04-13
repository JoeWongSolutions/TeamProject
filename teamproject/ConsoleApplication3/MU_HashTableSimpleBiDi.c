
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include "MU_HashTableSimpleBiDi.h"

struct MU_HashTableSimpleBiDi *MU_HashTableSimpleBiDi_create(unsigned int size)
{
	struct MU_HashTableSimpleBiDi *p;

	p = calloc(1, sizeof(*p));
	if (p == NULL)
		return NULL;

	p->nodes = MU_HashTable_create(NULL, NULL, size);
	if (p->nodes == NULL) {
		free(p);
		return NULL;
	}

	p->reverseNodes = MU_HashTable_create(MU_HashTable_index_hash, MU_HashTable_index_hash_cmp, size);
	if (p->reverseNodes == NULL) {
		MU_HashTable_destroy(p->nodes, 0, 0);
		free(p);
		return NULL;
	}

	return p;
}

void MU_HashTableSimpleBiDi_destroy(struct MU_HashTableSimpleBiDi *h)
{
	MU_HashTable_destroy(h->nodes, 0, 0);         // Note: the memory of the string should be freed
	MU_HashTable_destroy(h->reverseNodes, 0, 0);  // Note: the memory of the string was freed. Don't free it twice.
	free(h);
}

int MU_HashTableSimpleBiDi_traverse(struct MU_HashTableSimpleBiDi *h, void(*callback)(char* key, int value))
{
	unsigned int i;
	struct MU_HashTable_Node *cur;
	int count = 0;

	if (!h)
		return count;

	for (i = 0; i < h->nodes->size; i++) {
		cur = h->nodes->htable[i];
		while (cur != NULL) {
			count++;
			callback((char *)cur->key, (int)cur->datum);
			cur = cur->next;
		}
	}

	return count;
}

// Return 0 for success, or error code(less than 0). You should manually delete the Key<->Value from the HashTable when error occurs
int MU_HashTableSimpleBiDi_insert(struct MU_HashTableSimpleBiDi *h, char* key, int value)
{
	int i1, i2;

	i1 = MU_HashTable_insert(h->nodes, (void *)key, (void *)value);
	if (i1 < 0)
		return i1;

	i2 = MU_HashTable_insert(h->reverseNodes, (void *)value, (void *)key);
	if (i2 < 0) {
		return i2;
	}

	return 0;
}

void *MU_HashTableSimpleBiDi_search(struct MU_HashTableSimpleBiDi *h, char* key)
{
	return MU_HashTable_search(h->nodes, key);
}

void *MU_HashTableSimpleBiDi_searchByValue(struct MU_HashTableSimpleBiDi *h, int value)
{
	return MU_HashTable_search(h->reverseNodes, (void *)value);
}

void *MU_HashTableSimpleBiDi_delete(struct MU_HashTableSimpleBiDi *h, char* key, int value)
{
	void *p1, *p2;
	
	p1 = MU_HashTable_delete(h->nodes, (void *)key, 1, 0);
	p2 = MU_HashTable_delete(h->reverseNodes, (void *)value, 0, 1);

	if (!p1 || !p2)
		return NULL;

	return h;
}
