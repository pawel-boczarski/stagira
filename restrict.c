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
//		printf("++++++++++++ type = %d", ast_type(bc->e->matter->value.list[i]));
//		ast_debug_print(bc->e->matter->value.list[i]);
		if(bc->e->matter->value.list[i]->type == ast_number) {
			if(bc->e->matter->value.list[i]->value.num == place) {
				return 1;
			}
		} else if(bc->e->matter->value.list[i]->type == ast_range) {
			struct ast *b = ast_range_begin(bc->e->matter->value.list[i]);
			struct ast *e = ast_range_end(bc->e->matter->value.list[i]);
//			printf("+++\n");
//			ast_debug_print(b);
//			ast_debug_print(e);

			int arche = 0;
			int telos = 0;

			if(b->type == ast_number) arche = b->value.num;
			else if(b->type == ast_literal) {
				printf("TODO: implement mem read access for literal\n");
				return 0;
			} else {
				printf("This kind of expression is not served for range.");
				return 0;
			}

			if(e->type == ast_number) telos = e->value.num;
			else if(e->type == ast_literal) {
				printf("TODO: implement mem read access for literal\n");
				return 0;
			} else {
				printf("This kind of expression is not served for range.");
				return 0;
			}

			if(place >= arche && place <= telos) 
				return 1;
		}
	}
    if(bc->parent) return require_mem_write_access(bc->parent, place);
	printf("Write access denied!\n");
	return 0;
}

// necessary to do the same for ranges
int require_mem_read_access(struct binding_context *bc, int place) {
	int len = ast_list_length(bc->e->species);
	for(int i = 0; i < len; i++) {
		if(bc->e->species->value.list[i]->type == ast_number) {
			if(bc->e->species->value.list[i]->value.num == place) {
				return 1;
			}
		} else if(bc->e->species->value.list[i]->type == ast_range) {
			struct ast *b = ast_range_begin(bc->e->species->value.list[i]);
			struct ast *e = ast_range_end(bc->e->species->value.list[i]);
			int arche = 0;
			int telos = 0;

			if(b->type == ast_number) arche = b->value.num;
			else if(b->type == ast_literal) {
				printf("TODO: implement mem read access for literal\n");
				return 0;
			} else {
				printf("This kind of expression is not served for range.");
				return 0;
			}

			if(e->type == ast_number) telos = e->value.num;
			else if(e->type == ast_literal) {
				printf("TODO: implement mem read access for literal\n");
				return 0;
			} else {
				printf("This kind of expression is not served for range.");
				return 0;
			}

			if(place >= arche && place <= telos) 
				return 1;
		}
	}
    if(bc->parent) return require_mem_read_access(bc->parent, place);
	printf("Read access denied!\n");
	return 0;
}