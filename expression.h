#ifndef __EXPRESSION_H
#define __EXPRESSION_H


struct expression {
	char *name; /* might be null, esp. for immediate expression */
	char *act_mode; /* call, now, after(nn), call-from(...) */
	char *goal;
	struct ast *matter; /* everything this call might change [] */
	struct ast *species; /* everything this call might read */
	struct ast *form; /* function name of expression; */
	struct ast *accidental_matter; /* variables that are passed by reference, to write */
	struct ast *accidental_species; /* variables that are passed by values, to read */
};

struct expression* accidental_expression_new(struct ast *form,
                                 struct ast *accidental_matter, struct ast *accidental_species);

void accidental_expression_print(struct expression *e);

struct expression* expression_new(char *act_mode, char *goal,
                                 struct ast *matter, struct ast *species, struct ast *form,
                                 struct ast *accidental_matter, struct ast *accidental_species, char *name);


void _expression_print(struct expression *e, int level);
void expression_print(struct expression *e);

#endif