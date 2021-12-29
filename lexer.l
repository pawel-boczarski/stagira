%{
#include "parser.tab.h"
#include "ast.h"
//char yylval[64];

%}

%%
[ \t\r\n] { }
[a-zA-Z_]+ { yylval.a = ast_literal_new(yytext); return LITERAL;}
[0-9]+ { yylval.a = ast_int_new(atoi(yytext)); return NUMBER; }
\"[^"]*\" { yytext[strlen(yytext)-1] = '\0'; yylval.a = ast_string_new(yytext+1); return STRING; }
\[ { return L_SQ_BRA; }
\] { return R_SQ_BRA; }
\( { return L_PAREN; }
\) { return R_PAREN; }
\{ { return L_BRACE; }
\} { return R_BRACE; }
\< { return L_TR_BRA; }
\> { return R_TR_BRA; }
: { return COLON; }
; { return SEMICOLON; }
, { return COMMA; }
. { return UNRECOGNIZED; }

%%

#if 0
int main() {
	int n;
	while(n = yylex()) {
		printf("%d = %d\n", n, yylval);
		//yylval[0] = '\0';
	}
	return 0;
}
#endif
