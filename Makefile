all:
	bison -d parser.y
	flex lexer.l
	gcc -o main ast.c lex.yy.c expression.c binding_context.c eval.c restrict.c parser.tab.c -lfl
