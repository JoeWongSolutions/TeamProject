
#ifndef MU_HASHTABLE_H
#define MU_HASHTABLE_H 

#define HASHTAB_MAX_NODES 10000
#define EINVAL 1
#define EEXIST 2
#define ENOMEM 3

struct MU_HashTable_Node {
	void  *key;
	void  *datum;
	struct MU_HashTable_Node *next;
};

struct MU_HashTable {
	struct MU_HashTable_Node **htable;				         /* hash table */
	unsigned int size;										 /* number of slots in hash table */
	unsigned int nel;										 /* number of elements in hash table */
	unsigned int(*hash_value)(int hashtablesize, void *key); /* hash function */
	int(*keycmp)(void *key1, void *key2);			         /* key comparison function */
};

struct MU_HashTable *MU_HashTable_create(unsigned int(*hash_value)(int hashtablesize, void *key),
	int(*keycmp)(void *key1, void *key2),
	unsigned int size);
void  MU_HashTable_destroy(struct MU_HashTable *h, int freeKey, int freeValue);

int   MU_HashTable_traverse(struct MU_HashTable *h, void(*callback)(void* key, void* value));

int   MU_HashTable_insert(struct MU_HashTable *h, void *key, void *value);
void *MU_HashTable_delete(struct MU_HashTable *h, void *key, int freeKey, int freeValue);

void *MU_HashTable_search(struct MU_HashTable *h, void *key);


/* Index Hash Function: use index as the Key to get fast access speed */
unsigned int MU_HashTable_index_hash(int hashtablesize, void *index);
int MU_HashTable_index_hash_cmp(const void *s1, const void *s2);

#endif