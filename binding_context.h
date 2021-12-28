#ifndef __BINDING_CONTEXT_H
#define __BINDING_CONTEXT_H

struct binding_context {
	struct binding_context *parent;
    struct expression *e;
	struct binding_table {
		char *name;
		struct ast *binding; // way waste of space...
	} *bindings;
	int bindings_size;
};

struct binding_context *binding_context_new(struct binding_context *parent, struct expression *e);
void binding_context_delete(struct binding_context *bc);
struct ast *binding_context_get_binding(struct binding_context *bc, char *name, int check_parents);
void binding_context_set_binding(struct binding_context *bc, char *name, struct ast *value);
void binding_context_print(struct binding_context *bc, int print_parents);

#endif