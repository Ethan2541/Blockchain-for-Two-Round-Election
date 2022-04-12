#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../CryptographyTools/cryptotools.h"
#include "../ProtectedDeclarations/prdecl.h"
#include "list.h"

int main(int argc, char **argv) {
  srand(time(NULL));

  // Testing the structures and the functions
  CellKey *keyList = read_public_keys("../ProtectedDeclarations/keys.txt");
  print_list_keys("TestKeys", keyList);
  delete_list_keys(keyList);
  printf("\n");

  CellProtected *prList = read_protected("../ProtectedDeclarations/declarations.txt");
  print_list_protected("TestProtected", prList);
  delete_list_protected(prList);
  printf("\n");


  // Testing delete_false_protected
  int p = rand() % 10;
  CellProtected *prList2 = read_protected("../ProtectedDeclarations/declarations.txt");
  CellProtected *cell = prList2;

  while ((cell != NULL) && p != 0) {
    p--;
    cell = cell->next;
  }

  // Tampering with the list of declarations
  free((cell->data)->mess);
  (cell->data)->mess = strdup("FALSE");

  // Cleaning the list
  delete_false_protected(&prList2);
  print_list_protected("TestDFP", prList2);
  delete_list_protected(prList2);
}
