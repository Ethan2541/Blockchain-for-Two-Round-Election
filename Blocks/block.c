#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/prdecl.h"
#include "../Headers/list.h"
#include "../Headers/block.h"


/* Blocks Reading and Writing */

void write_block(char *filename, Block *b) {
  if (b == NULL) {
    return;
  }


  FILE *f = fopen(filename, "w");

  if (f == NULL) {
    fprintf(stderr, "%s; %s; l.%d: Can't open file: %s\n", __FILE__, __PRETTY_FUNCTION__, __LINE__, filename);
    exit(EXIT_FAILURE);
  }


  // Printing the key, hash, previous_hash and the proof of work
  char *str = key_to_str(b->author);
  fprintf(f, "%s %s %s %d\n", str, b->hash, b->previous_hash, b->nonce);
  free(str);

  // Browsing b->votes to print every declaration
  char *prstr;
  CellProtected *c = b->votes;
  while (c != NULL) {
    prstr = protected_to_str(c->data);
    fprintf(f, "%s\n", prstr);
    free(prstr);
    c = c->next;
  }

  fclose(f);
}



Block *read_block(char *filename) {
  FILE *f = fopen(filename, "r");

  if (f == NULL) {
    fprintf(stderr, "%s; %s; l.%d: Can't open file: %s\n", __FILE__, __PRETTY_FUNCTION__, __LINE__, filename);
    exit(EXIT_FAILURE);
  }

  Block *b = (Block *) malloc(sizeof(Block));

  if (b == NULL) {
    fprintf(stderr, "%s; %s; l.%d: Block Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }


  unsigned char *hash = (unsigned char *) malloc(256 * sizeof(unsigned char));
  unsigned char *previous_hash = (unsigned char *) malloc(256 * sizeof(unsigned char));

  if ((hash == NULL) || (previous_hash == NULL)) {
    fprintf(stderr, "%s; %s; l.%d: unsigned char* Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }


  char author[256];
  char buffer[256];
  char prstr[256];
  CellProtected *list = NULL;
  // Reading the first line of the file
  if (fgets(buffer, 256, f) != NULL) {
    if (sscanf(buffer, "%s %s %s %d", author, hash, previous_hash, &(b->nonce)) != 4) {
      fprintf(stderr, "%s; %s; l.%d: Invalid Block Format\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
      exit(EXIT_FAILURE);
    }
  }

  b->author = str_to_key(author);
  b->hash = hash;
  b->previous_hash = previous_hash;

  // Reading the other lines
  while (fgets(buffer, 256, f) != NULL) {
    if (sscanf(buffer, "%[^\n]", prstr) != 1) {
      fprintf(stderr, "%s; %s; l.%d: Invalid Block Format\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
      exit(EXIT_FAILURE);
    }

    // Creating and inserting the declarations following each line's data
    insert_cell_protected(&list, str_to_protected(prstr));
  }

  b->votes = list;
  fclose(f);
  return b;
}



void free_block(Block *b) {
  free(b->author);
  if (b->hash != NULL) {
    free(b->hash);
  }

  if (b->previous_hash != NULL) {
    free(b->previous_hash);
  }

  delete_list_protected(b->votes);
  free(b);
}





/* Valid Blocks Creation */

char *block_to_str(Block *block) {
  char *str = (char *) malloc(65536 * sizeof(char));

  if (str == NULL) {
    fprintf(stderr, "%s; %s; l.%d: char* Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  int cpt = 0;


  // Adding each character of the key
  char *author = key_to_str(block->author);

  for (int i = 0; i < strlen(author); i++) {
    str[cpt++] = author[i];
  }


  str[cpt++] = ' ';
  free(author);


  // Adding the hashed value of the previous block (if it exists)
  if (block->previous_hash != NULL) {
    for (int i = 0; i < strlen(block->previous_hash); i++) {
      str[cpt++] = (block->previous_hash)[i];
    }

    str[cpt++] = ' ';
  }


  // Adding each declaration to the string
  char *prstr;
  CellProtected *c = block->votes;

  // Browsing block->votes
  while (c != NULL) {
    prstr = protected_to_str(c->data);
    // Adding the characters of every declaration
    for (int i = 0; i < strlen(prstr); i++) {
      str[cpt++] = prstr[i];
    }

    free(prstr);
    str[cpt++] = ' ';
    c = c->next;
  }


  // We can finally end the string
  str[cpt] = '\0';
  return str;
}



unsigned char *hash_SHA256(char *s) {
  unsigned char *d = SHA256(s, strlen(s), 0);
  unsigned char tmp[8];


  // Creating the returned string
  unsigned char *str = (unsigned char *) malloc(256 * sizeof(unsigned char));

  if (str == NULL) {
    fprintf(stderr, "%s; %s; l.%d: unsigned char* Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  str[0] = '\0';


  // Adding every character of d to str with hexadecimal format
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    sprintf(tmp, "%02x", d[i]);
    strcat(str, tmp);
  }

  return str;
}



void compute_proof_of_work(Block *B, int d) {
  B->nonce = 0;

  if (B->hash == NULL) {
    char *str = block_to_str(B);
    B->hash = hash_SHA256(str);
    free(str);
  }


  unsigned char tmp[256];
  unsigned char nonce[128];
  unsigned char t[256];
  int cpt = 0, i = 0;


  // Creating the substring "hash"."nonce" and hashing the substring
  sprintf(nonce, "%d", B->nonce);
  strcpy(tmp, B->hash);
  strcat(tmp, nonce);
  unsigned char *u = hash_SHA256(tmp);


  // Scanning the hexadecimal hashed value in order to find d consecutive zeros
  while (cpt < d) {
    // Incrementing the counter if a zero is found
    if (u[i++] == '0') {
      cpt++;
    }

    // Else, the counter is reset, B->nonce is incremented and a new substring is created
    else {
      B->nonce++;
      i = 0;
      cpt = 0;
      free(u);
      sprintf(nonce, "%d", B->nonce);
      strcpy(tmp, B->hash);
      strcat(tmp, nonce);
      u = hash_SHA256(tmp);
    }
  }
  printf("Final substring: %d %s\n", B->nonce, u);
  free(u);
}



int verify_block(Block *b, int d) {
  unsigned char tmp[256];
  unsigned char nonce[128];
  int cpt = 0, i = 0;

  if (b->hash == NULL) {
    return 0;
  }

  // Creating the substring "hash"."nonce" and hashing the substring
  sprintf(nonce, "%d", b->nonce);
  strcpy(tmp, b->hash);
  strcat(tmp, nonce);
  unsigned char *u = hash_SHA256(tmp);


  // We have to check if u[0] = ... = u[d - 1] = 0
  while (cpt < d) {
    // If a zero is found, the counter rises by one
    if (u[i++] == '0') {
      cpt++;
    }

    // Else, the proof of work is invalid
    else {
      free(u);
      return 0;
    }
  }

  free(u);
  return 1;
}



void delete_block(Block *b) {
  if (b->hash != NULL) {
    free(b->hash);
  }

  if (b->previous_hash != NULL) {
    free(b->previous_hash);
  }


  // Freeing cells of b->votes without freeing their content
  CellProtected *c = b->votes;
  CellProtected *tmp;

  while (c != NULL) {
    tmp = c;
    c = c->next;
    free(tmp);
  }

  free(c);
  free(b);
}
