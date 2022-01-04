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

struct ast *ast_range_new(struct ast *a, struct ast *b) {
	struct ast *r = ast_list_new();
	if(a->type != ast_number && a->type != ast_literal && a->type != ast_expression) {
		printf("Only literals, numbers and expressions may be range ends, have (left side): ");
		ast_debug_print(a);
	//	assert(0);
		return NULL;
	}
	if(b->type != ast_number && b->type != ast_literal && b->type != ast_expression) {
		printf("Only literals, numbers and expressions may be range ends, have (right side): ");
		ast_debug_print(b);
	//	assert(0);
		return NULL;
	}
	ast_list_append(r, a);
	ast_list_append(r, b);
	r->type = ast_range;
	return r;
}

struct ast *ast_range_begin(struct ast *a) {
	assert(a->type == ast_range);
	return a->value.list[0];
}

struct ast *ast_range_end(struct ast *a) {
	assert(a->type == ast_range);
	return a->value.list[1];
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

struct ast *ast_in_new() {
	struct ast *r = calloc(1, sizeof(*r)); r->type = ast_in;
	return r;
}

struct ast *ast_out_new() {
	struct ast *r = calloc(1, sizeof(*r)); r->type = ast_out;
	return r;
}

struct ast *ast_bind_new() {
	struct ast *r = calloc(1, sizeof(*r)); r->type = ast_bind;
	return r;
}

int ast_type(struct ast *a) {
	return a ? a->type : -127;
}

void ast_delete(struct ast *a) {
	if(!a) return;
	int type = a->type;
	if(type == ast_string || type == ast_literal) free(a->value.str);
	if(type == ast_range) {
		ast_delete(a->value.list[0]);
		ast_delete(a->value.list[1]);
	}
	for(int i = 0; i < a->type; i++) { /* all non-list types are negative */
	    ast_delete(a->value.list[i]); // TODO ast_delete?
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
	else if(a->type == ast_literal) printf("LITERAL: '%s'\n", a->value.str);
	else if(a->type == ast_range) {
		printf("RANGE:\n");
			ast_debug_print_level(a->value.list[0], depth+1);
			ast_debug_print_level(a->value.list[1], depth+1);
	}
	else if(a->type == ast_number) printf("NUM: %d\n", a->value.num);
	else if(a->type >= ast_list_first) {
		printf("LIST:\n");
		for(int i = 0; i < a->type; i++) {
			ast_debug_print_level(a->value.list[i], depth+1);
		}
	} else if(a->type == ast_expression) {
		_expression_print(a->value.e, depth+1);
	} else {
		// TODO: expressions don't go well!
		printf("Unknown type: %d\n", a->type);
	}
}

void ast_debug_print(struct ast *a) {
	ast_debug_print_level(a, 0);
}
