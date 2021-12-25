#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "ast.h"
#include "binding_context.h"


////////////


//////////////////

struct binding_context *binding_context_new(struct binding_context *parent, struct expression *e) {
	struct binding_context *bc;
	bc = calloc(1, sizeof(*bc));
	bc->parent = parent;
    bc->e = e;
	return bc;
}

struct ast *binding_context_get_binding(struct binding_context *bc, char *name, int check_parents) {
	if(!bc) {
		printf("Null binding context, return!");
		return NULL;
	}
	
	for(int i = 0; i < bc->bindings_size; i++) {
		if(bc->bindings[i].name && strcmp(bc->bindings[i].name, name) == 0) return bc->bindings[i].binding;
	}
	
	if(check_parents && bc->parent) {
		return binding_context_get_binding(bc->parent, name, check_parents);
	}
}

void binding_context_set_binding(struct binding_context *bc, char *name, struct ast *value) {
	if(!bc) {
		printf("Null binding context, return!\n");
		return;
	}

	for(int i = 0; i < bc->bindings_size; i++) {
		if(bc->bindings[i].name && strcmp(bc->bindings[i].name, name) == 0) {
			bc->bindings[i].binding = value; // todo free previous ast
			return;
		}
	}

	bc->bindings = realloc(bc->bindings, sizeof(*bc->bindings) * ++bc->bindings_size);
	bc->bindings[bc->bindings_size-1].name = strdup(name);
	bc->bindings[bc->bindings_size-1].binding = value;
}

void _binding_context_print(struct binding_context *bc, int print_parents, int level) {
	if(!bc) {
		for(int j = 0; j < level; j++) putchar(' '); printf("No bindings.");
		return;
	}
	for(int j = 0; j < level; j++) putchar(' '); printf("Bindings: %d\n", bc->bindings_size);
	for(int i = 0; i < bc->bindings_size; i++) {
		for(int j = 0; j < level+1; j++) putchar(' '); printf("NAME: %s\n", bc->bindings[i].name);
		ast_debug_print_level(bc->bindings[i].binding, 1+level);
	}
	if(print_parents) {
		_binding_context_print(bc->parent, print_parents, level+1);
	}
}

void binding_context_print(struct binding_context *bc, int print_parents) {
	_binding_context_print(bc, print_parents, 0);
}
