#ifndef __RESTRICT_H
#define __RESTRICT_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ast.h"
#include "binding_context.h"
#include "expression.h"
#include "eval.h"

int require_out_access(struct binding_context *bc);
int require_in_access(struct binding_context *bc);
int require_mem_write_access(struct binding_context *bc, int place);
int require_mem_read_access(struct binding_context *bc, int place);

#endif