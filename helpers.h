#ifndef __HELPERS
#define __HELPERS

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

struct expression {
	char *act_mode; /* call, now, after(nn), call-from(...) */
	char *goal;
	struct ast *matter; /* everything this call might change [] */
	struct ast *species; /* everything this call might read */
	struct ast *form; /* function name of expression; */
	struct ast *accidental_matter; /* variables that are passed by reference, to write */
	struct ast *accidental_species; /* variables that are passed by values, to read */
	char *name;
};

struct expression* expression_new(char *act_mode, char *goal,
                                 struct ast *matter, struct ast *species, struct ast *form,
                                 struct ast *accidental_matter, struct ast *accidental_species, char *name);

struct expression expression_print(struct expression *e);


#endif
