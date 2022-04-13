#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/prdecl.h"
#include "../Headers/list.h"
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
  if (father->firstChild == NULL) {
    father->firstChild = child;
  }

  else {
    CellTree *tmp = father->firstChild;

    while (tmp->nextBro != NULL) {
      tmp = tmp->nextBro;
    }

    tmp->nextBro = child;
  }

  while (father != NULL) {
    update_height(father, child);
    father = father->father;
    child = father;
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
  CellTree *father = node->father, *child = node->firstChild, *tmp;

  // If the first child is deleted
  if (father->firstChild == node) {
    father->firstChild = child;
  }

  else {
    tmp = father->firstChild;
    // Adding the brothers of node's child as its father's sons
    while (tmp->nextBro != node) {
      tmp = tmp->nextBro;
    }

    tmp->nextBro = child;


    // Keeping the other brothers of node
    while (tmp->nextBro != NULL) {
      tmp = tmp->nextBro;
    }

    tmp->nextBro = node->nextBro;
  }

  // Deleting the block (without its author's key and votes' declarations)
  delete_block(node->block);
  free(node);
}



void delete_tree(CellTree *tree) {
  // Unnecessary to free an empty tree
  if (tree == NULL) {
    return;
  }

  // Deleting the brothers and the sons, and finally the node
  delete_tree(tree->firstChild);
  delete_tree(tree->nextBro);
  delete_node(tree);
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



CellTree *last_child(CellTree *tree) {
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

  return last_child(highestChild);
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
