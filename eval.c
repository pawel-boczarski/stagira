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
	if(e->act_mode && strcmp(e->act_mode, "now") == 0) {
		env.bc_main.e = e;
		return eval_now(&(env.bc_main));
	} else if(e->act_mode && strcmp(e->act_mode, "func") == 0) {
		if(!e->name) {
			printf("eval_queue: Can't store a function with no name, did you forget : \"functionName\" before the semicolon?\n");
			return NULL;
		}
		for(int i = 0; i < env.stored_exp_size; i++) {
			if(strcmp(env.stored_exp[i]->name, e->name) == 0) {
				printf("eval_queue: Can't store function with name '%s' twice!\n", e->name);
				return NULL;
			}
		}
		env.stored_exp = realloc(env.stored_exp, ++env.stored_exp_size * sizeof(*env.stored_exp));
		env.stored_exp[env.stored_exp_size-1] = e;
	} else {
		printf("eval_queue: Not knowing how to act, skipping...\n");
	}
	return NULL;
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
		} else if(strcmp(bc->e->form->value.str, "pst") == 0) {
			return eval_pst(bc);
		} else {
			for(int i = 0; i < env.stored_exp_size; i++) {
				// note this will run more functions if more are defined - do we support "universals" ? ;)
				if(strcmp(bc->e->form->value.str, env.stored_exp[i]->name) == 0) {
//					printf("BC accidentals: "); ast_debug_print(bc->e->accidental_matter);
					return _eval_in_child_context(bc, env.stored_exp[i]);
				}
			}
			printf("Error: tried calling a non-existent function '%s'!\n", bc->e->form->value.str);
//			eval_pst(bc);
			return 0;
		}
	} else {
		assert(0);
	}
}

int _create_bindings(struct binding_context *bc_child, struct ast *access_list, struct ast *accidental_access_list, struct expression *e) {
	struct binding_context *bc = bc_child->parent; // todo check - is the expression copied?
	
	int al_no = ast_list_length(access_list);
	int aal_no = ast_list_length(accidental_access_list);

	// todo what if something appears in both places - avoid this!
	int aal_to_bind = 0;
	for(int i = 0; i < al_no; i++) {
		if(access_list->value.list[i]->type == ast_literal && !binding_context_is_bound(bc_child, access_list->value.list[i]->value.str, 0)) {
			if(aal_to_bind >= aal_no) {
				printf("_eval_in_child_context: could not bind '%s' parameter for function '%s'",
							access_list->value.list[i]->value.str,
							bc->e->form->value.str);
				printf("Aal to bind = %d, aal_no = %d\n", aal_to_bind, aal_no);
				assert(0);
				binding_context_delete(bc_child);
				return 0;
			}

			struct ast *current_aal_binding = accidental_access_list->value.list[aal_to_bind];
			if(current_aal_binding->type == ast_expression) {
				struct expression *sube = current_aal_binding->value.e;
				struct ast *ret = _eval_in_child_context(bc, sube);
				if(!ret) {
					printf("eval: could not evaluate subexpression as binding, the expression was: ");
					expression_print(sube);
				}
				binding_context_set_binding(bc_child, access_list->value.list[i]->value.str, ret);
			} else if(current_aal_binding->type == ast_number) {
				binding_context_set_binding(bc_child, access_list->value.list[i]->value.str, current_aal_binding);
			} else if(current_aal_binding->type == ast_literal) {
				binding_context_set_binding(bc_child, access_list->value.list[i]->value.str, current_aal_binding);
			} else if(current_aal_binding->type == ast_out) {
				eval_pst(bc);
				assert(0);
			} else if(current_aal_binding->type == ast_in) {
				eval_pst(bc);
				assert(0);
			} else if(current_aal_binding->type == ast_bind) {
				eval_pst(bc);
				assert(0);
			}
			aal_to_bind++;
		} else if(access_list->value.list[i]->type == ast_range) {
			struct ast *left = access_list->value.list[i]->value.list[0];
			struct ast *right = access_list->value.list[i]->value.list[1];
			int left_bound = (left->type == ast_number) || binding_context_is_bound(bc_child, left->value.str, 0);
			int right_bound = (right->type == ast_number) || binding_context_is_bound(bc_child, right->value.str, 0);

			if(!left_bound && !right_bound) {

				if(!accidental_access_list->value.list[aal_to_bind]) {
					printf("eval: one of bindings is invalid for range.\n");
					return 0;
				}

				if(accidental_access_list->value.list[aal_to_bind]->type != ast_range) {
					printf("eval: We don't do this way. When both range ends are unbound we support a..b range as parameter\n");
					return 0;
				}

				struct ast *accidental_left_bind, *accidental_right_bind;
				if(accidental_access_list->value.list[aal_to_bind]->value.list[0]->type == ast_expression) {
					accidental_left_bind = _eval_in_child_context(bc, accidental_access_list->value.list[aal_to_bind]->value.list[0]->value.e);
				} else {
					accidental_left_bind = accidental_access_list->value.list[aal_to_bind]->value.list[0];
				}

				if(accidental_access_list->value.list[aal_to_bind]->value.list[1]->type == ast_expression) {
					accidental_right_bind = _eval_in_child_context(bc, accidental_access_list->value.list[aal_to_bind]->value.list[1]->value.e);
				} else {
					accidental_right_bind = accidental_access_list->value.list[aal_to_bind]->value.list[1];
				}

				binding_context_set_binding(bc_child, left->value.str, accidental_left_bind);

				binding_context_set_binding(bc_child, right->value.str, accidental_right_bind);
				aal_to_bind++;
			} else if(left_bound && !right_bound) {
				// todo expression binding won't work. Check if "normal" operation for single literal will.
				if(right->type == ast_literal) {
					if(aal_to_bind >= aal_no) { /* todo up */
						printf("_eval_in_child_context: could not bind '%s' write-out parameter for function '%s'",
								access_list->value.list[i]->value.str,
								e->form->value.str);
						binding_context_delete(bc_child);
						return 0;
					}
					// todo make a function or macro out of this?
					struct ast *accidental_bind;
					if(accidental_access_list->value.list[aal_to_bind]->type == ast_expression) {
						accidental_bind = _eval_in_child_context(bc, accidental_access_list->value.list[aal_to_bind]->value.e);
					} else {
						accidental_bind = accidental_access_list->value.list[aal_to_bind];
					}

					binding_context_set_binding(bc_child, right->value.str, accidental_bind);
					aal_to_bind++;
				} else {
					printf("eval: do not know how to serve left range bind not being int, literal or expression.\n");
					return 0;
				}
			} else if(!left_bound && right_bound) {
				// todo expression binding won't work. Check if "normal" operation for single literal will.
				if(left->type == ast_literal) {
					if(aal_to_bind >= aal_no) {
						printf("_eval_in_child_context: could not bind '%s' write-out parameter for function '%s'",
								access_list->value.list[i]->value.str,
								e->form->value.str);
						binding_context_delete(bc_child);
						return 0;
					}

					// todo make a function or macro out of this?
					struct ast *accidental_bind;
					if(accidental_access_list->value.list[aal_to_bind]->type == ast_expression) {
						accidental_bind = _eval_in_child_context(bc, accidental_access_list->value.list[aal_to_bind]->value.e);
					} else {
						accidental_bind = accidental_access_list->value.list[aal_to_bind];
					}

					binding_context_set_binding(bc_child, left->value.str, accidental_bind);
					aal_to_bind++;
				} else {
					printf("eval: do not know how to serve right range bind not being int or literal.\n");
					return 0;
				}
			}
		}
	}
	return 1;
}

struct ast *_eval_in_child_context (struct binding_context *bc, struct expression *e) {
	struct binding_context *bc_child = binding_context_new(bc, e); // todo check - is the expression copied?
	
	int mno = ast_list_length(e->matter);
	int sno = ast_list_length(e->species);

	int am_no = ast_list_length(bc->e->accidental_matter);
	int s_no = ast_list_length(bc->e->accidental_species);

	int am_to_bind = 0;

	if(!_create_bindings(bc_child, e->matter, bc->e->accidental_matter, e)) {
		printf("_eval_in_child_context: could not bind write-out parameters.\n");
		
		return NULL;
	}

	if(!_create_bindings(bc_child, e->species, bc->e->accidental_species, e)) {
		printf("_eval_in_child_context: could not bind read-in parameters.\n");
		return NULL;
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

/// destination = as->value.list[0]
struct ast *_decode_parameter(struct binding_context *bc, struct ast *destination) { // TODO origin should be stack trace...
	struct ast *read_dest = NULL; // todo this could be something else...
	if(destination->type == ast_number) {
		read_dest = ast_int_new(destination->value.num);
	} else if(destination->type == ast_literal) { /* this will need change for "in" */
		if(strcmp(destination->value.str, "in") == 0) {
			return ast_in_new();
		} else if(strcmp(destination->value.str, "out") == 0) {
			return ast_out_new();
		} else if(strcmp(destination->value.str, "bind") == 0) {
			return ast_bind_new();
		} else {
			struct ast *binding = binding_context_get_binding(bc/*->parent*/, destination->value.str, 1);
			if(!binding) {
				printf("get: cannot get binding '%s' in context of '%s'\n", destination->value.str, bc->e->form->value.str);
				eval_pst(bc);
				// todo stack trace
			//	return NULL;
			}

			if(binding->type == ast_literal && strcmp(binding->value.str, "out") == 0) return ast_out_new();
			if(binding->type == ast_literal && strcmp(binding->value.str, "in") == 0) return ast_in_new();
			if(binding->type == ast_literal && strcmp(binding->value.str, "bind") == 0) return ast_bind_new();

			struct binding_context *current_bc = bc->parent;

			if(!binding) {
				printf("get: could not unwind binding '%s'\n", destination->value.str);
				// todo stack trace
				return NULL;
			}

			if(binding->type != ast_number) {
				return binding;
			}
			return ast_int_new(binding->value.num);
		}
	} else if(destination->type == ast_expression) {
		// todo change this
		struct binding_context *bc_child = binding_context_new(bc, destination->value.e);
		// bindings now ?
		struct ast *return_value = eval_now(bc_child);
		if(!return_value) {
			printf("_decode_parameter: could not decode, child returned null value ('bind' missing?) !");
		}
		if(return_value->type != ast_number) {
			printf("get: can't serve other input destination than number for get\n");
			return NULL;
		}
		read_dest = ast_int_new(return_value->value.num); // it looks like a memory leak
		binding_context_delete(bc_child);
		return read_dest;
	} else {
		printf("Don't know how to serve other argument than num for get.\n");
		return NULL;
	}
}

// TODO access control for literal bindings in matter/form won't work for now...
struct ast *_eval_get(struct binding_context *bc, int direct) {
	// we need "where to" and "what"
	struct ast *result = NULL;
	if(ast_list_length(bc->e->accidental_matter) < 1) {
		printf("get: no output destination\n");
		return NULL;
	}
	if(ast_list_length(bc->e->accidental_species) < 1) {
		printf("get: no input place\n");
		return NULL;
	}

	struct ast *am = bc->e->accidental_matter; // TODO refactor
	struct ast *as = bc->e->accidental_species;

	struct ast *decoded_src = _decode_parameter(bc, as->value.list[0]);
	int read_dest;
	if(decoded_src->type == ast_number) {
		read_dest = decoded_src->value.num;
	} else if(decoded_src->type == ast_in) {
			scanf("%d", &read_dest);
	} else if(decoded_src->type == ast_out) {
		printf("get: This is impossible for 'out' to be an input parameter\n");
		return NULL;
	} else if(decoded_src->type == ast_bind) {
		printf("get: this is impossible to bind an input parameter\n");
		return NULL;
	}

	if(!direct && !require_mem_read_access(bc, read_dest)) {
		return NULL;
	}

	if(direct) {
		result = ast_int_new(read_dest);
	} else {
		result = ast_int_new(env.memory[read_dest]);
	}

	// todo this code to be stored
	struct ast *decoded_dest = _decode_parameter(bc, am->value.list[0]);
	int write_dest;
	if(decoded_dest->type == ast_number) {
		int write_dest = decoded_dest->value.num;
		if(!require_mem_write_access(bc, write_dest)) {
			return NULL;
		}
		env.memory[write_dest] = result->value.num;
		ast_delete(result);
		return NULL;
	} else if(decoded_dest->type == ast_in) {
		printf("get: This is impossible for 'in' to be an output parameter\n");
		return NULL;
	} else if(decoded_dest->type == ast_out) {
		if(!require_out_access(bc)) {
			return NULL;
		}
		ast_debug_print(result);
		ast_delete(result);
		return NULL;
	} else if(decoded_dest->type == ast_bind) {
		return result;
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
		return NULL;
	}

	struct ast *as = bc->e->accidental_species;
	struct ast *am = bc->e->accidental_matter;
	char *opname;

	if(as->value.list[1]->type != ast_string) {
		printf("eval_op: operator not a string");
		return NULL;
	}

	// This will likely fail if something is not right...
	opname = as->value.list[1]->value.str;

	int argv[2];
	for(int i = 0; i < 2; i++) {
		struct ast *arg = _decode_parameter(bc, as->value.list[2*i]);
		if(!arg) {
			printf("Could not decode the argument for op!");
			return free(arg), NULL;
		}
		if(arg->type == ast_number) {
			argv[i] = arg->value.num;
		} else if(arg->type == ast_in) {
			scanf("%d", &argv[i]);
		} else if(arg->type == ast_out) {
			printf("op: This is impossible for 'out' to be an input parameter\n");
			return ast_delete(arg), NULL;
		} else if(arg->type == ast_bind) {
			printf("op: this is impossible to bind an input parameter\n");
			return ast_delete(arg), NULL;
		}
		ast_delete(arg);
	}

	if(strcmp(opname, "<") == 0) result = ast_int_new(!!(argv[0] < argv[1]));
	else if(strcmp(opname, "<=") == 0) result = ast_int_new(!!(argv[0] <= argv[1]));
	else if(strcmp(opname, ">") == 0) result = ast_int_new(!!(argv[0] > argv[1]));
	else if(strcmp(opname, ">=") == 0) result = ast_int_new(!!(argv[0] >= argv[1]));
	else if(strcmp(opname, "=") == 0) result = ast_int_new(!!(argv[0] == argv[1]));
	else if(strcmp(opname, "~=") == 0) result = ast_int_new(!!(argv[0] != argv[1]));
	else if(strcmp(opname, "+") == 0) result = ast_int_new(argv[0] + argv[1]);
	else if(strcmp(opname, "-") == 0) result = ast_int_new(argv[0] - argv[1]);
	else if(strcmp(opname, "*") == 0) result = ast_int_new(argv[0] * argv[1]);
	else if(strcmp(opname, "/") == 0) result = ast_int_new(argv[0] / argv[1]);
	else if(strcmp(opname, "%%") == 0) result = ast_int_new(argv[0] % argv[1]);


	struct ast *decoded_dest = _decode_parameter(bc, am->value.list[0]);
	int write_dest;
	if(decoded_dest->type == ast_number) {
		int write_dest = decoded_dest->value.num;
		if(!require_mem_write_access(bc, write_dest)) {
			return NULL;
		}
		env.memory[write_dest] = result->value.num;
		ast_delete(result);
		return NULL;
	} else if(decoded_dest->type == ast_in) {
		printf("get: This is impossible for 'in' to be an output parameter\n");
		return NULL;
	} else if(decoded_dest->type == ast_out || (decoded_dest->type == ast_literal && strcmp(decoded_dest->value.str, "out") ==0) ) {
		// wrrrrrr !!!! should fix this asap!
		if(!require_out_access(bc)) {
			return NULL;
		}
		ast_debug_print(result);
		ast_delete(result);
		return NULL;
	} else if(decoded_dest->type == ast_bind) {
		return result;
	}

	assert(0);
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

	// todo additional type for error!
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

struct ast *eval_pst(struct binding_context *bc) {
	int i = 0;
	/*if(require_out_access(bc)) {
		return NULL;
	}*/ // maybe error should always go
	printf("bc='%p', bc->e='%p'\n", bc, bc->e);
	while(bc && bc->e) {
		printf("#%d %s\n", i, bc->e->form->value.str);
		binding_context_print(bc, 0);
		bc = bc->parent, i++;
	}
}