#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

BLIST* create_list(BIND *binding, BLIST *second) {
  BLIST *head = (BLIST*) malloc(sizeof(BLIST));
  if (head == NULL) {
    perror("Failed to create list: ");
    exit(1);
  }
  head->binding = binding;
  head->next = second;
  return head;
}

ENV* create_new_function_env(ENV* env) {
  ENV* n_env = (ENV*) malloc(sizeof(ENV));
  if (n_env == NULL) {
    perror("Failed to create environment");
    exit(1);
  }
  n_env->bindings = NULL;
  n_env->parent = env;
  return n_env;
}

BIND* create_binding(char* name, NODE* tree) {
  BIND* n_bind = (BIND*) malloc(sizeof(BIND));
  if (n_bind == NULL) {
    perror("Failed to create binding: ");
    exit(1);
  }
  n_bind->name = name;
  n_bind->tree = tree;

  return n_bind;
}

void append_list(BLIST *head, BIND *binding) {
  BLIST *ptr = head;
  while (ptr->next != NULL) {
    ptr = ptr->next;
  }
  ptr->next = create_list(binding, NULL);
}

BIND* find_name_in_list(char* name, BLIST *head) {
  BLIST *ptr = head;
  while (ptr != NULL) {
    if (strcmp(ptr->binding->name, name) == 0) {
      return ptr->binding;
    }
    ptr = ptr->next;
  }
  return NULL;
}

BIND* find_name_in_env(char* name, ENV* env) {
  BIND* ptr = NULL;
  ENV* env_ptr = env;
  while (env_ptr != NULL && (ptr = find_name_in_list(name, env_ptr->bindings)) == NULL) {
    env_ptr = env_ptr->parent;
  }
  return ptr;
}

