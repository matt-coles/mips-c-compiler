#ifndef TYPES_H
#define TYPES_H
#include "nodes.h"

typedef struct binding {
  char* name;
  NODE* tree;
} BIND;

typedef struct blist {
  BIND* binding;
  struct blist *next;
} BLIST;


typedef struct environ {
  BLIST *bindings;
  struct environ *parent;
} ENV;
#endif
