#include <assert.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/prdecl.h"
#include "../Headers/list.h"
#include "../Headers/htable.h"
#include "../Headers/block.h"
#include "../Headers/tree.h"


/* Tree Manipulation */

CellTree *create_node(Block *b){
  CellTree *node = (CellTree *) malloc(sizeof(CellTree));

  if(node == NULL){
      fprintf(stderr, "%s; %s; l.%d: CellTree Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
      exit(EXIT_FAILURE);
  }

  node->block = b;
  node->father = NULL;
  node->firstChild = NULL;
  node->nextBro = NULL;
  node->height = 0;

  return node;
}



int update_height(CellTree* father, CellTree* child){
  // If the child has a greater height than its father
  if (father->height <= child->height) {
    father->height = child->height + 1;
    return 1;
  }

  // Else, the father still has a greater height than its child
  return 0;
}



void add_child(CellTree* father, CellTree* child){
  // The father should not be an empty tree
  assert(father);

  // The father has no child
  if (father->firstChild == NULL) {
    father->firstChild = child;
  }

  else {
    CellTree *tmp = father->firstChild;
    // Looking for the first child having no brother
    while (tmp->nextBro != NULL) {
      tmp = tmp->nextBro;
    }

    tmp->nextBro = child;
  }

  child->father = father;


  // Updating the height of its ancestors (one after another)
  while (father != NULL) {
    update_height(father, child);
    child = father;
    father = father->father;
  }
}



void print_tree_rec(FILE *f, CellTree *node) {
  // Case: Empty Tree
  if (node == NULL) {
    return;
  }

  // Print the node, then the child, and finally the brother
  fprintf(f, "%d %s\n", node->height, (node->block)->hash);
  print_tree_rec(f, node->firstChild);
  print_tree_rec(f, node->nextBro);
}



void print_tree(char *filename, CellTree *tree) {
  FILE *f = fopen(filename, "w");

  if (f == NULL) {
    fprintf(stderr, "%s; %s; l.%d: Can't open file: %s\n", __FILE__, __PRETTY_FUNCTION__, __LINE__, filename);
    exit(EXIT_FAILURE);
  }

  // Recursive printing
  print_tree_rec(f, tree);
  fclose(f);
}



void delete_node(CellTree *node) {
  // Deleting the block (without its author's key and votes' declarations)
  delete_block(node->block);
  free(node);
}



void delete_tree(CellTree *tree) {
  // Unnecessary to free an empty tree
  if (tree != NULL) {
    // Deleting the brothers and the sons, and finally the node
    delete_tree(tree->firstChild);
    delete_tree(tree->nextBro);

    // Deleting the node when deprived of any bond
    delete_node(tree);
  }
}



void free_node(CellTree *node) {
  // Deleting the block (without its author's key and votes' declarations)
  free_block(node->block);
  free(node);
}



void free_tree(CellTree *tree) {
  // Unnecessary to free an empty tree
  if (tree != NULL) {
    // Deleting the brothers and the sons, and finally the node
    free_tree(tree->firstChild);
    free_tree(tree->nextBro);

    // Deleting the node when deprived of any bond
    free_node(tree);
  }
}





/* Find Last Block */

CellTree *highest_child(CellTree *cell) {
  // Empty Tree or Leaf
  if ((cell == NULL) || (cell->firstChild == NULL)){
    return NULL;
  }


  CellTree *highestChild = cell->firstChild;
  CellTree *brother = highestChild->nextBro;

  // Looking for a brother with a greater height
  while (brother != NULL) {
    if (brother->height > highestChild->height) {
      highestChild = brother;
    }

    brother = brother->nextBro;
  }

  return highestChild;
}



CellTree *last_node(CellTree *tree) {
  // If the tree is empty
  if (tree == NULL) {
    return NULL;
  }

  // Considering a leaf, it has to be the last child
  if (tree->firstChild == NULL) {
    return tree;
  }


  // Else, the last child can be found in tree's highest child's offspring
  CellTree *highestChild = highest_child(tree);

  return last_node(highestChild);
}





/* Extract Protected */

CellProtected *longest_chain_protected(CellTree *tree) {
  // Empty Tree
  if (tree == NULL) {
    return NULL;
  }


  // Adding the first node's list
  CellProtected *list = NULL;
  merge_list_protected(&list, &((tree->block)->votes));


  // Adding the sons' lists
  CellTree *highestChild = highest_child(tree);
  while (highestChild != NULL) {
    merge_list_protected(&list, &((highestChild->block)->votes));
    highestChild = highest_child(highestChild);
  }

  return list;
}





/* Votes and Valid Blocks Creation */

void submit_vote(Protected *p) {
  // Opening the file in "append" mode
  FILE *f = fopen("../Blockchain/Pending_votes.txt", "a");

  if (f == NULL) {
    fprintf(stderr, "%s; %s; l.%d: Can't open file: ../Blockchain/Pending_votes.txt\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  // Printing the protected in Pending_votes.txt
  char *str = protected_to_str(p);
  fprintf(f, "%s\n", str);
  free(str);

  fclose(f);
}



void create_block(CellTree *tree, Key *author, int d) {
  Block *b = (Block *) malloc(sizeof(Block));

  if (b == NULL) {
    fprintf(stderr, "%s; %s; l.%d: Block Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  // Initializing the author's key and the CellProtected with "Pending_votes.txt"
  CellProtected *pr = read_protected("../Blockchain/Pending_votes.txt");
  Key *k = create_key();
  init_key(k, author->val, author->n);
  b->author = k;
  b->votes = pr;

  // If the tree isn't empty
  if (tree != NULL) {
    CellTree *lastNode = last_node(tree);

    // Linking the current block with the last block of the longest chain
    b->previous_hash = strdup((lastNode->block)->hash);
  }

  // Else, the previous hash value is NULL
  else {
    b->previous_hash = NULL;
  }


  // Hash and Nonce fields initialization
  b->hash = NULL;
  compute_proof_of_work(b, d);


  // Removing "Pending_votes.txt"
  remove("../Blockchain/Pending_votes.txt");

  // Writing the block in "Pending_block.txt"
  write_block("../Blockchain/Pending_block.txt", b);

  // Completely deleting the block
  free_block(b);
}



void add_block(int d, char *name) {
  // Reading the block
  Block *b = read_block("../Blockchain/Pending_block.txt");


  // If the block is valid
  if (verify_block(b, d) != 0) {
    // Writing the block in "../Blockchain/name"
    char str[256];
    strcpy(str, "../Blockchain/");
    strcat(str, name);
    write_block(str, b);
  }

  free_block(b);


  // Removing "Pending_block.txt"
  remove("../Blockchain/Pending_block.txt");
}





/* Tree Reading and Winner Calculation */

CellTree *read_tree() {
  DIR *rep = opendir("../Blockchain/");
  int cpt = 0, i = 0;

  if (rep != NULL) {
    struct dirent *dir;

    // Browsing the whole repository to find out the number of files
    while (dir = readdir(rep)) {
      if ((strcmp(dir->d_name, ".") != 0) && (strcmp(dir->d_name, "..")) && (strcmp(dir->d_name, ".gitkeep") != 0)) {
        // Excluding the "Pending_block.txt and "Pending_votes.txt" files
        if ((strcmp(dir->d_name, "Pending_block.txt") != 0) && (strcmp(dir->d_name, "Pending_votes.txt") != 0)) {
          cpt++;
        }
      }
    }


    // Rewinding the stream
    rewinddir(rep);


    // Preparing the CellTree Array and temporary variables
    CellTree **T = (CellTree **) malloc(cpt * sizeof(CellTree *));

    if (T == NULL) {
      fprintf(stderr, "%s; %s; l.%d: CellTree Array Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
      exit(EXIT_FAILURE);
    }

    Block *b;
    CellTree *t;
    int index = 0;
    char name[256];


    // Browsing the whole repository to add the nodes to the CellTree Array
    while (dir = readdir(rep)) {
      if ((strcmp(dir->d_name, ".") != 0) && (strcmp(dir->d_name, "..")) && (strcmp(dir->d_name, ".gitkeep") != 0)) {
        // Excluding the "Pending_block.txt and "Pending_votes.txt" files
        if ((strcmp(dir->d_name, "Pending_block.txt") != 0) && (strcmp(dir->d_name, "Pending_votes.txt") != 0)) {
          strcpy(name, "../Blockchain/");
          strcat(name, dir->d_name);
          b = read_block(name);
          t = create_node(b);
          T[index++] = t;
        }
      }
    }


    // Analyzing the array T to link parents and children
    for (int i = 0; i < cpt; i++) {
      for (int j = 0; j < cpt; j++) {
        // If T[j] is a child of T[i]
        if ((((T[j])->block)->previous_hash != NULL) && (strcmp(((T[j])->block)->previous_hash, ((T[i])->block)->hash) == 0)) {
          // Adding it as its child
          add_child(T[i], T[j]);
        }
      }
    }


    // Finding the root of the tree
    CellTree *tmp;
    for (int i = 0; i < cpt; i++) {
      if ((T[i])->father == NULL) {
        tmp = T[i];
      }
    }

    closedir(rep);

    free(T);
    return tmp;
  }
}



Key *compute_winner_BT(CellTree *tree, CellKey *candidates, CellKey *voters, int sizeC, int sizeV) {
  // Extracting the protected declarations
  CellProtected *decl = longest_chain_protected(tree);

  // Deleting the false declarations
  delete_false_protected(&decl);

  // Calculating the winner
  Key * winner = compute_winner(decl, candidates, voters, sizeC, sizeV);

  // Freeing the memory used
  delete_list_protected(decl);

  return winner;
}
