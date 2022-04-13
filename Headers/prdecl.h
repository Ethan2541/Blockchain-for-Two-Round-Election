#ifndef PRDECL_H
#define PRDECL_H

/* Structures */

typedef struct _key {
  long val;
  long n;
} Key;

typedef struct _signature {
  int size;
  long *content;
} Signature;

typedef struct _protected {
  Key *pKey;
  char *mess;
  Signature *sgn;
} Protected;

typedef struct _kArray {
  Key *pKey;
  Key *sKey;
  int b;
} KArray;



/* Keys Manipulation */

Key *create_key();
void init_key(Key *key, long val, long n);
void init_pair_keys(Key *pKey, Key *sKey, long low_size, long up_size);
char *key_to_str(Key *key);
Key *str_to_key(char *str);



/* Signatures */

Signature *init_signature(long *content, int size);
Signature *sign(char *mess, Key *skey);
char *signature_to_str(Signature *sgn);
Signature *str_to_signature(char *str);
void free_signature(Signature *sgn);



/* Protected */

Protected *init_protected(Key *pkey, char *mess, Signature *sgn);
int verify(Protected *pr);
char *protected_to_str(Protected *pr);
Protected *str_to_protected(char *str);
void free_protected(Protected *pr);



/* Simulation */

void generate_random_data(int nv, int nc);


#endif
