#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../Headers/cryptotools.h"
#include "../Headers/prdecl.h"
#include "../Headers/list.h"
#include "../Headers/htable.h"
#include "../Headers/block.h"
#include "../Headers/tree.h"

#define SIZEV 2000
#define SIZEC 50
#define POW 3


int main(int argc, char **argv) {
  srand(time(NULL));


  // Data Generation
  generate_random_data(1000, 5);


  // Data Reading
  CellProtected *decl = read_protected("../Database/declarations.txt");
  CellKey *candidates = read_public_keys("../Database/candidates.txt");
  CellKey *voters = read_public_keys("../Database/keys.txt");


  // Submitting all Votes
  CellTree *tree = NULL, *tmp;
  CellProtected *pr = decl;
  CellKey *author = voters;

  char name[256];
  char name2[256];
  char n[32];
  char ext[8];

  int cpt = 1, i = 1;

  strcpy(ext, ".txt");


  while (pr != NULL) {
    submit_vote(pr->data);
    // Creating a block every ten votes
    if (cpt % 10 == 0) {
      // Names of the files
      strcpy(name, "../Blockchain/Block");
      sprintf(n, "%d", i++);
      strcat(name, n);
      strcat(name, ext);

      strcpy(name2, "Block");
      strcat(name2, n);
      strcat(name2, ext);

      create_block(tree, author->data, POW);
      add_block(POW, name2);


      // Adding the block to the tree
      if (tree == NULL) {
        tree = create_node(read_block(name));
      }

      else {
        tmp = tree;
        // As part of the same chain
        while (tmp->firstChild != NULL) {
          tmp = tmp->firstChild;
        }

        add_child(tmp, create_node(read_block(name)));
      }
    }

    cpt++;
    author = author->next;
    pr = pr->next;
  }


  // Reading and Printing the Tree
  CellTree *t = read_tree();
  print_tree("tree.txt", t);


  // Calculating the winner
  Key *winner = compute_winner_BT(tree, candidates, voters, SIZEC, SIZEV);
  char *str = key_to_str(winner);
  printf("Winner: %s\n", str);



  // Freeing the memory
  free(str);
  free(winner);
  free_tree(tree);
  free_tree(t);
  delete_list_protected(decl);
  delete_list_keys(candidates);
  delete_list_keys(voters);
}
