#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "helpers.h"

struct world {
	int memory[4096];
	struct binding_context bc_main;
	struct expression **stored_exp;
	int stored_exp_size;	
} env;

void eval_now(struct expression *e, struct binding_context *bc);

void eval_queue(struct expression *e) {
	printf("Queueing...\n");
	expression_print(e);
// 1. check if executable now
	if(e->act_mode && strcmp(e->act_mode, "now") == 0) {
	        printf("Evaluating this expression should take place now.\n");
		eval_now(e, &(env.bc_main));
	} else if(e->name) {
	        printf("This expression will be stored.\n");
		env.stored_exp = realloc(env.stored_exp, ++env.stored_exp_size * sizeof(*env.stored_exp));
		env.stored_exp[env.stored_exp_size-1] = e;		
	} else {
		printf("Not knowing how to act, skipping...\n");
	}
}

void eval_now(struct expression *e, struct binding_context *bc) {
	printf("!!! Evaluation machine should be here.\n");
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
