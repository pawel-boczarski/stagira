%{
#include <stdio.h>
#include "ast.h"
#include "expression.h"
#include "binding_context.h"

int yyerror(char *s);

void eval_queue(struct expression *e);
%}

%union {
   struct ast *a;
   int i;
   char s[64];
   struct expression *e;
};

%token<a> LITERAL
%token<a> STRING
%token<a> NUMBER
%token L_SQ_BRA
%token R_SQ_BRA
%token L_TR_BRA
%token R_TR_BRA
%token L_PAREN
%token R_PAREN
%token L_BRACE
%token R_BRACE
%token COLON
%token SEMICOLON
%token COMMA
%token UNRECOGNIZED

%type<a> term
%type<a> paren_termlist_start
%type<a> paren_termlist_progress
%type<a> paren_termlist

%type<a> sq_termlist_start
%type<a> sq_termlist_progress
%type<a> sq_termlist

%type<a> tr_termlist_start
%type<a> tr_termlist_progress
%type<a> tr_termlist

%type<a> unrestricted_call
%type<a> restricted_call
%type<a> perfect_call
%type<a> named_perfect_call

%type<a> pure_function_call
%type<a> pure_procedure_call
%type<a> procedure_call


%%

stmt: command
    | stmt command
;

command: NUMBER SEMICOLON { ast_debug_print($1); }
    | LITERAL SEMICOLON { ast_debug_print($1); }
    | STRING SEMICOLON { ast_debug_print($1); }
    | paren_termlist SEMICOLON { ast_debug_print($1); }
    | sq_termlist SEMICOLON { ast_debug_print($1); }
    | tr_termlist SEMICOLON { ast_debug_print($1); }
    | unrestricted_call SEMICOLON { /*eval_queue($1); printf("Unrestricted call - skipping...");*/ }
    | restricted_call SEMICOLON { /*eval_queue($1); printf("Imperfect call - skipping...");*/ }
    | perfect_call SEMICOLON { eval_queue($1->value.e); }
    | named_perfect_call SEMICOLON { eval_queue($1->value.e); }
;

named_perfect_call: perfect_call COLON STRING { $$ = $1; $$->value.e->name = $3->value.str; } 
;

perfect_call: LITERAL STRING restricted_call { $$ = $3; $$->value.e->act_mode = $1->value.str; $$->value.e->goal = $2->value.str; }
;

restricted_call: paren_termlist unrestricted_call { $$ = $2; $$->value.e->species = $1; }
               | sq_termlist unrestricted_call { $$ = $2; $$->value.e->matter = $1; }
               | sq_termlist paren_termlist unrestricted_call { $$ = $3; $$->value.e->matter = $1; $$->value.e->species = $2; }
;

unrestricted_call: pure_function_call
                | pure_procedure_call
                | procedure_call
;

pure_function_call: LITERAL paren_termlist { struct expression *e = accidental_expression_new($1, NULL, $2);
                                             $$ = ast_expression_new(e); }
;

pure_procedure_call: LITERAL sq_termlist { struct expression *e = accidental_expression_new($1, $2, NULL);
                                            $$ = ast_expression_new(e); }
;

procedure_call: LITERAL sq_termlist paren_termlist { struct expression *e = accidental_expression_new($1, $2, $3);
                                                     $$ = ast_expression_new(e); }
;

term: LITERAL
    | STRING
    | NUMBER
    | paren_termlist
    | unrestricted_call
;

paren_termlist_start: L_PAREN { $$ = ast_list_new(); }
;

paren_termlist_progress: paren_termlist_start term { $$ = ast_list_append($1, $2); }
                      |  paren_termlist_progress COMMA term { $$ = ast_list_append($1, $3); }
;

paren_termlist: paren_termlist_start R_PAREN { $$ = $1; }
             |  paren_termlist_progress R_PAREN { $$ = $1; }
;

sq_termlist_start: L_SQ_BRA { $$ = ast_list_new(); }
;

sq_termlist_progress: sq_termlist_start term { $$ = ast_list_append($1, $2); }
                      |  sq_termlist_progress COMMA term { $$ = ast_list_append($1, $3); }
;

sq_termlist: sq_termlist_start R_SQ_BRA { $$ = $1; }
             |  sq_termlist_progress R_SQ_BRA { $$ = $1; }
;

tr_termlist_start: L_TR_BRA { $$ = ast_list_new(); }
;

tr_termlist_progress: tr_termlist_start term { $$ = ast_list_append($1, $2); }
                      |  tr_termlist_progress COMMA term { $$ = ast_list_append($1, $3); }
;

tr_termlist: tr_termlist_start R_TR_BRA { $$ = $1; }
             |  tr_termlist_progress R_TR_BRA { $$ = $1; }
;


%%

int main(int argc, char *argv[]) {
	yyparse();
}

int yyerror(char *s) {
	fprintf(stderr, "Error: '%s'", s);
	return 0;
}
