#ifndef TREE_H
#define TREE_H


/* Structure */

typedef struct block_tree_cell {
    Block *block;
    struct block_tree_cell *father;
    struct block_tree_cell *firstChild;
    struct block_tree_cell *nextBro;
    int height;
} CellTree;


/* Tree Manipulation */

CellTree *create_node(Block *b);
int update_height(CellTree *father, CellTree *child);
void add_child(CellTree *father, CellTree *child);
void print_tree_rec(FILE *f, CellTree *node);
void print_tree(char *filename, CellTree *tree);
void delete_node(CellTree *node);
void delete_tree(CellTree *tree);


/* Find Last Block */

CellTree *highest_child(CellTree *cell);
CellTree *last_child(CellTree *tree);


/* Extract Protected */

CellProtected *longest_chain_protected(CellTree *tree);


/* Votes and Valid Blocks Creation */

void submit_vote(Protected *p);
void create_block(CellTree *tree, Key *author, int d);
void add_block(int d, char *name);


/* Tree Reading and Winner Calculation */

CellTree *read_tree();
Key *computre_winner_BT(CellTree *tree, CellKey *candidates, CellKey *voters, int sizeC, int sizeV);


#endif
