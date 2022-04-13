#ifndef LIST_H
#define LIST_H


/* Structures */

typedef struct _cellKey {
  Key *data;
  struct _cellKey *next;
} CellKey;

typedef struct _cellProtected {
  Protected *data;
  struct _cellProtected *next;
} CellProtected;



/* CellKey */

CellKey *create_cell_key(Key *key);
void insert_cell_key(CellKey **list, Key *key);
CellKey *read_public_keys(char *filename);
void print_list_keys(char *filename, CellKey *LCK);
void delete_cell_key(CellKey *c);
void delete_list_keys(CellKey *list);



/* CellProtected */

CellProtected *create_cell_protected(Protected *pr);
void insert_cell_protected(CellProtected **list, Protected *pr);
CellProtected *read_protected(char *filename);
void print_list_protected(char *filename, CellProtected *LCP);
void delete_cell_protected(CellProtected *c);
void delete_list_protected(CellProtected *list);
void merge_list_protected(CellProtected **list1, CellProtected **list2);



/* Delete Invalid Protected */

void delete_false_protected(CellProtected **list);


#endif
