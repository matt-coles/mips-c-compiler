#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "nodes.h"
#include "C.tab.h"
#include <string.h>
#include "types.h"
#include "list.h"

char *named(int t)
{
    static char b[100];
    if (isgraph(t) || t==' ') {
      sprintf(b, "%c", t);
      return b;
    }
    switch (t) {
      default: return "???";
    case IDENTIFIER:
      return "id";
    case CONSTANT:
      return "constant";
    case STRING_LITERAL:
      return "string";
    case LE_OP:
      return "<=";
    case GE_OP:
      return ">=";
    case EQ_OP:
      return "==";
    case NE_OP:
      return "!=";
    case EXTERN:
      return "extern";
    case AUTO:
      return "auto";
    case INT:
      return "int";
    case VOID:
      return "void";
    case APPLY:
      return "apply";
    case LEAF:
      return "leaf";
    case IF:
      return "if";
    case ELSE:
      return "else";
    case WHILE:
      return "while";
    case CONTINUE:
      return "continue";
    case BREAK:
      return "break";
    case RETURN:
      return "return";
    }
}



void print_leaf(NODE *tree, int level)
{
    TOKEN *t = (TOKEN *)tree;
    int i;
    for (i=0; i<level; i++) putchar(' ');
    if (t->type == CONSTANT) printf("%d\n", t->value);
    else if (t->type == STRING_LITERAL) printf("\"%s\"\n", t->lexeme);
    else if (t) puts(t->lexeme);
}

void print_tree0(NODE *tree, int level)
{
    int i;
    if (tree==NULL) return;
    if (tree->type==LEAF) {
      print_leaf(tree->left, level);
    }
    else {
      for(i=0; i<level; i++) putchar(' ');
      printf("%s\n", named(tree->type));

/*       if (tree->type=='~') { */
/*         for(i=0; i<level+2; i++) putchar(' '); */
/*         printf("%p\n", tree->left); */
/*       } */
/*       else */
        print_tree0(tree->left, level+2);
      print_tree0(tree->right, level+2);
    }
}

// forward declare because it is used in add_var_to_env
int recursive_interpret(NODE*, ENV*);

void print_tree(NODE *tree)
{
    print_tree0(tree, 0);
}

void add_function_to_env(NODE *tree, ENV *env_ptr) {
  NODE* func_name = tree->left->right->left->left;
  TOKEN* name_token = (TOKEN*) func_name;
  printf("Added function name %s to environment with value: \n", name_token->lexeme);
  print_tree(tree);
  if (env_ptr->bindings == NULL) {
    env_ptr->bindings = create_list(create_binding(name_token->lexeme, tree), NULL);
  } else {
    append_list(env_ptr->bindings, create_binding(name_token->lexeme, tree));
  }
}


void add_var_to_env(NODE *tree, ENV *env_ptr) {
  NODE* var_name = tree->left->left;
  TOKEN* name_token = (TOKEN*) var_name;
  TOKEN* tok = new_token(INT);
  tok->value = recursive_interpret(tree->right, env_ptr);
  asprintf(&tok->lexeme, "%d", tok->value);
  printf("Added variable name %s to environment with value: %s\n", name_token->lexeme, tok->lexeme);
  if (env_ptr->bindings == NULL) {
    env_ptr->bindings = create_list(create_binding(name_token->lexeme, (NODE*) tok), NULL);
  } else {
    append_list(env_ptr->bindings, create_binding(name_token->lexeme, (NODE*) tok));
  }
}

int recursive_interpret(NODE *tree, ENV *env_ptr) {
  if (tree==NULL) return 0;
  if (tree->type==LEAF) {
    if (tree->left->type == CONSTANT) {
      return ((TOKEN *) tree->left)->value;
    } else if (tree->left->type == STRING_LITERAL) {
      printf("Not implemented\n");
      exit(1);
    } else if (tree->left->type == INT) {
      // do nothing we dont care about types
      return 0;
    } else {
      TOKEN* tok = (TOKEN *) tree->left;
      BIND* var_bind = find_name_in_env(tok->lexeme, env_ptr);
      if (var_bind == NULL) {
        printf("Could not find variable %s\n", tok->lexeme);
        exit(1);
      }
      TOKEN* var_tok = (TOKEN *) var_bind->tree;
      return var_tok->value;
    }
  }
  if (tree->type=='D') {
    // this is a function definition
    add_function_to_env(tree, env_ptr);
    return 0;
  }
  if (tree->type=='=') {
    // this is a variable definition
    add_var_to_env(tree, env_ptr);
    return 0;
  }
  if (tree->type==APPLY) {
    ENV* new_env = create_new_function_env(env_ptr);
    char* func_name = ((TOKEN*) tree->left->left)->lexeme;
    BIND* func = find_name_in_env(func_name, env_ptr);
    if (func == NULL) {
      printf("Could not find binding for function with name %s\n", func_name);
      exit(1);
    }
    return recursive_interpret(func->tree->right, new_env);
  }
  if (tree->type == '+') {
    return recursive_interpret(tree->left, env_ptr) + recursive_interpret(tree->right, env_ptr);
  }
  if (tree->type == RETURN) {
    return recursive_interpret(tree->left, env_ptr);
  }
  recursive_interpret(tree->left, env_ptr);
  recursive_interpret(tree->right, env_ptr);
  return 0;
}

ENV* cons_global_env(NODE *tree) {
  ENV* global = create_new_function_env(NULL);
  recursive_interpret(tree, global);
  return global;
}

void interpret_tree(NODE *tree) {
  ENV* global_env = cons_global_env(tree);
  BIND* ref_main = find_name_in_env("main", global_env);
  if (ref_main == NULL) {
    printf("Could not find main, cannot run!\n");
    exit(1);
  }
  // print ref_main to make sure we really got it
  printf("Located %s, ready to run!\n", ref_main->name);
  printf("%d\n", recursive_interpret(ref_main->tree->right, global_env));
}

extern int yydebug;
extern NODE* yyparse(void);
extern NODE* ans;
extern void init_symbtable(void);

int main(int argc, char** argv)
{
    NODE* tree;
    if (argc>1 && strcmp(argv[1],"-d")==0) yydebug = 1;
    init_symbtable();
    printf("--C COMPILER\n");
    yyparse();
    tree = ans;
    printf("parse finished with %p\n", tree);
    print_tree(tree);
    interpret_tree(tree);
    return 0;
}
