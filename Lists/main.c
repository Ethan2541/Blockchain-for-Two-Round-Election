#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../Headers/prdecl.h"
#include "../Headers/list.h"

int main(int argc, char **argv) {
  srand(time(NULL));

  // Testing the structures and the functions
  CellKey *keyList = read_public_keys("../Database/keys.txt");
  print_list_keys("Tests/TestKeys", keyList);
  delete_list_keys(keyList);

  CellProtected *prList = read_protected("../Database/declarations.txt");
  print_list_protected("Tests/TestProtected", prList);
  delete_list_protected(prList);


  // Testing delete_false_protected
  int p = rand() % 10;
  CellProtected *prList2 = read_protected("../Database/declarations.txt");
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
  print_list_protected("Tests/TestDFP", prList2);
  delete_list_protected(prList2);
}
