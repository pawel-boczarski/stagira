#ifndef __AST_H
#define __AST_H

#include "expression.h"

#define ast_list_length ast_type

enum {
   ast_expression = -5,
   ast_number = -4,
   ast_range = -3,
   ast_string = -2,
   ast_literal = -1,
   ast_list_first = 0 /* all positive values are list lengths */
};

struct ast {
   int type;
   union {
      int num;
      char *str;
      struct expression *e;
      struct ast **list;
   } value;
};

struct ast *ast_int_new(int i);
struct ast *ast_range_new(struct ast *a, struct ast *b);
struct ast *ast_range_begin(struct ast *a);
struct ast *ast_range_end(struct ast *a);
struct ast *ast_string_new(char *s);
struct ast *ast_literal_new(char *s);

struct ast *ast_list_new();
struct ast *ast_list_append(struct ast *a, struct ast *b);

struct ast *ast_expression_new(struct expression *e);

void ast_delete(struct ast *a);
int ast_type(struct ast *a); /* if >= 0, this is a list, otherwise, ast_number or ast_string */

void ast_debug_print(struct ast *a);

void ast_debug_print_level(struct ast *a, int level);

#endif