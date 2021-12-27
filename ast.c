#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "ast.h"
#include "expression.h"

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

struct ast *ast_expression_new(struct expression *e) {
    struct ast *r = calloc(1, sizeof(*r)); r->type = ast_expression, r->value.e = e;
    return r;
}

int ast_type(struct ast *a) {
	return a ? a->type : -127;
}

void ast_delete(struct ast *a) {
	int type = a->type;
	if(type == ast_string) free(a->value.str);
	for(int i = 0; i < a->type; i++) { /* all non-list types are negative */
	    free(a->value.list[i]);
	}
	free(a);
}

void ast_debug_print_level(struct ast *a, int depth) {
	for(int i = 0; i < depth; i++) printf(" ");
        if(!a) {
        	printf("(empty result)\n");
        	return;
        }
        
	if(a->type == ast_string) printf("STR: '%s'\n", a->value.str);
	if(a->type == ast_literal) printf("LITERAL: '%s'\n", a->value.str);
	else if(a->type == ast_number) printf("NUM: %d\n", a->value.num);
	else if(a->type >= ast_list_first) {
		printf("LIST:\n");
		for(int i = 0; i < a->type; i++) {
			ast_debug_print_level(a->value.list[i], depth+1);
		}
	}
}

void ast_debug_print(struct ast *a) {
	ast_debug_print_level(a, 0);
}
