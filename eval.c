#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "binding_context.h"
#include "expression.h"
#include "restrict.h"
#include "eval.h"

struct world {
	int memory[4096];
	struct binding_context bc_main;
	struct expression **stored_exp;
	int stored_exp_size;
	int debug_on;	
} env;

struct ast* eval_queue(struct expression *e) {
//	printf("Queueing...\n");
//	expression_print(e);
// 1. check if executable now
	if(e->act_mode && strcmp(e->act_mode, "now") == 0) {
//	    printf("Evaluating this expression should take place now.\n");
		env.bc_main.e = e;
		return eval_now(&(env.bc_main));
	} else if(e->name) {
//	        printf("This expression will be stored.\n");
		env.stored_exp = realloc(env.stored_exp, ++env.stored_exp_size * sizeof(*env.stored_exp));
		env.stored_exp[env.stored_exp_size-1] = e;		
	} else {
		printf("Not knowing how to act, skipping...\n");
	}
}

struct ast* eval_now(struct binding_context *bc) {
//	printf("!!! Evaluation machine should be here.\n");
	if(ast_type(bc->e->form) == ast_literal) {
		if(strcmp(bc->e->form->value.str, "print") == 0) {
			return eval_print(bc);
		}
		else if(strcmp(bc->e->form->value.str, "get") == 0) {
			return eval_get(bc);
		} else if(strcmp(bc->e->form->value.str, "set") == 0) {
			return eval_set(bc);
		} else {
			printf("Searching for stored ones...\n");
			for(int i = 0; i < env.stored_exp_size; i++) {
				if(strcmp(bc->e->form->value.str, env.stored_exp[i]->name) == 0) {
					printf("Go with stored expression...\n");
//					env.bc_main.e = env.stored_exp[i];
					struct binding_context *bc = binding_context_new(&env.bc_main, env.stored_exp[i]);
					// bindings now ?
					struct ast *retval = eval_now(bc);
					return free(bc), retval;
				}
			}
		}
	}
}

struct ast* eval_print(struct binding_context *bc) {
	// need to check if "out" is accessible
	if(require_out_access(bc)) {
		if(ast_list_length(bc->e->accidental_species) > 0) {
			// --> get_accidental_species_no(e, i) ?
			// --> get_accidental_species_at(e, i) ?
			int as1 = bc->e->accidental_species->value.list[0]->value.num;
			printf("%d\n", as1);
		} else {
			fprintf(stderr, "Error: no accidental to print!\n");
		}
	}
	return NULL;
}

struct ast *_eval_get(struct binding_context *bc, int direct) {
	// we need "where to" and "what"
	struct ast *result = NULL;
	if(ast_list_length(bc->e->accidental_matter) < 1) {
		printf("get: no output destination\n");
		binding_context_print(bc, 1);
		return NULL;
	}
	if(ast_list_length(bc->e->accidental_species) < 1) {
		printf("get: no input place\n");
		binding_context_print(bc, 1);
		return NULL;
	}


	if(bc->e->accidental_species->value.list[0]->type == ast_number) {
		int read_dest = bc->e->accidental_species->value.list[0]->value.num;
		if(!require_mem_read_access(bc, read_dest)) {
			printf("get: denied read access\n");
			binding_context_print(bc, 1);
		}

		if(direct) {
			result = ast_int_new(bc->e->accidental_species->value.list[0]->value.num);
		} else {
			result = ast_int_new(env.memory[bc->e->accidental_species->value.list[0]->value.num]);
		}
	} else {
		printf("Don't know how to serve other argument than num for get.\n");
		return NULL;
	}

	if(bc->e->accidental_matter->value.list[0]->type == ast_number) {
		int write_dest = bc->e->accidental_matter->value.list[0]->value.num;
		if(!require_mem_write_access(bc, write_dest)) {
			printf("get: denied write access\n");
			binding_context_print(bc, 1);
			return NULL;
		}
		env.memory[write_dest] = result->value.num;
		ast_delete(result);
		return NULL;
	} else if(bc->e->accidental_matter->value.list[0]->type == ast_literal) {
		if(strcmp(bc->e->accidental_matter->value.list[0]->value.str, "out") == 0) {
			ast_debug_print(result);
			ast_delete(result);
			return NULL;
		} else if(strcmp(bc->e->accidental_matter->value.list[0]->value.str, "bind") == 0) {
			return result;
		}
		
	}
	
	return NULL;
}

struct ast *eval_get(struct binding_context *bc) {
	return _eval_get(bc, 0);
}

struct ast* eval_set(struct binding_context *bc) {
	return _eval_get(bc, 1);
}