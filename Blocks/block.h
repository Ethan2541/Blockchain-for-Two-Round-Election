#ifndef BLOCK_H
#define BLOCK_H


/* Structure */

typedef struct _block {
  Key *author;
  CellProtected *votes;
  unsigned char *hash;
  unsigned char *previous_hash;
  int nonce;
} Block;


/* Blocks Reading and Writing */

void print_block(char *filename, Block *b);
Block *read_block(char *filename);
void free_block(Block *b);


/* Valid Blocks Creation */

char *block_to_str(Block *block);
unsigned char *hash_SHA256(char *s);
void compute_proof_of_work(Block *B, int d);
int verify_block(Block *b, int d);
void delete_block(Block *b);

#endif
