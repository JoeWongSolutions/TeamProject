#ifndef MU_HASHTABLEBIDI_H
#define MU_HASHTABLEBIDI_H 

#include "MU_HashTable.h"

/*
 * Bi-Directional HashTable, typeof(Key) is string and typeof(Value) is int. Key -> Value and Value -> Key pairs are stored at the same time
 * Note: 
 * 1) all the keys should be unique, all the values should also be unique.
 * 2) Value is an integer instead of a pointer and thus free(Value) should NOT be called.
 */

struct MU_HashTableSimpleBiDi {
	struct MU_HashTable *nodes;
	struct MU_HashTable *reverseNodes;
};

struct MU_HashTableSimpleBiDi *MU_HashTableSimpleBiDi_create(unsigned int size);
void  MU_HashTableSimpleBiDi_destroy(struct MU_HashTableSimpleBiDi *h);

int   MU_HashTableSimpleBiDi_traverse(struct MU_HashTableSimpleBiDi *h, void(*callback)(char* key, int value));

int   MU_HashTableSimpleBiDi_insert(struct MU_HashTableSimpleBiDi *h, char* key, int value);
void *MU_HashTableSimpleBiDi_delete(struct MU_HashTableSimpleBiDi *h, char* key, int value);

void *MU_HashTableSimpleBiDi_search(struct MU_HashTableSimpleBiDi *h, char* key);
void *MU_HashTableSimpleBiDi_searchByValue(struct MU_HashTableSimpleBiDi *h, int value);

#endif
