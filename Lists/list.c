#include <stdio.h>
#include <stdlib.h>
#include "../Headers/prdecl.h"
#include "../Headers/list.h"


/* CellKey Functions */

CellKey *create_cell_key(Key *key) {
  CellKey *cell = (CellKey *) malloc(sizeof(CellKey));

  if (cell == NULL) {
    fprintf(stderr, "%s; %s; l.%d: CellKey Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
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
    fprintf(stderr, "%s; %s; l.%d: Can't open the file: %s\n", __FILE__, __PRETTY_FUNCTION__, __LINE__, filename);
    exit(EXIT_FAILURE);
  }

  char buffer[256];
  char pKeystr[256];
  CellKey *list = NULL;


  // Reading the file
  while (fgets(buffer, 256, f) != NULL) {
    if (sscanf(buffer, "%s", pKeystr) != 1) {
      fprintf(stderr, "%s; %s; l.%d: Invalid Key Format\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
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
    fprintf(stderr, "%s; %s; l.%d: Can't open file: %s\n", __FILE__, __PRETTY_FUNCTION__, __LINE__, filename);
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
    fprintf(stderr, "%s; %s; l.%d: CellProtected Allocation Error\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
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
    fprintf(stderr, "%s; %s; l.%d: Can't open file: %s\n", __FILE__, __PRETTY_FUNCTION__, __LINE__, filename);
    exit(EXIT_FAILURE);
  }

  char buffer[256];
  char prstr[256];
  CellProtected *list = NULL;


  // Reading the file
  while (fgets(buffer, 256, f) != NULL) {
    if (sscanf(buffer, "%[^\n]", prstr) != 1) {
      fprintf(stderr, "%s; %s; l.%d: Invalid Protected Format\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
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
    fprintf(stderr, "%s; %s; l.%d: Can't open file: %s\n", __FILE__, __PRETTY_FUNCTION__, __LINE__, filename);
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



void merge_list_protected(CellProtected **list1, CellProtected **list2) {
  // First List is Empty
  if (*list1 == NULL) {
    *list1 = *list2;
    // The Second List is then "destroyed" without freeing the memory
    *list2 = NULL;
  }


  // Else, we go at the end of the first list and set the first element of the second list as the follower
  CellProtected *c = *list1;

  while (c->next != NULL) {
    c = c->next;
  }

  c->next = *list2;
  *list2 = NULL;
}





/* Delete Invalid Protected */

void delete_false_protected(CellProtected **list) {
  CellProtected *tmp;


  // Checking the first cell and the following "first" cells if deleted
  while ((*list != NULL) && (verify((*list)->data) == 0)) {
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
