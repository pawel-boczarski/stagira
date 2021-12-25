#ifndef __AST_H
#define __AST_H

#define ast_list_length ast_type

enum {
   ast_number = -3,
   ast_string = -2,
   ast_literal = -1,
   ast_list_first = 0 /* all positive values are list lengths */
};

struct ast {
   int type;
   union {
      int num;
      char *str;
      struct ast **list;
   } value;
};

struct ast *ast_int_new(int i);
struct ast *ast_string_new(char *s);
struct ast *ast_literal_new(char *s);

struct ast *ast_list_new();
struct ast *ast_list_append(struct ast *a, struct ast *b);

void ast_delete(struct ast *a);
int ast_type(struct ast *a); /* if >= 0, this is a list, otherwise, ast_number or ast_string */

void ast_debug_print(struct ast *a);

void ast_debug_print_level(struct ast *a, int level);

#endif