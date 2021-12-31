#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "ast.h"
#include "binding_context.h"


struct binding_context *binding_context_new(struct binding_context *parent, struct expression *e) {
	struct binding_context *bc;
	bc = calloc(1, sizeof(*bc));
	assert(parent != bc->parent);
	bc->parent = parent;
    bc->e = e;
	return bc;
}

void binding_context_delete(struct binding_context *bc) {
	for(int i = 0; i < bc->bindings_size; i++) {
		free(bc->bindings[i].name);
		//ast_delete()
	}
	free(bc);
}

struct ast *binding_context_get_binding(struct binding_context *bc, char *name, int check_parents) {
//	printf("get bcsb ptr=%p, name=%s\n", bc, name);
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

	printf("Got no binding.\n");
	return NULL;
}

// todo shouldn't we check in parents?
int binding_context_is_bound(struct binding_context *bc, char *name) {
	//printf("checking '%s'...", name);
	if(!bc) {
		printf("Null binding context, return!");
		return 0;
	}

	if(strcmp(name, "in") == 0 || strcmp(name, "out") == 0 || strcmp(name, "bind") == 0) {
	//	printf("Bound.\n");
		return 1;
	}
	
	for(int i = 0; i < bc->bindings_size; i++) {
		if(bc->bindings[i].name && strcmp(bc->bindings[i].name, name) == 0) { /*printf("Bound.\n");*/ return 1; }
	}

    //printf("Not bound!\n");
	return 0;
#if 0
	if(check_parents && bc->parent) {
		return binding_context_get_binding(bc->parent, name, check_parents);
	}
#endif
}

void binding_context_set_binding(struct binding_context *bc, char *name, struct ast *value) {
//	printf("set bcsb ptr=%p, name=%s\n", bc, name);
//	printf("value:\n");
//	ast_debug_print(value);
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
	printf("Expression: ");
	expression_print(bc->e);
	for(int j = 0; j < level; j++) putchar(' '); printf("Bindings: %d\n", bc->bindings_size);
	for(int i = 0; i < bc->bindings_size; i++) {
		for(int j = 0; j < level+1; j++) putchar(' ');printf("NAME: %s\n", bc->bindings[i].name);
		ast_debug_print_level(bc->bindings[i].binding, 1+level);
		printf("Expression: ");
		expression_print(bc->e);
	}
	if(print_parents) {
		_binding_context_print(bc->parent, print_parents, level+1);
	}
}

void binding_context_print(struct binding_context *bc, int print_parents) {
	_binding_context_print(bc, print_parents, 0);
}
