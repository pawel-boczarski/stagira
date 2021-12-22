all:
	bison -d parser.y
	flex lexer.l
	gcc -o main lex.yy.c helpers.c eval.c parser.tab.c -lfl
