a.out: parse.o tiger.tab.o lex.yy.o errormsg.o util.o prabsyn.o absyn.o symbol.o table.o
	cc -g parse.o tiger.tab.o lex.yy.o errormsg.o util.o prabsyn.o absyn.o symbol.o table.o

parse.o: parse.c errormsg.h util.h parse.h tiger.tab.h
	cc -g -c parse.c

prabsyn.o: prabsyn.c absyn.h
	cc -g -c prabsyn.c

absyn.o: absyn.c tiger.tab.h
	cc -g -c absyn.c

symbol.o: symbol.c table.h
	cc -g -c symbol.c

table.o: table.c
	cc -g -c table.c

tiger.tab.o: tiger.tab.c
	cc -g -c tiger.tab.c

tiger.tab.c: tiger.y
	bison -dv tiger.y

tiger.tab.h: tiger.tab.c
	@echo "tiger.tab.h was created at the same time as tiger.tab.c"

errormsg.o: errormsg.c errormsg.h util.h
	cc -g -c errormsg.c

lex.yy.o: lex.yy.c tiger.tab.h errormsg.h util.h
	cc -g -c lex.yy.c

lex.yy.c: tiger.lex
	flex tiger.lex

util.o: util.c util.h
	cc -g -c util.c

clean: 
	rm -f a.out *.o lex.yy.c tiger.output tiger.tab.c tiger.tab.h
