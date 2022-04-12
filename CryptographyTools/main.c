#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cryptotools.h"

#define A 10
#define N 10
#define MIN 0
#define MAX 10000
#define STEP 100


void print_long_vector (long *result, int size) {
  printf("Vector: [");
  for (int i = 0; i < size ; i++) {
    printf("%lx\t", result[i]);
  }
  printf("]\n");
}



int main(int argc, char **argv) {
  srand(time(NULL));
  clock_t tI, tF;
  double t1, t2;


  /* is_prime_naive test */

  int res = 0;
  t1 = 0;

  // Check if uneven numbers are prime and print them if it is the case
  for (long prime = 3; t1 <= 0.002; prime += 2) {
    tI = clock();
    res = is_prime_naive(prime);
    tF = clock();
    t1 = ((double) (tF - tI)) / CLOCKS_PER_SEC;

    if (res == 1) {
      printf("%ld %d %f\n", prime, res, t1);
    }
  }





  /* modpow tests */

  FILE *fw1;
  fw1 = fopen("ModPowOutput.txt", "w");

  if (fw1 == NULL) {
    fprintf(stderr, "Can't open file: ModPowOutput.txt\n");
    exit(EXIT_FAILURE);
  }

  // Comparison of modpow functions
  for (long i = MIN; i <= MAX; i += STEP) {
    // mowpow_naive
    tI = clock();
    modpow_naive(A, i, N);
    tF = clock();
    t1 = ((double) (tF - tI)) / CLOCKS_PER_SEC;

    // modpow
    tI = clock();
    modpow(A, i, N);
    tF = clock();
    t2 = ((double) (tF - tI)) / CLOCKS_PER_SEC;

    // Output
    fprintf(fw1, "%ld %f %f\n", i, t1, t2);
  }





  /* RSA Cryptography */

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


  // Print the keys in hexadecimal
  printf("Clé publique = (%lx, %lx)\n", s, n);
  printf("Clé privée = (%lx, %lx)\n", u, n);


  // Encryption of an input
  char mess[256];
  printf("\nPlease enter a message : \n");
  fgets(mess, 256, stdin);
  int len = strlen(mess);

  // Get rid of the unneccesary '\n' character
  if (strcmp(mess, "") != 0) {
    mess[len-1] = '\0';
  }

  long *crypted = encrypt(mess, s, n);

  // Show the crypted message
  printf("Initial message: %s\n", mess);
  printf("Encoded representation : \n");
  print_long_vector(crypted, len-1);


  // Decryption
  char *decoded = decrypt(crypted, len-1, u, n);
  printf("Decoded: %s\n", decoded);


  // Free the long and char* arrays
  free(crypted);
  free(decoded);
}
