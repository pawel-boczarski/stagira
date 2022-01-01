#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ast.h"
#include "expression.h"


struct expression* accidental_expression_new(
                                 struct ast *form,
                                 struct ast *accidental_matter, struct ast *accidental_species) {
	struct expression *ret = calloc(1,sizeof(*ret));
	ret->form = form;
	ret->accidental_matter = accidental_matter;
	ret->accidental_species = accidental_species;
	return ret;
}

void accidental_expression_print(struct expression *e) {
	printf("FORM:\n");
	ast_debug_print_level(e->form, 1);
	printf("ACCIDENTAL MATTER:\n");
	ast_debug_print_level(e->accidental_matter, 1);
	printf("ACCIDENTAL SPECIES:\n");
	ast_debug_print_level(e->accidental_species, 1);
	
	printf("NAME: '%s'\n", e->name);
}


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

void _expression_print(struct expression *e, int level) {
	for(int i = 0; i < level;i++) putchar(' '); printf("EFFICIENT CAUSE: '%s'\n", e->act_mode);
	for(int i = 0; i < level;i++) putchar(' '); printf("FINAL CAUSE: '%s'\n", e->goal);
	for(int i = 0; i < level;i++) putchar(' '); printf("MATTER:\n");
	ast_debug_print_level(e->matter, 1+level);
	for(int i = 0; i < level;i++) putchar(' '); printf("SPECIES:\n");
	ast_debug_print_level(e->species, 1+level);
	for(int i = 0; i < level;i++) putchar(' '); printf("FORM:\n");
	ast_debug_print_level(e->form, 1+level);
	for(int i = 0; i < level;i++) putchar(' '); printf("ACCIDENTAL MATTER:\n");
	ast_debug_print_level(e->accidental_matter, 1+level);
	for(int i = 0; i < level;i++) putchar(' '); printf("ACCIDENTAL SPECIES:\n");
	ast_debug_print_level(e->accidental_species, 1+level);
	
	for(int i = 0; i < level;i++) putchar(' '); printf("NAME: '%s'\n", e->name);
}

void expression_print(struct expression *e) {
	_expression_print(e, 0);
}