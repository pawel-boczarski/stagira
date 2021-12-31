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

// TODO single literal values handled in eval, ranges here - not a consistence at all...
int _require_access(struct binding_context *bc, int place, int write) {
	struct ast *access_list = write ? bc->e->matter : bc->e->species;
	int len = ast_list_length(access_list);
	for(int i = 0; i < len; i++) {
		if(access_list->value.list[i]->type == ast_number) {
			if(access_list->value.list[i]->value.num == place) {
				return 1;
			}
		} else if(access_list->value.list[i]->type == ast_range) {
			struct ast *b = ast_range_begin(access_list->value.list[i]);
			struct ast *e = ast_range_end(access_list->value.list[i]);

			int arche = 0;
			int telos = 0;

			if(b->type == ast_number) arche = b->value.num;
			else if(b->type == ast_literal) {
				struct ast *binding = binding_context_get_binding(bc, b->value.str, 1);
				if(!binding) {
					printf("ptr=%p\n", bc->parent);
					printf("_require_access: cannot get binding '%s'\n", b->value.str);
					return 0;
				}
				if(binding->type != ast_number) {
					printf("TODO: implement recursive bindings if possible, can't serve this now\n");
					return 0;
				}
				arche = binding->value.num;
			}
			else {
				printf("get: cannot resolve binding in range not of literal type");
				return 0;
			}

			if(e->type == ast_number) telos = e->value.num;
			else if(e->type == ast_literal) {
				struct ast *binding = binding_context_get_binding(bc, e->value.str, 1);
				if(!binding) {
					printf("_require_access: cannot get binding '%s'\n", e->value.str);
					return 0;
				}
				if(binding->type != ast_number) {
					printf("TODO: implement recursive bindings if possible, can't serve this now\n");
					return 0;
				}
				telos = binding->value.num;
			}
			else {
				printf("get: cannot resolve binding in range not of literal type");
				return 0;
			}

			if(place >= arche && place <= telos) 
				return 1;
		}
	}
    if(bc->parent) return _require_access(bc->parent, place, write);
	printf("%s access denied at %d!\n", write ? "Write" : "Read", place);
//	binding_context_print(bc, 1);
	return 0;
}

int require_mem_write_access(struct binding_context *bc, int place) {
	return _require_access(bc, place, 1);
}


int require_mem_read_access(struct binding_context *bc, int place) {
	return _require_access(bc, place, 0);
}