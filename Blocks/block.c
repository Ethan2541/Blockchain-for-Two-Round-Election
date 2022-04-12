#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../CryptographyTools/cryptotools.h"
#include "../ProtectedDeclarations/prdecl.h"
#include "../Lists/list.h"
#include "block.h"


/* Blocks Reading and Writing */

void print_block(char *filename, Block *b) {
  if (b == NULL) {
    return;
  }


  FILE *f = fopen(filename, "w");

  if (f == NULL) {
    fprintf(stderr, "Can't open file: %s\n", filename);
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
    fprintf(stderr, "Can't open file: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  Block *b = (Block *) malloc(sizeof(Block));

  if (b == NULL) {
    fprintf(stderr, "Block Allocation Error\n");
    exit(EXIT_FAILURE);
  }


  unsigned char *hash = (unsigned char *) malloc(256 * sizeof(unsigned char));
  unsigned char *previous_hash = (unsigned char *) malloc(256 * sizeof(unsigned char));

  if ((hash == NULL) || (previous_hash == NULL)) {
    fprintf(stderr, "unsigned char* Allocation Error\n");
    exit(EXIT_FAILURE);
  }


  char author[256];
  char buffer[256];
  char prstr[256];
  CellProtected *list = NULL;
  // Reading the first line of the file
  if (fgets(buffer, 256, f) != NULL) {
    if (sscanf(buffer, "%s %s %s %d", author, hash, previous_hash, &(b->nonce)) != 4) {
      fprintf(stderr, "Invalid Block Format\n");
      exit(EXIT_FAILURE);
    }
  }

  b->author = str_to_key(author);
  b->hash = hash;
  b->previous_hash = previous_hash;

  // Reading the other lines
  while (fgets(buffer, 256, f) != NULL) {
    if (sscanf(buffer, "%[^\n]", prstr) != 1) {
      fprintf(stderr, "Invalid Block Format\n");
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
    fprintf(stderr, "char* Allocation Error\n");
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
  return d;
}



void compute_proof_of_work(Block *B, int d) {
  // The proof of work is the index of the first zero of the requested series
  B->nonce = 0;
  char *str = block_to_str(B);
  B->hash = strdup(hash_SHA256(str));

  int indice = B->nonce;
  int cpt = 0;


  // Scanning the hexadecimal hashed value in order to find 4d consecutive zeros
  while (cpt < 4*d) {
    if ((B->hash)[indice] == '\0') {
      free(str);
      return;
    }

    // Incrementing the counter if a zero is found
    if ((B->hash)[indice++] == '0') {
      cpt++;
    }

    // Else, the counter is reset, and "cpt" is added to B->nonce to keep browsing the hashed value
    else {
      B->nonce += cpt;
      cpt = 0;
    }
  }

  free(str);
}



int verify_block(Block *b, int d) {
  int cpt = 0;
  int indice = b->nonce;

  if (b->hash == NULL) {
    return 0;
  }


  // We have to check if u[B->nonce] = ... = u[B->nonce + 4*d - 1] = 0
  while (cpt < 4*d) {
    if ((b->hash)[indice] == '\0') {
      return 0;
    }

    // If a zero is found, the counter rises by one
    if ((b->hash)[indice++] == '0') {
      cpt++;
    }

    // If not, the proof of work is invalid
    else {
      return 0;
    }
  }

  return 1;
}
