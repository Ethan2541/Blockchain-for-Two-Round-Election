#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../Headers/prdecl.h"
#include "../Headers/list.h"
#include "../Headers/htable.h"


/* Functions to interact with the hashtable */

HashCell *create_hashcell(Key *key) {
  HashCell *hc = (HashCell *) malloc(sizeof(HashCell));

  if (hc == NULL) {
    fprintf(stderr, "%s; %s; l.%d: HashCell Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  hc->key = key;
  hc->val = 0;

  return hc;
}



int hash_function(Key *key, int size) {
  // Choosing the key
  long cle = key->n;
  double A = (sqrt(5.) - 1) / 2;
  // Multiplicative hashing
  int h = (int) (floor(size * (cle * A - floor(cle * A))));
  return h;
}



int find_position(HashTable *t, Key *key) {
  int n = t->size;
  int h = hash_function(key, n);
  int i = 0;
  int indice = (h + i) % n;

  // Linear Probing
  while ((t->tab)[indice] != NULL) {
    // If not empty: look for the next possible place
    if (((((t->tab)[indice])->key)->val == key->val) && ((((t->tab)[indice])->key)->n == key->n)) {
      return indice;
    }
    i += 1;
    indice = (h + i) % n;
  }

  return indice;
}



HashTable *create_hashtable(CellKey *keys, int size) {
  HashTable *ht = (HashTable *) malloc(sizeof(HashTable));

  if (ht == NULL) {
    fprintf(stderr, "%s; %s; l.%d: HashTable Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  ht->size = size;
  ht->tab = (HashCell **) malloc(size * sizeof(HashCell));

  // Allocating the array of HashCells
  if ((ht->tab) == NULL) {
    free(ht);
    fprintf(stderr, "%s; %s; l.%d: HashTable->Tab Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  // Initializing the array
  for (int i = 0; i < size; i++) {
    (ht->tab)[i] = NULL;
  }


  int pos;
  // Filling the array with cells and their keys
  while (keys != NULL) {
    pos = find_position(ht, keys->data);
    (ht->tab)[pos] = create_hashcell(keys->data);
    keys = keys->next;
  }

  return ht;
}



void delete_hashtable(HashTable *t) {
  // Deleting the hashcells without freeing the keys to prevent SegFault while manipulating the lists used during the creation
  for (int i = 0; i < t->size; i++) {
    if ((t->tab)[i] != NULL) {
      free((t->tab)[i]);
    }
  }

  // Free the memory used by the array
  free(t->tab);
  free(t);
}





Key *compute_winner(CellProtected *decl, CellKey *candidates, CellKey *voters, int sizeC, int sizeV) {
  // decl is only composed of valid declarations
  // Creating the HashTables related to the citizens' and candidates' keys
  HashTable *hv = create_hashtable(voters, sizeV);
  HashTable *hc = create_hashtable(candidates, sizeC);

  Key *keyV, *keyC;
  int posV, posC;

  // Browsing the declarations
  while (decl != NULL) {
    keyV = (decl->data)->pKey;
    posV = find_position(hv, keyV);

    // Checking if the citizen exits and if he has already voted
    if (((hv->tab)[posV] != NULL) && (((hv->tab)[posV])->val == 0)) {
      keyC = str_to_key((decl->data)->mess);
      posC = find_position(hc, keyC);

      // Checking if the candidate actually exists
      if ((hc->tab)[posC] != NULL) {
        // Adding a vote and ensuring the citizen won't vote again
        ((hc->tab)[posC])->val += 1;
        ((hv->tab)[posV])->val = 1;
      }

      free(keyC);
    }

    decl = decl->next;
  }


  // Calculating the winner based on the number of votes
  int score = 0;
  int position;
  Key *keyTmp;
  Key *keyWinner = create_key();

  while (candidates != NULL) {
    // We calculate the position of the candidate in the HashTable
    position = find_position(hc, candidates->data);

    // If the candidate has more votes than the one currently stored:
    if (((hc->tab)[position])->val > score) {
      score = ((hc->tab)[position])->val;
      keyTmp = ((hc->tab)[position])->key;
      init_key(keyWinner, keyTmp->val, keyTmp->n);
    }

    candidates = candidates->next;
  }

  delete_hashtable(hv);
  delete_hashtable(hc);

  return keyWinner;
}
