#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "binding_context.h"
#include "expression.h"
#include "eval.h"

struct world {
	int memory[4096];
	struct binding_context bc_main;
	struct expression **stored_exp;
	int stored_exp_size;	
} env;

int require_out_access(struct binding_context *bc) {
	printf("require_out_access...\n");
	int len = ast_list_length(bc->e->matter);
	for(int i = 0; i < len; i++) {
		if(bc->e->matter->value.list[i]->type == ast_literal) {
			if(strcmp(bc->e->matter->value.list[i]->value.str, "out") == 0) {
				printf("Access OK!\n");
				return 1;
			}
		}
	}
	printf("Out access denied!\n");
	return 0;
}

int require_in_access(struct binding_context *bc) {
	printf("require_in_access: always admit for now...\n");
	return 1;
}


struct ast* eval_queue(struct expression *e) {
	printf("Queueing...\n");
	expression_print(e);
// 1. check if executable now
	if(e->act_mode && strcmp(e->act_mode, "now") == 0) {
	    printf("Evaluating this expression should take place now.\n");
		env.bc_main.e = e;
		return eval_now(&(env.bc_main));
	} else if(e->name) {
	        printf("This expression will be stored.\n");
		env.stored_exp = realloc(env.stored_exp, ++env.stored_exp_size * sizeof(*env.stored_exp));
		env.stored_exp[env.stored_exp_size-1] = e;		
	} else {
		printf("Not knowing how to act, skipping...\n");
	}
}

struct ast* eval_now(struct binding_context *bc) {
	printf("!!! Evaluation machine should be here.\n");
	if(ast_type(bc->e->form) == ast_literal) {
		if(strcmp(bc->e->form->value.str, "print") == 0) {
			return eval_print(bc);
		}
		else if(strcmp(bc->e->form->value.str, "get") == 0) {
			return eval_get(bc);
		} else if(strcmp(bc->e->form->value.str, "set") == 0) {
			return eval_set(bc);
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
			printf("> %d\n", as1);
		} else {
			fprintf(stderr, "Error: no accidental to print!\n");
		}
	}
	return NULL;
}

struct ast *eval_get(struct binding_context *bc) {
	return NULL;
}

struct ast* eval_set(struct binding_context *bc) {
	return NULL;
}

#if 0
void eval_set(struct ast *e) {
   // create binding context
   // create "matter1" as first
   // create "form1" as second
   // go internal
   if(e->matter[0]->type == ast_int) {
   }
   if(e->species[0]->type == ast_int) {
   } else {
   	assert(0 && "Not implemented.");
   }
   
   printf("Skipping the check...");
   memory[e->matter[0]->num] = e->form[0]->num;
}

int eval_get(struct ast *e) {
	
}

int eval_set(struct ast *e) {
}

int eval_get_internal(struct ast *e, struct binding_context *bc) {
	
}

int eval_set_internal(struct ast *e, struct binding_context *bc) {
// get binding of

}


int eval_cond(struct ast *e) {
}

int eval_loop() {
}
#endif
