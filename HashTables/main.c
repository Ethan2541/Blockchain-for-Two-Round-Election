#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../Headers/prdecl.h"
#include "../Headers/list.h"
#include "../Headers/htable.h"

#define SIZEV 1000
#define SIZEC 200

int main(int argc, char **argv) {
  srand(time(NULL));


  // Data Generation
  generate_random_data(500, 20);


  // Initialization
  CellKey *voters = read_public_keys("../Database/keys.txt");
  CellKey *candidates = read_public_keys("../Database/candidates.txt");
  CellProtected *decl = read_protected("../Database/declarations.txt");

  delete_false_protected(&decl);


  // Counting the number of collisions
  HashTable *t = create_hashtable(voters, SIZEV);
  CellKey *c = voters;
  int tab[SIZEV];
  int pos;
  int cpt = 0;

  for (int i = 0; i < SIZEV; i++) {
    tab[i] = 0;
  }

  while (c != NULL) {
    pos = find_position(t, c->data);
    if (tab[pos] != 0) {
      cpt++;
    }
    tab[pos]++;
    c = c->next;
  }

  printf("Collisions: %d\n", cpt);
  delete_hashtable(t);


  // Testing compute_winner
  Key *key = compute_winner(decl, candidates, voters, SIZEC, SIZEV);
  char *str = key_to_str(key);
  printf("%s\n", str);


  free(key);
  free(str);
  delete_list_keys(voters);
  delete_list_keys(candidates);
  delete_list_protected(decl);
}
