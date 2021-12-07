%{
#include "parser.tab.h"
#include "helpers.h"
//char yylval[64];

%}

%%
\r\n { }
[a-zA-Z]+ { yylval.a = ast_string_new(yytext); return LITERAL;}
[0-9]+ { yylval.a = ast_int_new(atoi(yytext)); return NUMBER; }
\[ { return L_SQ_BRA; }
\] { return R_SQ_BRA; }
\( { return L_PAREN; }
\) { return R_PAREN; }
\{ { return L_BRACE; }
\} { return R_BRACE; }
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
