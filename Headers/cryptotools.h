#ifndef CRYPTOTOOLS_H
#define CRYPTOTOOLS_H

/* Naive Primality */

int is_prime_naive(long p);


/* ModPow */

long modpow_naive(long a, long m, long n);
long modpow(long a, long m, long n);


/* Miller-Rabin */

int witness(long a, long b, long d, long p);
long rand_long(long low, long up);
int is_prime_miller(long p, int k);
long quickexp(long a, long b);
long random_prime_number(int low_size, int up_size, int k);


/* RSA */

long extended_gcd(long s, long t, long *u, long *v);
void generate_key_values(long p, long q, long *n, long *s, long *u);
long *encrypt(char *chaine, long s, long n);
char *decrypt(long *crypted, int size, long u, long n);

#endif
