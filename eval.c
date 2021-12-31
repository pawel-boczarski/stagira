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
struct ast *_eval_in_child_context (struct binding_context *bc, struct expression *e);

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
		} else if(strcmp(bc->e->form->value.str, "seq") == 0) {
			return eval_seq(bc);
		} else if(strcmp(bc->e->form->value.str, "if") == 0) {
			return eval_if(bc);
		} else if(strcmp(bc->e->form->value.str, "loop") == 0) {
			return eval_loop(bc);
		} else if(strcmp(bc->e->form->value.str, "op") == 0) {
			return eval_op(bc);
		} else {
			for(int i = 0; i < env.stored_exp_size; i++) {
				// note this will run more functions if more are defined - do we support "universals" ? ;)
				if(strcmp(bc->e->form->value.str, env.stored_exp[i]->name) == 0) {
					_eval_in_child_context(&env.bc_main, env.stored_exp[i]);
				}
			}
		}
	}
}

struct ast *_eval_in_child_context (struct binding_context *bc, struct expression *e) {
	struct binding_context *bc_child = binding_context_new(bc, e); // todo check - is the expression copied?
	
	int mno = ast_list_length(e->matter);
	int sno = ast_list_length(e->species);

	int am_no = ast_list_length(bc->e->accidental_matter);
	int s_no = ast_list_length(bc->e->accidental_species);

	// todo what if something appears in both places - avoid this!
	int am_to_bind = 0;
	for(int i = 0; i < mno; i++) {
		if(e->matter->value.list[i]->type == ast_literal && !binding_context_is_bound(bc_child, e->matter->value.list[i]->value.str)) {
			if(am_to_bind >= am_no) {
				printf("_eval_in_child_context: could not bind '%s' write-out parameter for function '%s'",
							e->matter->value.list[i]->value.str,
							e->form->value.str);
				binding_context_delete(bc_child);
				return NULL;
			}
			binding_context_set_binding(bc_child, e->matter->value.list[i]->value.str, bc->e->accidental_matter->value.list[am_to_bind++]);
		}
	}

	int s_to_bind = 0;
	for(int i = 0; i < sno; i++) {
		if(e->species->value.list[i]->type == ast_literal && !binding_context_is_bound(bc_child, e->species->value.list[i]->value.str)) {
			if(s_to_bind >= s_no) {
				printf("_eval_in_child_context: could not bind '%s' read-out parameter for function '%s'",
							e->species->value.list[i]->value.str,
							e->form->value.str);
				binding_context_delete(bc_child);
				return NULL;
			}
			binding_context_set_binding(bc_child, e->species->value.list[i]->value.str, bc->e->accidental_species->value.list[s_to_bind++]);
		}
	}

	struct ast *retval = eval_now(bc_child);
	binding_context_delete(bc_child);
	return retval;
}

struct ast* eval_print(struct binding_context *bc) {
	// need to check if "out" is accessible
	if(require_out_access(bc)) {
		if(ast_list_length(bc->e->accidental_species) > 0) {
			// --> get_accidental_species_no(e, i) ?
			// --> get_accidental_species_at(e, i) ?
			for(int i = 0; i < ast_list_length(bc->e->accidental_species); i++) {
				if(bc->e->accidental_species->value.list[i]->type == ast_number) {
					int as1 = bc->e->accidental_species->value.list[i]->value.num;
					printf("%d\n", bc->e->accidental_species->value.list[i]->value.num);
				} else if(bc->e->accidental_species->value.list[i]->type == ast_string) {
					printf("%s\n", bc->e->accidental_species->value.list[i]->value.str);
				} else {
					printf("print: don't know how to print this accidental\n");
				}
			}
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

	int read_dest; // todo this could be something else...
	if(bc->e->accidental_species->value.list[0]->type == ast_number) {
		read_dest = bc->e->accidental_species->value.list[0]->value.num;
	} else if(bc->e->accidental_species->value.list[0]->type == ast_literal) { /* this will need change for "in" */
		if(strcmp(bc->e->accidental_species->value.list[0]->value.str, "in") == 0) {
			scanf(stdin, "%d", &read_dest);
		} else {
			struct ast *binding = binding_context_get_binding(bc->parent, bc->e->accidental_species->value.list[0]->value.str, 0);
			if(!binding) {
				printf("get: cannot get binding '%s'\n", bc->e->accidental_species->value.list[0]->value.str);
				return NULL;
			}
			if(binding->type != ast_number) {
				printf("get: cannot serve other type than number, and binding '%s' does not represent it\n",
						bc->e->accidental_species->value.list[0]->value.str);
			return NULL;
			}
			read_dest = binding->value.num;
		}
	} else if(bc->e->accidental_species->value.list[0]->type == ast_expression) {
		struct binding_context *bc_child = binding_context_new(bc, bc->e->accidental_species->value.list[0]->value.e);
		// bindings now ?
		struct ast *return_value = eval_now(bc_child);
		if(return_value->type != ast_number) {
			printf("get: can't serve other input destination than number for get\n");
			return NULL;
		}
		read_dest = return_value->value.num;
		binding_context_delete(bc_child);
	} else {
		printf("Don't know how to serve other argument than num for get.\n");
		return NULL;
	}
	if(!direct && !require_mem_read_access(bc, read_dest)) {
		//printf("get: denied read access\n");
			//binding_context_print(bc, 1);
	return NULL;
	}

	if(direct) {
		result = ast_int_new(read_dest);
	} else {
		result = ast_int_new(env.memory[read_dest]);
	}

	// this code to be stored
	if(bc->e->accidental_matter->value.list[0]->type == ast_number) {
		int write_dest = bc->e->accidental_matter->value.list[0]->value.num;
		if(!require_mem_write_access(bc, write_dest)) {
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

struct ast *eval_op(struct binding_context *bc) {
	int as_no = ast_list_length(bc->e->accidental_species);
	int am_no = ast_list_length(bc->e->accidental_matter);
	struct ast *retval = NULL, *result = NULL;
	if(as_no < 3) {
		printf("op: at least three read parameters necessary");
		return NULL;
	}
	if(am_no < 1) {
		printf("op: at least one write parameter necessary");
	}

	struct ast *as = bc->e->accidental_species;
	struct ast *am = bc->e->accidental_matter;
	int arg[2];
	char *opname;
//	int result = 0;

	for(int i = 0; i < 2; i++) {
		if(as->value.list[2*i]->type == ast_expression) {
			struct ast *ret = _eval_in_child_context(bc, as->value.list[2*i]->value.e);
			if(ret->type != ast_number) {
			printf("eval_op: argument %d not a number", i+1);
			return NULL;
		}
		arg[i] = ret->value.num;
		} else if(as->value.list[2*i]->type == ast_number) {
			arg[i] = as->value.list[2*i]->value.num;
		} else {
			printf("eval_op: argument %d not a number", i+1);
			return NULL;
		}
	}

	if(as->value.list[1]->type != ast_string) {
		printf("eval_op: operator not a string");
		return NULL;
	}
	opname = as->value.list[1]->value.str;

	if(strcmp(opname, "<") == 0) result = ast_int_new(!!(arg[0] < arg[1]));
	else if(strcmp(opname, "<=") == 0) result = ast_int_new(!!(arg[0] <= arg[1]));
	else if(strcmp(opname, ">") == 0) result = ast_int_new(!!(arg[0] > arg[1]));
	else if(strcmp(opname, ">=") == 0) result = ast_int_new(!!(arg[0] >= arg[1]));
	else if(strcmp(opname, "=") == 0) result = ast_int_new(!!(arg[0] == arg[1]));
	else if(strcmp(opname, "~=") == 0) result = ast_int_new(!!(arg[0] != arg[1]));
	else if(strcmp(opname, "+") == 0) result = ast_int_new(arg[0] + arg[1]);
	else if(strcmp(opname, "-") == 0) result = ast_int_new(arg[0] - arg[1]);
	else if(strcmp(opname, "*") == 0) result = ast_int_new(arg[0] * arg[1]);
	else if(strcmp(opname, "/") == 0) result = ast_int_new(arg[0] / arg[1]);
	else if(strcmp(opname, "%%") == 0) result = ast_int_new(arg[0] % arg[1]);

	// this code to be stored
	if(bc->e->accidental_matter->value.list[0]->type == ast_number) {
		int write_dest = bc->e->accidental_matter->value.list[0]->value.num;
		if(!require_mem_write_access(bc, write_dest)) {
			ast_delete(result);
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
	
	return retval;
}

struct ast *eval_seq(struct binding_context *bc) {
	int am_no = ast_list_length(bc->e->accidental_species);
	struct ast *retval = NULL; /* warning - if "bind" used, we should bind - we will try binding here first */
	for(int i = 0; i < am_no; i++) {
		if(bc->e->accidental_species->value.list[i]->type != ast_expression) {
			printf("seq: this is not an expression!\n");
			ast_debug_print(bc->e->accidental_species->value.list[i]);
			return NULL;
		}
		struct ast *r = _eval_in_child_context(bc, bc->e->accidental_species->value.list[i]->value.e);
		if(r) retval = r; /* last result matches */
	}
	return retval;
}

int _is_simple_condition_true(struct ast *cond) {
	if(cond->type == ast_number) {
		return cond->value.num != 0;
	} else if(cond->type == ast_string) {
		return cond->value.str && strlen(cond->value.str) > 0;
	}

	printf("Assuming unrecognized condition false...");
	return 0;
}

struct ast *_eval_loop_if(struct binding_context *bc, int loop) {
	int am_no = ast_list_length(bc->e->accidental_species);
	if(am_no != 2) {
		printf("if/loop: should have exactly two accidental arguments");
		return NULL;
	}

	struct ast *ifcond = NULL;
	struct ast *retval = NULL; /* warning - if "bind" used, we should bind - we will try binding in seq first */
	do {
	if(bc->e->accidental_species->value.list[0]->type == ast_expression) {
		ifcond = _eval_in_child_context(bc, bc->e->accidental_species->value.list[0]->value.e);
	} else if(bc->e->accidental_species->value.list[0]->type == ast_number) {
		ifcond = bc->e->accidental_species->value.list[0];
	} else {
		printf("if/loop: don't know how to serve this kind of condition\n");
		return NULL;
	}

	if(!_is_simple_condition_true(ifcond)) {
		return retval;
	}

	if(bc->e->accidental_species->value.list[1]->type == ast_expression) {
		retval = _eval_in_child_context(bc, bc->e->accidental_species->value.list[1]->value.e);
	} else {
		return bc->e->accidental_species->value.list[1];
	}
	} while(loop);

	return retval;
}

struct ast *eval_if(struct binding_context *bc) {
	return _eval_loop_if(bc, 0);
}

struct ast *eval_loop(struct binding_context *bc) {
	return _eval_loop_if(bc, 1);
}