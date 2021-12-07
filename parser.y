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
};

%token<a> LITERAL
%token<a> NUMBER
%token L_SQ_BRA
%token R_SQ_BRA
%token L_PAREN
%token R_PAREN
%token L_BRACE
%token R_BRACE
%token COLON
%token SEMICOLON
%token COMMA
%token UNRECOGNIZED

%type<a> term
//%type<a> termlist
%type<a> paren_termlist_start
%type<a> paren_termlist_progress
%type<a> paren_termlist

%%

stmt: command
    | stmt command
;

command: NUMBER SEMICOLON { /*printf("=> %d\n", $1);*/ ast_debug_print($1); }
    | LITERAL SEMICOLON { /*printf("=> '%s'\n", $1);*/ ast_debug_print($1); }
    | paren_termlist SEMICOLON { /*printf("'%s'\n", $1);*/ ast_debug_print($1); }
    
//    | pure_function_call SEMICOLON { printf("Pure function call\n"); }
//    | pure_procedure_call SEMICOLON { printf("Pure procedure call\n"); }
//    | procedure_call SEMICOLON { printf("Procedure call\n"); }
//    | termlist SEMICOLON { ast_debug_print($1); } // debug only
    
;

//pure_function_call: LITERAL paren_termlist
//;

//pure_procedure_call: LITERAL L_SQ_BRA termlist R_SQ_BRA
//;

//procedure_call: LITERAL L_SQ_BRA termlist R_SQ_BRA L_PAREN termlist R_PAREN
//;

term: LITERAL
    | NUMBER
    | paren_termlist
//    | pure_procedure_call
//    | procedure_call
;

paren_termlist_start: L_PAREN { $$ = ast_list_new(); }
;

paren_termlist_progress: paren_termlist_start term { $$ = ast_list_append($1, $2); }
                      |  paren_termlist_progress COMMA term { $$ = ast_list_append($1, $3); }
;

paren_termlist: paren_termlist_start R_PAREN { $$ = $1; }
             |  paren_termlist_progress R_PAREN { $$ = $1; }
;

%%

int main(int argc, char *argv[]) {
	yyparse();
}

int yyerror(char *s) {
	fprintf(stderr, "Error: '%s'", s);
	return 0;
}
