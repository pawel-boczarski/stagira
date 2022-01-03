#ifndef __EVAL_H
#define __EVAL_H

struct ast* eval_queue(struct expression *e);

struct ast* eval_now(struct binding_context *bc);

struct ast* eval_print(struct binding_context *bc);
struct ast* eval_get(struct binding_context *bc);
struct ast* eval_set(struct binding_context *bc);

struct ast* eval_op(struct binding_context *bc);


struct ast* eval_seq(struct binding_context *bc);
struct ast* eval_if(struct binding_context *bc);
struct ast* eval_cond(struct binding_context *bc);
struct ast* eval_loop(struct binding_context *bc);

// print stack trace
struct ast* eval_pst(struct binding_context *bc);


#endif