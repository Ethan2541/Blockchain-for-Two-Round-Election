#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../Headers/cryptotools.h"
#include "../Headers/prdecl.h"


void print_long_vector (long *result, int size) {
  printf("Vector: [");
  for (int i = 0; i < size ; i++) {
    printf("%lx\t", result[i]);
  }
  printf("]\n");
}


int main(int argc, char **argv) {
  srand(time(NULL));


  // Testing Init Keys
  Key *pKey = create_key();
  Key *sKey = create_key();

  init_pair_keys(pKey, sKey, 3, 7);
  printf("pKey: %lx, %lx\n", pKey->val, pKey->n);
  printf("sKey: %lx, %lx\n", sKey->val, sKey->n);


  // Testing Key Serialization
  char *tmpstr = key_to_str(pKey);
  printf("key to str: %s\n", tmpstr);
  Key *k = str_to_key(tmpstr);
  printf("str to key: %lx, %lx\n", k->val, k->n);

  free(tmpstr);
  free(k);


  // Testing signature
  // Candidate keys
  Key *pKeyC = create_key();
  Key *sKeyC = create_key();
  init_pair_keys(pKeyC, sKeyC, 3, 7);


  // Declaration
  char *mess = key_to_str(pKeyC);
  char *kstr = key_to_str(pKey);
  printf("%s votes for %s\n", kstr, mess);

  free(kstr);

  Signature *sgn = sign(mess, sKey);
  printf("signature: ");
  print_long_vector(sgn->content, sgn->size);

  tmpstr = signature_to_str(sgn);
  printf("signature to str: %s\n", tmpstr);

  free_signature(sgn);

  sgn = str_to_signature(tmpstr);
  printf("str to signature: ");
  print_long_vector(sgn->content, sgn->size);

  free(tmpstr);


  // Testing protected
  Protected *pr = init_protected(pKey, mess, sgn);
  free(mess);


  // Verification
  if (verify(pr)) {
    printf("Valid Signature\n");
  }

  else {
    printf("Invalid Signature\n");
  }


  tmpstr = protected_to_str(pr);
  printf("protected to str: %s\n", tmpstr);

  free_protected(pr);

  pr = str_to_protected(tmpstr);
  free(tmpstr);

  kstr = key_to_str(pr->pKey);
  char *sstr = signature_to_str(pr->sgn);
  printf("str to protected: %s %s %s\n", kstr, pr->mess, sstr);

  free(kstr);
  free(sstr);
  free_protected(pr);


  free(sKey);
  free(pKeyC);
  free(sKeyC);


  // Testing generate_random_data
  generate_random_data(500, 20);

  return 0;
}
