#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "binding_context.h"
#include "expression.h"
#include "eval.h"
#include "restrict.h"

int require_out_access(struct binding_context *bc) {
	int len = ast_list_length(bc->e->matter);
	for(int i = 0; i < len; i++) {
		if(bc->e->matter->value.list[i]->type == ast_literal) {
			if(strcmp(bc->e->matter->value.list[i]->value.str, "out") == 0) {
//				printf("Access OK!\n");
				return 1;
			}
		}
	}
    if(bc->parent) return require_out_access(bc->parent);
	printf("Out access denied!\n");
	return 0;
}

int require_in_access(struct binding_context *bc) {
	int len = ast_list_length(bc->e->species);
	for(int i = 0; i < len; i++) {
		if(bc->e->species->value.list[i]->type == ast_literal) {
			if(strcmp(bc->e->species->value.list[i]->value.str, "in") == 0) {
				return 1;
			}
		}
	}
    if(bc->parent) return require_in_access(bc->parent);
	printf("In access denied!\n");
	return 0;
}


int require_mem_write_access(struct binding_context *bc, int place) {
	int len = ast_list_length(bc->e->matter);
	for(int i = 0; i < len; i++) {
		if(bc->e->matter->value.list[i]->type == ast_number) {
			if(bc->e->matter->value.list[i]->value.num == place) {
//				printf("Write access OK!\n");
				return 1;
			}
		}
	}
    if(bc->parent) return require_mem_write_access(bc->parent, place);
	printf("Write access denied!\n");
	return 0;
}

int require_mem_read_access(struct binding_context *bc, int place) {
	int len = ast_list_length(bc->e->species);
	for(int i = 0; i < len; i++) {
		if(bc->e->species->value.list[i]->type == ast_number) {
			if(bc->e->species->value.list[i]->value.num == place) {
//				printf("Access OK!\n");
				return 1;
			}
		}
	}
    if(bc->parent) return require_mem_read_access(bc->parent, place);
	printf("Read access denied!\n");
	return 0;
}