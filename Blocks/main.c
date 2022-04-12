#include <openssl/sha.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../CryptographyTools/cryptotools.h"
#include "../ProtectedDeclarations/prdecl.h"
#include "../Lists/list.h"
#include "block.h"

#define POW 1


int main(int argc, char **argv) {
  // Testing SHA256
  const char *a = "Rosette code";
  unsigned char *d = SHA256(a, strlen(a), 0);

  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    printf("%02x", d[i]);
  }

  putchar('\n');


  // Block Creation
  Block *b = (Block *) malloc(sizeof(Block));
  b->author = create_key();
  b->previous_hash = NULL;
  b->votes = read_protected("../ProtectedDeclarations/declarations.txt");

  // Key Generation
  long p = random_prime_number(3, 7, 5000);
  long q = random_prime_number(3, 7, 5000);

  // Ensure that both primes are different
  while (p == q) {
    q = random_prime_number (3, 7, 5000);
  }

  long n, s, u;
  generate_key_values(p, q, &n, &s, &u);

  // Ensure that the keys are positive
  if (u < 0) {
    long t = (p - 1) * (q - 1) ;
    u = u + t ; // s * u mod t = 1
  }

  init_key(b->author, s, n);

  // Testing compute_proof_of_work and verify_block
  compute_proof_of_work(b, POW);
  printf("%d\n", verify_block(b, POW));


  // Testing Block Functions
  print_block("blocks.txt", b);
  Block *b2 = read_block("blocks.txt");

  char *str = block_to_str(b);
  char *str2 = block_to_str(b2);

  printf("Block 1 : %s\n", str);
  printf("Block 2 : %s\n", str2);


  // Free the memory used
  free(str);
  free(str2);
  free_block(b);
  free_block(b2);
}
