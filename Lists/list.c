#include <stdio.h>
#include <stdlib.h>
#include "../CryptographyTools/cryptotools.h"
#include "../ProtectedDeclarations/prdecl.h"
#include "list.h"


/* CellKey Functions */

CellKey *create_cell_key(Key *key) {
  CellKey *cell = (CellKey *) malloc(sizeof(CellKey));

  if (cell == NULL) {
    fprintf(stderr, "PARTIE 3 - ERREUR ALLOCATION CREATE_CELL_KEY\n");
    exit(EXIT_FAILURE);
  }

  cell->data = key;
  cell->next = NULL;

  return cell;
}



void insert_cell_key(CellKey **list, Key *key) {
  CellKey *cell = create_cell_key(key);
  cell->next = *list;
  *list = cell;
}



CellKey *read_public_keys(char *filename) {
  FILE *f = fopen(filename, "r");

  if (f == NULL) {
    fprintf(stderr, "Can't open the file: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  char buffer[256];
  char pKeystr[256];
  CellKey *list = NULL;


  // Reading the file
  while (fgets(buffer, 256, f) != NULL) {
    if (sscanf(buffer, "%s", pKeystr) != 1) {
      fprintf(stderr, "Invalid Key Format\n");
      exit(EXIT_FAILURE);
    }

    // Creation of the keys following each line's data
    insert_cell_key(&list, str_to_key(pKeystr));
  }

  fclose(f);
  return list;
}



void print_list_keys(char *filename, CellKey *LCK) {
  FILE *f = fopen(filename, "w");

  if (f == NULL) {
    fprintf(stderr, "Can't open file: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  char *pKeystr;

  // Browsing LCK to print every key
  while (LCK != NULL) {
    pKeystr = key_to_str(LCK->data);
    fprintf(f, "%s\n", pKeystr);
    free(pKeystr);
    LCK = LCK->next;
  }

  fclose(f);
}



void delete_cell_key(CellKey *c) {
  free(c->data);
  free(c);
}



void delete_list_keys(CellKey *list) {
  CellKey *tmp;

  // Browsing list to free every key
  while (list != NULL) {
    tmp = list;
    list = list->next;
    delete_cell_key(tmp);
  }
}





/* CellProtected Functions */

CellProtected *create_cell_protected(Protected *pr) {
  CellProtected *cell = (CellProtected *) malloc(sizeof(CellProtected));

  if (cell == NULL) {
    fprintf(stderr, "PARTIE 3 - ERREUR ALLOCATION CREATE_CELL_PROTECTED\n");
    exit(EXIT_FAILURE);
  }

  cell->data = pr;
  cell->next = NULL;

  return cell;
}



void insert_cell_protected(CellProtected **list, Protected *pr) {
  CellProtected *cell = create_cell_protected(pr);
  cell->next = *list;
  *list = cell;
}



CellProtected *read_protected(char *filename) {
  FILE *f = fopen(filename, "r");

  if (f == NULL) {
    fprintf(stderr, "PARTIE 3 - ERREUR OUVERTURE FICHIER READ_PUBLIC_KEYS\n");
    exit(EXIT_FAILURE);
  }

  char buffer[256];
  char prstr[256];
  CellProtected *list = NULL;


  // Reading the file
  while (fgets(buffer, 256, f) != NULL) {
    if (sscanf(buffer, "%[^\n]", prstr) != 1) {
      fprintf(stderr, "Invalid Protected Format\n");
      exit(EXIT_FAILURE);
    }

    // Creating Protected following each line's data
    insert_cell_protected(&list, str_to_protected(prstr));
  }

  fclose(f);
  return list;
}



void print_list_protected(char *filename, CellProtected *LCP) {
  FILE *f = fopen(filename, "w");

  if (f == NULL) {
    fprintf(stderr, "Can't open file: %s\n", filename);
    exit(EXIT_FAILURE);
  }

  char *prstr;


  // Browsing LCP to print every protected declaration
  while (LCP != NULL) {
    prstr = protected_to_str(LCP->data);
    fprintf(f, "%s\n", prstr);
    free(prstr);
    LCP = LCP->next;
  }

  fclose(f);
}



void delete_cell_protected(CellProtected *c) {
  free_protected(c->data);
  free(c);
}



void delete_list_protected(CellProtected *list) {
  CellProtected *tmp;

  // Browsing list to free every cell
  while (list != NULL) {
    tmp = list;
    list = list->next;
    delete_cell_protected(tmp);
  }
}





/* Delete Invalid Protected */

void delete_false_protected(CellProtected **list) {
  CellProtected *tmp;


  // Checking the first cell and the following "first" cells if deleted
  while ((verify((*list)->data) == 0) && (*list != NULL)) {
    tmp = *list;
    *list = (*list)->next;
    delete_cell_protected(tmp);
  }


  CellProtected *prec = *list;

  // The list may be empty
  if (prec == NULL) {
    return;
  }


  CellProtected *c = (*list)->next;

  // Browsing the list
  while (c != NULL) {
    // Veryfing the protected declarations
    if (verify(c->data) == 0) {
      prec->next = c->next;
      tmp = c;
      c = c->next;
      delete_cell_protected(tmp);
    }

    else {
      prec = c;
      c = c->next;
    }
  }
}
