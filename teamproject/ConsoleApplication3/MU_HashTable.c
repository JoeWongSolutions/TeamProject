
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>

#include "MU_HashTable.h"


typedef struct
{
	char  *pairName;
	char  *targetName;
	char  *sourceName;
	long   distance;
	int    isBidirectional;
	int    isShortestPath;
} MyGraphviz;

/* SDBM Hash Function: use string as the Key, and use strcmp to compare */
static unsigned int MU_HashTable_SDBM_hash(int hashtablesize, char *str)
{
	unsigned int hash = 0;

	while (*str)
	{
		hash = (*str++) + (hash << 6) + (hash << 16) - hash;
	}

	return ((hash & 0x7FFFFFFF) % hashtablesize);
}


/* Index Hash Function: use index as the Key to get fast access speed */
unsigned int MU_HashTable_index_hash(int hashtablesize, void *index)
{
	unsigned int idx = (unsigned int)index;

	if (idx >= (unsigned int)hashtablesize - 1)
	{
		printf("Fatal error: size of hash table needs to be increased !");
		exit(-1);
	}

	return idx;
}

int MU_HashTable_index_hash_cmp(const void *s1, const void *s2)
{
	if (s1 == s2)
		return 0;
	else if (s1 < s2)
		return -1;
	else
		return 1;
}




struct MU_HashTable *MU_HashTable_create(unsigned int(*hash_value)(int hashtablesize, void *key),
	int(*keycmp)(void *key1, void *key2),
	unsigned int size)
{
	struct MU_HashTable *p;
	unsigned int i;

	p = calloc(1, sizeof(*p));
	if (p == NULL)
		return p;

	p->size = size;
	p->nel = 0;
	p->hash_value = hash_value ? hash_value : MU_HashTable_SDBM_hash;
	p->keycmp = keycmp ? keycmp : strcmp;
	p->htable = calloc(size, sizeof(*(p->htable)));
	if (p->htable == NULL) {
		free(p);
		return NULL;
	}

	for (i = 0; i < size; i++)
		p->htable[i] = NULL;

	return p;
}

int MU_HashTable_insert(struct MU_HashTable *h, void *key, void *value)
{
	unsigned int hvalue;
	struct MU_HashTable_Node *prev, *cur, *newnode;

	if (!h || !key || !value || h->nel == HASHTAB_MAX_NODES)
		return -EINVAL;

	hvalue = h->hash_value(h->size, key);
	prev = NULL;
	cur = h->htable[hvalue];
	while (cur && h->keycmp(key, cur->key) > 0) {
		prev = cur;
		cur = cur->next;
	}

	if (cur && (h->keycmp(key, cur->key) == 0))
		return -EEXIST;

	newnode = calloc(1, sizeof(*newnode));
	if (newnode == NULL)
		return -ENOMEM;

	newnode->key = key;
	newnode->datum = value;
	if (prev) {
		newnode->next = prev->next;
		prev->next = newnode;
	}
	else {
		newnode->next = h->htable[hvalue];
		h->htable[hvalue] = newnode;
	}

	h->nel++;

	return 0;
}

void *MU_HashTable_search(struct MU_HashTable *h, void *key)
{
	unsigned int hvalue;
	struct MU_HashTable_Node *cur;

	if (!h)
		return NULL;

	hvalue = h->hash_value(h->size, key);
	cur = h->htable[hvalue];
	while (cur != NULL && h->keycmp(key, cur->key) > 0)
		cur = cur->next;

	if (cur == NULL || (h->keycmp(key, cur->key) != 0))
		return NULL;

	return cur->datum;
}

void *MU_HashTable_delete(struct MU_HashTable *h, void *key, int freeKey, int freeValue)
{
	unsigned int hvalue;
	struct MU_HashTable_Node *cur;
	struct MU_HashTable_Node *prev = NULL;

	if (!h)
		return NULL;

	hvalue = h->hash_value(h->size, key);
	cur = h->htable[hvalue];
	while (cur != NULL && h->keycmp(key, cur->key) > 0)
	{
		prev = cur;
		cur = cur->next;
	}

	if (cur == NULL || (h->keycmp(key, cur->key) != 0))
		return NULL;

	if (prev)
		prev->next = cur->next;
	else
		h->htable[hvalue] = cur->next;

	if (freeValue && cur->datum) 
		free(cur->datum);
	if (freeKey) 
		free(cur->key);

	free(cur);

	return h;
}

void MU_HashTable_destroy(struct MU_HashTable *h, int freeKey, int freeValue)
{
	unsigned int i;
	struct MU_HashTable_Node *cur, *temp;

	if (!h)
		return;

	for (i = 0; i < h->size; i++) {
		cur = h->htable[i];
		while (cur != NULL) {
			temp = cur;
			cur = cur->next;

			if (freeValue && temp->datum){
                if((freeValue == 2)){
              MyGraphviz * gv= (MyGraphviz *)temp->datum;
              if(gv->sourceName)
              free(gv->sourceName);

              if(gv->targetName)
              free(gv->targetName);
                }
				free(temp->datum);
            }
			if (freeKey)
				free(temp->key);

			free(temp);
		}
		h->htable[i] = NULL;
	}

	free(h->htable);
	h->htable = NULL;

	free(h);
}


int MU_HashTable_traverse(struct MU_HashTable *h, void(*callback)(void* key, void* value))
{
	unsigned int i;
	struct MU_HashTable_Node *cur;
	int count = 0;

	if (!h)
		return count;

	for (i = 0; i < h->size; i++) {
		cur = h->htable[i];
		while (cur != NULL) {
			count++;
			callback(cur->key, cur->datum);
			cur = cur->next;
		}
	}

	return count;
}

