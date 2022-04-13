#ifndef HTABLE_H
#define HTABLE_H


/* Structures */

typedef struct _hashcell {
  Key *key;
  int val;
} HashCell;

typedef struct _hashtable {
  HashCell **tab;
  int size;
} HashTable;



/* Functions */

HashCell *create_hashcell(Key *key);
int hash_function(Key *key, int size);
int find_position(HashTable *t, Key *key);
HashTable *create_hashtable(CellKey *keys, int size);
void delete_hashtable(HashTable *t);
Key *compute_winner(CellProtected *decl, CellKey *candidates, CellKey *voters, int sizeC, int sizeV);


#endif
