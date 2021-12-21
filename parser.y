%{
#include <stdio.h>
#include "helpers.h"
int yyerror(char *s);

int universe[1024];
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


%type<e> pure_function_call
%type<e> pure_procedure_call
%type<e> procedure_call

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
    | simple_call
;


simple_call: pure_function_call SEMICOLON { expression_print($1); }
    | pure_procedure_call SEMICOLON { expression_print($1); }
    | procedure_call SEMICOLON { expression_print($1); }
;

// pure... call => unrestricted_call (one token type)
// then restricted_call with restrictions
// and sensible_expression with both actualization_mode and goal

pure_function_call: LITERAL paren_termlist { ast_debug_print($2); $$ = expression_new(NULL, NULL, NULL, NULL, $1, NULL, $2, NULL); }
;

pure_procedure_call: LITERAL sq_termlist { ast_debug_print($2); $$ = expression_new(NULL, NULL, NULL, NULL, $1, $2, NULL, NULL); }
;

procedure_call: LITERAL sq_termlist paren_termlist { ast_debug_print($2); $$ = expression_new(NULL, NULL, NULL, NULL, $1, $2, $3, NULL); }
;

term: LITERAL
    | STRING
    | NUMBER
    | paren_termlist
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
