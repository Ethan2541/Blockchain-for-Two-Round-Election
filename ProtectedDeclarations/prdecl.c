#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/cryptotools.h"
#include "../Headers/prdecl.h"


/* Keys Manipulation */

Key *create_key() {
  Key *key = (Key *) malloc(sizeof(Key));

  if (key == NULL) {
    fprintf(stderr, "%s; %s; l.%d: Key Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  return key;
}



void init_key(Key *key, long val, long n) {
  // Ensure that the key is properly initialized
  assert(key);
  key->val = val;
  key->n = n;
}



void init_pair_keys(Key *pKey, Key *sKey, long low_size, long up_size) {
  assert(pKey && sKey);

  // Keys generation
  long p = random_prime_number(low_size, up_size, 5000);
  long q = random_prime_number(low_size, up_size, 5000);

  // While the two numbers aren't distinct
  while (p == q) {
    q = random_prime_number (low_size, up_size, 5000);
  }

  long n, s, u;
  generate_key_values(p, q, &n, &s, &u);


  // Ensure that the keys are positive
  if (u < 0) {
    long t = (p - 1) * (q - 1) ;
    u = u + t ; // s * u mod t = 1
  }


  // Keys initialization
  init_key(pKey, s, n);
  init_key(sKey, u, n);
}



char *key_to_str(Key *key) {
  char *str = (char *) malloc(256 * sizeof(char));

  if (str == NULL) {
    fprintf(stderr, "%s; %s; l.%d: char* Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  // Values are stored in str with the right format
  sprintf(str, "(%lx,%lx)", key->val, key->n);
  return str;
}


Key *str_to_key(char *str) {
  Key *key = create_key();

  // sscanf allows to read and store the values
  if (sscanf(str, "(%lx,%lx)", &(key->val), &(key->n)) != 2) {
    fprintf(stderr, "%s; %s; l.%d: Invalid Key Format\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  return key;
}





/* Signature */

Signature *init_signature(long *content, int size) {
  // Ensure that the long array is not NULL
  assert(content);
  Signature *signature = (Signature *) malloc(sizeof(Signature));

  if (signature == NULL) {
    fprintf(stderr, "%s; %s; l.%d: Signature Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  signature->content = content;
  signature->size = size;

  return signature;
}



Signature *sign(char *mess, Key *skey) {
  // Encryption and signature initialization
  long *content = encrypt(mess, skey->val, skey->n);
  return init_signature(content, strlen(mess));
}



char *signature_to_str(Signature *sgn) {
  char *result = (char *) malloc (10 * sgn->size * sizeof(char));
  result[0] = '#';
  int pos = 1;
  char buffer[156];

  for (int i = 0; i < sgn->size; i++) {
    sprintf(buffer, "%lx", (sgn->content)[i]);

    for (int j = 0; j < strlen(buffer); j++) {
      result[pos] = buffer[j];
      pos++;
    }

    result[pos] = '#';
    pos++;
  }

  result[pos] = '\0';
  result = realloc(result, (pos + 1) * sizeof(char));

  return result ;
}



Signature *str_to_signature(char *str) {
  int len = strlen(str);
  long *content = (long *) malloc (sizeof(long) * len);
  int num = 0;
  char buffer[256];
  int pos = 0;

  for (int i = 0; i < len; i++) {
    if (str[i] != '#') {
      buffer[pos] = str[i];
      pos++;
    }

    else {
      if (pos != 0) {
        buffer[pos] = '\0';
        sscanf(buffer, "%lx", &(content[num]));
        num++;
        pos = 0;
      }
    }
  }

  content = realloc(content, num * sizeof(long));

  return init_signature(content, num);
}



void free_signature(Signature *sgn) {
  free(sgn->content);
  free(sgn);
}





/* Protected Declarations */

Protected *init_protected(Key *pKey, char *mess, Signature *sgn) {
  // Ensure that the values given are properly initialized
  assert(pKey && mess && sgn);

  Protected *protected = (Protected *) malloc(sizeof(Protected));

  if (protected == NULL) {
    fprintf(stderr, "%s; %s; l.%d: Protected Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  protected->pKey = pKey;
  protected->mess = strdup(mess);
  protected->sgn = sgn;

  return protected;
}



int verify(Protected *pr) {
  // Decryption of the long array
  char *dmess = decrypt((pr->sgn)->content, (pr->sgn)->size, (pr->pKey)->val, (pr->pKey)->n);

  // Strings "mess" and "dmess" are compared
  int tmp = strcmp(pr->mess, dmess);
  free(dmess);

  if (tmp == 0) {
    return 1;
  }

  return 0;
}


char *protected_to_str(Protected *pr) {
  char *str = (char *) malloc (256 * sizeof(char));

  if (str == NULL) {
    fprintf(stderr, "%s; %s; l.%d: char* Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  int cpt = 0;
  char *kstr = key_to_str(pr->pKey);
  char *sstr = signature_to_str(pr->sgn);


  // The Key is first added in the string
  for (int i = 0; i < strlen(kstr); i++) {
    str[cpt++] = kstr[i];
  }

  str[cpt++] = ' ';


  // The message is then added
  for (int i = 0; i < strlen(pr->mess); i++) {
    str[cpt++] = (pr->mess)[i];
  }

  str[cpt++] = ' ';


  // And the signature is finally added
  for (int i = 0; i < strlen(sstr); i++) {
    str[cpt++] = sstr[i];
  }

  str[cpt] = '\0';

  free(kstr);
  free(sstr);

  return str;
}



Protected *str_to_protected(char *str) {
  int i = 0, cpt = 0;

  char *mess = (char *) malloc (256 * sizeof(char));
  char *kstr = (char *) malloc (256 * sizeof(char));
  char *sstr = (char *) malloc (256 * sizeof(char));

  if (mess == NULL || kstr == NULL || sstr == NULL) {
    fprintf(stderr, "%s; %s; l.%d: char* Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  // The first part (Key) of the string is read and stored
  while ((str[cpt] != ' ') && (str[cpt] != '\0')) {
    kstr[i++] = str[cpt++];
  }

  kstr[i] = '\0';
  cpt++;
  i = 0;

  // The second part (Message) of the string is read and stored
  while ((str[cpt] != ' ') && (str[cpt] != '\0')) {
    mess[i++] = str[cpt++];
  }

  mess[i] = '\0';
  cpt++;
  i = 0;

  // The last part (Signature) of the string is read and stored
  while ((str[cpt] != ' ') && (str[cpt] != '\0')) {
    sstr[i++] = str[cpt++];
  }

  sstr[i] = '\0';


  // The key and the signature are both created
  Key *pKey = str_to_key(kstr);
  Signature *sgn = str_to_signature(sstr);

  free(kstr);
  free(sstr);


  // Leading to the creation of the protected
  Protected *pr = init_protected(pKey, mess, sgn);
  free(mess);

  return pr;
}



void free_protected(Protected *pr) {
  free(pr->pKey);
  free(pr->mess);
  free_signature(pr->sgn);
  free(pr);
}





/* Simulation */

void generate_random_data(int nv, int nc) {
  if ((nv == 0) || (nv < nc)) {
    return;
  }

  FILE *f1 = fopen("../Database/keys.txt", "w");
  FILE *f2 = fopen("../Database/candidates.txt", "w");
  FILE *f3 = fopen("../Database/declarations.txt", "w");

  if ((f1 == NULL) || (f2 == NULL) || (f3 == NULL)) {
    fprintf(stderr, "%s; %s; l.%d: Can't open file(s): ../Database/keys.txt or ../Database/candidates.txt or ../Database/declarations.txt\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }


  /* A Structure "KArray" is prepared in order to list every Key Couple and check
  whether they have been selected as candidated thanks to an int */

  KArray *cartecit = (KArray *) malloc(nv * sizeof(KArray));

  if (cartecit == NULL) {
    fprintf(stderr, "%s; %s; l.%d: KArray Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }


  // Citizens' keys are added
  for (int i = 0; i < nv ; i++) {
    (cartecit[i]).pKey = create_key();
    (cartecit[i]).sKey = create_key();
  }


  // Array of the Candidates' public keys
  Key **pKeytab = (Key **) malloc(nc * sizeof(Key*));

  if (pKeytab == NULL) {
    free(cartecit);
    fprintf(stderr, "%s; %s; l.%d: Key Array Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    exit(EXIT_FAILURE);
  }

  char *pKeystr, *sKeystr;

  // The keys are then written
  for (int i = 0; i < nv; i++) {
    init_pair_keys((cartecit[i]).pKey, (cartecit[i]).sKey, 3, 7);
    pKeystr = key_to_str((cartecit[i]).pKey);
    sKeystr = key_to_str((cartecit[i]).sKey);

    fprintf(f1, "%s %s\n", pKeystr, sKeystr);

    (cartecit[i]).b = 0;

    free(pKeystr);
    free(sKeystr);
  }


  // The candidates are chosen randomly
  int cpt = 0; int p;

  while (cpt < nc) {
    p = rand() % nv;

    if ((cartecit[p]).b == 0) {
      (cartecit[p]).b = 1;

      pKeystr = key_to_str((cartecit[p]).pKey);
      pKeytab[cpt] = (cartecit[p]).pKey;

      fprintf(f2, "%s\n", pKeystr);

      free(pKeystr);
      cpt++;
    }
  }


  // Protected Declarations are finally written
  Signature *sgn;
  Protected *pr;
  char *prstr;

  for (int i = 0; i < nv; i++) {
    p = rand() % nc;
    pKeystr = key_to_str(pKeytab[p]);

    // Signature created following the public key chosen
    sgn = sign(pKeystr, (cartecit[i]).sKey);
    // Protected accordingly created
    pr = init_protected((cartecit)[i].pKey, pKeystr, sgn);

    free(pKeystr);

    prstr = protected_to_str(pr);

    fprintf(f3, "%s\n", prstr);

    free(prstr);
    free_signature(sgn);
    free(pr->mess);
    free(pr);
  }


  // The memory used is freed
  for (int i = 0; i < nv; i++) {
    free((cartecit[i]).pKey);
    free((cartecit[i]).sKey);
  }

  free(cartecit);
  free(pKeytab);

  fclose(f1);
  fclose(f2);
  fclose(f3);
}
