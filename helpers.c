#include "helpers.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

struct ast *ast_int_new(int i) {
    struct ast *r = calloc(1, sizeof(*r)); r->type = ast_number, r->value.num = i;
    return r;
}

struct ast *ast_string_new(char *s) {
    struct ast *r = calloc(1, sizeof(*r)); r->type = ast_string, r->value.str = strdup(s);
    return r;
}

struct ast *ast_literal_new(char *s) {
    struct ast *r = calloc(1, sizeof(*r)); r->type = ast_literal, r->value.str = strdup(s);
    return r;
}


struct ast *ast_list_new() {
    struct ast *r = calloc(1, sizeof(*r)); r->type = ast_list_first, r->value.list = NULL;
    return r;
}

struct ast *ast_list_append(struct ast *a, struct ast *b) {
	assert(a->type >= 0);
	a->value.list = realloc(a->value.list, sizeof(*a->value.list) * ++a->type), a->value.list[a->type-1] = b;
	return a;
}

int ast_type(struct ast *a) {
	return a->type;
}

void ast_delete(struct ast *a) {
	int type = a->type;
	if(type == ast_string) free(a->value.str);
	for(int i = 0; i < a->type; i++) { /* all non-list types are negative */
	    free(a->value.list[i]);
	}
	free(a);
}

void _ast_debug_print(struct ast *a, int depth) {
	for(int i = 0; i < depth; i++) printf(" ");
        if(!a) {
        	printf("(empty list)\n");
        	return;
        }
        
	if(a->type == ast_string) printf("STR: '%s'\n", a->value.str);
	if(a->type == ast_literal) printf("LITERAL: '%s'\n", a->value.str);
	else if(a->type == ast_number) printf("NUM: %d\n", a->value.num);
	else if(a->type >= ast_list_first) {
		printf("LIST:\n");
		for(int i = 0; i < a->type; i++) {
			_ast_debug_print(a->value.list[i], depth+1);
		}
	}
}

void ast_debug_print(struct ast *a) {
	_ast_debug_print(a, 0);
}

////////////

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
	_ast_debug_print(e->matter, 1);
	printf("SPECIES:\n");
	_ast_debug_print(e->species, 1);
	printf("FORM:\n");
	_ast_debug_print(e->form, 1);
	printf("ACCIDENTAL MATTER:\n");
	_ast_debug_print(e->accidental_matter, 1);
	printf("ACCIDENTAL SPECIES:\n");
	_ast_debug_print(e->accidental_species, 1);
	
	printf("NAME: '%s'\n", e->name);
}

