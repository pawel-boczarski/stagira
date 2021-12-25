#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"
#include "expression.h"

struct expression* expression_new(char *act_mode, char *goal,
                                 struct ast *matter, struct ast *species, struct ast *form,
                                 struct ast *accidental_matter, struct ast *accidental_species, char *name) {
	struct expression *ret = calloc(1,sizeof(*ret));
	ret->act_mode = act_mode ? strdup(act_mode) : NULL;
	ret->goal = goal ? strdup(goal) : NULL;
	ret->matter = matter;
	ret->species = species;
	ret->form = form;
	ret->accidental_matter = accidental_matter;
	ret->accidental_species = accidental_species;
	ret->name = name ? strdup(name) : NULL;
	return ret;
}

struct expression expression_print(struct expression *e) {
	printf("EFFICIENT CAUSE: '%s'\n", e->act_mode);
	printf("FINAL CAUSE: '%s'\n", e->act_mode);
	printf("MATTER:\n");
	ast_debug_print_level(e->matter, 1);
	printf("SPECIES:\n");
	ast_debug_print_level(e->species, 1);
	printf("FORM:\n");
	ast_debug_print_level(e->form, 1);
	printf("ACCIDENTAL MATTER:\n");
	ast_debug_print_level(e->accidental_matter, 1);
	printf("ACCIDENTAL SPECIES:\n");
	ast_debug_print_level(e->accidental_species, 1);
	
	printf("NAME: '%s'\n", e->name);
}
