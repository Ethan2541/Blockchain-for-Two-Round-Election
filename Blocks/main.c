#include <openssl/sha.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "../CryptographyTools/cryptotools.h"
#include "../ProtectedDeclarations/prdecl.h"
#include "../Lists/list.h"
#include "block.h"


int main(int argc, char **argv) {
  srand(time(NULL));
  clock_t tI, tF;
  double t1 = 0;


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
  b->hash = NULL;
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
  FILE *f = fopen("PoWOutput.txt", "w");

  if (f == NULL) {
    fprintf(stderr, "%s; %s; l.%d: Can't open file: PoWOutput.txt\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  // Calculating the time spent for possible values of pow, while it is under a second
  for (int pow = 0; t1 < 1.0; pow++) {
    tI = clock();
    compute_proof_of_work(b, pow);
    tF = clock();

    // Time spent
    t1 = ((double) (tF - tI)) / CLOCKS_PER_SEC;
    fprintf(f, "%d %f\n", pow, t1);

    printf("Verify: %d %d\n\n", b->nonce, verify_block(b, pow));
  }

  fclose(f);




  // Testing Block Functions
  print_block("block.txt", b);
  Block *b2 = read_block("block.txt");

  char *str = block_to_str(b);
  char *str2 = block_to_str(b2);

  printf("Block 1 : %s\n\n\n\n", str);
  printf("Block 2 : %s\n", str2);


  // Free the memory used
  free(str);
  free(str2);
  free_block(b);
  free_block(b2);
}
