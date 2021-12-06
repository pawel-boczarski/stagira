%{
#include <stdio.h>
int yyerror(char *s);

int universe[1024];
%}

%union {
   int i;
   char s[64];
};

%token<s> LITERAL
%token<i> NUMBER
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

%%

stmt: command
    | stmt command
;

command: NUMBER SEMICOLON { printf("=> %d\n", $1); }
    | LITERAL SEMICOLON { printf("=> '%s'\n", $1); }
    | pure_function_call SEMICOLON { printf("Pure function call\n"); }
    | pure_procedure_call SEMICOLON { printf("Pure procedure call\n"); }
    | procedure_call SEMICOLON { printf("Procedure call\n"); }
    
;

pure_function_call: LITERAL L_PAREN termlist R_PAREN
;

pure_procedure_call: LITERAL L_SQ_BRA termlist R_SQ_BRA
;

procedure_call: LITERAL L_SQ_BRA termlist R_SQ_BRA L_PAREN termlist R_PAREN
;

term: LITERAL
    | NUMBER
    | pure_function_call
    | pure_procedure_call
    | procedure_call
;

termlist:  term
      | termlist COMMA term;
;

%%

int main(int argc, char *argv[]) {
	yyparse();
}

int yyerror(char *s) {
	fprintf(stderr, "Error: '%s'", s);
	return 0;
}
