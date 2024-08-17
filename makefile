a.out: parsetest.o tiger.tab.o lex.yy.o errormsg.o util.o
	cc -g parsetest.o tiger.tab.o lex.yy.o errormsg.o util.o

parsetest.o: parsetest.c errormsg.h util.h
	cc -g -c parsetest.c

tiger.tab.o: tiger.tab.c
	cc -g -c tiger.tab.c -o tiger.tab.o

tiger.tab.c: tiger.y
	bison -dvt tiger.y

tiger.tab.h: tiger.tab.c
	echo OK

errormsg.o: errormsg.c errormsg.h util.h
	cc -g -c errormsg.c

lex.yy.o: lex.yy.c tiger.tab.h errormsg.h util.h
	cc -g -c lex.yy.c 

lex.yy.c: tiger.lex
	lex tiger.lex

util.o: util.c util.h
	cc -g -c util.c

clean: 
	rm -f a.out util.o parsetest.o lex.yy.o errormsg.o tiger.tab.o tiger.tab.c lex.yy.c tiger.tab.h tiger.output
