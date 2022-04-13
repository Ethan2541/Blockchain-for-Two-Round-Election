#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cryptotools.h"


/* Naive Primality */

int is_prime_naive(long p) {
  // Ensure that the number given is uneven
  assert(p % 2 == 1);

  // Base case
  if (p < 2) {
    return 0;
  }

  // Check every possible divider
  for (int i = 2; i < p; i++) {
    if (p % i == 0) {
      return 0;
    }
  }

  return 1;
}





/* Quick Modular Power */

long modpow_naive(long a, long m, long n) {
  // Ensure that we don't divide by 0
  assert(n != 0);

  long res = 1;

  // Case: n = 1
  if (n == 1) {
    return 0;
  }

  for (long i = 0; i < m; i++) {
    res = (a * res) % n;
  }

  return res;
}



long modpow(long a, long m, long n) {
  // Ensure that we don't divide by 0
  assert(n != 0);

  // Case: n = 1
  if (n == 1) {
    return 0;
  }

  // Case: m = 0
  if (m == 0) {
    return 1;
  }

  long tmp;

  // Case: m even
  if (m % 2 == 0) {
    tmp = modpow(a, m / 2, n);
    return (tmp * tmp) % n;
  }

  // Case: m uneven
  else {
    tmp = modpow(a, (m - 1) / 2, n);
    return (a * tmp * tmp) % n;
  }
}





/* Miller-Rabin Test */

int witness (long a, long b, long d, long p) {
  long x = modpow(a, d, p);

  if (x == 1) {
    return 0;
  }

  for (long i = 0; i < b ; i ++) {
    if (x == p - 1) {
      return 0;
    }

    x = modpow (x ,2 , p ) ;
  }

  return 1;
}



long rand_long (long low, long up) {
  return rand () % (up - low + 1) + low ;
}



int is_prime_miller (long p, int k) {
  if (p == 2) {
    return 1;
  }

  // Check that p is uneven and different of 1
  if (!(p & 1) || p <= 1) {
    return 0;
  }

  // We determine b and d
  long b = 0;
  long d = p - 1;

  // While d is even (check the last bit)
  while (!(d & 1)) {
    d = d / 2;
    b = b + 1;
  }

  // We generate k values for a, and test whether it is a witness :
  long a ;
  int i ;

  for (i = 0; i < k ; i++) {
    a = rand_long(2, p - 1) ;

    if (witness (a, b, d, p)) {
      return 0;
    }
  }

  return 1;
}



// Quick Exponentiation
long quickexp(long a, long b) {
  // Case: b = 0
  if (b == 0) {
    return 1;
  }

  long tmp;

  // Case: b even
  if (b % 2 == 0) {
    tmp = quickexp(a, b / 2);
    return tmp * tmp;
  }

  // Case: b uneven
  else {
    tmp = quickexp(a, (b - 1) / 2);
    return a * tmp * tmp;
  }
}



long random_prime_number(int low_size, int up_size, int k) {
  long min = quickexp(2, low_size);
  long max = quickexp(2, up_size + 1) - 1;
  long a;

  // Random numbers are tested until they pass the test
  do {
    a = rand_long(min, max);
  } while (is_prime_miller(a, k) == 0);

  return a;
}





/* RSA Encryption and Decryption */

long extended_gcd(long s, long t, long *u, long *v) {
  assert(u && v);

  if (t == 0) {
    *u = 1;
    *v = 0;
    return s;
  }

  long uPrim, vPrim;
  long gcd = extended_gcd(t, s % t, &uPrim, &vPrim);
  *u = vPrim;
  *v = uPrim - (s / t) * vPrim;

  return gcd;
}



void generate_key_values(long p, long q, long *n, long *s, long *u) {
  assert(n && s && u);

  long t = (p - 1) * (q - 1);

  // While GCD(s,t) != 1
  do {
    *s = rand_long(1, t);
  } while (extended_gcd(*s, t, u, n) != 1);

  // n is then initialized
  *n = p * q;
}



long *encrypt(char *str, long s, long n) {
  int l = strlen(str);

  // If the string is empty
  if (l == 0) {
    return NULL;
  }

  long *tab = (long *) malloc(l * sizeof(long));

  if (tab == NULL) {
    fprintf(stderr, "%s; %s; l.%d: long* Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  // The string is analyzed char by char
  for (int i = 0; i < l; i++) {
    // Each char is converted to a long and added to an array
    tab[i] = modpow((long) str[i], s, n);
  }

  return tab;
}



char *decrypt(long *crypted, int size, long u, long n) {
  char *str = (char *) malloc((size + 1) * sizeof(char));

  if (str == NULL) {
    fprintf(stderr, "%s; %s; l.%d: char* Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
  }

  // Decryption of every long of the array
  for (int i = 0; i < size; i++) {
    // Each long is then converted to the original char
    str[i] = (char) modpow(crypted[i], u, n);
  }

  // '\0' is finally added
  str[size] = '\0';
  return str;
}
