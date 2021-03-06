#ifndef LIST_H
#define LIST_H
#include "types.h"

BLIST* create_list(BIND*, BLIST*);
BIND* create_binding(char*, NODE*);
void append_list(BLIST*, BIND*);
BIND* find_name_in_list(char*, BLIST*);
BIND* find_name_in_env(char*, ENV*);
ENV* create_new_function_env(ENV*);
#endif

