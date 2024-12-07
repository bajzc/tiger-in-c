CC = gcc
CFLAGS = -g -O2 -Wall -D DEBUG=1 -D DEBUG2=0
#CFLAGS += -fsanitize=address
# CFLAGS += -fanalyzer

all: a.out

a.out: parse.o tiger.tab.o lex.yy.o errormsg.o util.o prabsyn.o absyn.o symbol.o table.o types.o semant.o env.o escape.o translate.o temp.o riscvframe.o tree.o printtree.o
	$(CC) $(CFLAGS) *.o

env.o: env.c
	$(CC) $(CFLAGS) -c env.c

types.o: types.c
	$(CC) $(CFLAGS) -c types.c

semant.o: semant.c types.c
	$(CC) $(CFLAGS) -c semant.c

parse.o: parse.c errormsg.h util.h parse.h tiger.tab.h
	$(CC) $(CFLAGS) -c parse.c

prabsyn.o: prabsyn.c absyn.h
	$(CC) $(CFLAGS) -c prabsyn.c

absyn.o: absyn.c tiger.tab.h
	$(CC) $(CFLAGS) -c absyn.c

symbol.o: symbol.c table.h
	$(CC) $(CFLAGS) -c symbol.c

table.o: table.c
	$(CC) $(CFLAGS) -c table.c

tiger.tab.o: tiger.tab.c
	$(CC) $(CFLAGS) -c tiger.tab.c

tiger.tab.c: tiger.y
	bison -dv tiger.y

tiger.tab.h: tiger.tab.c
	@echo "tiger.tab.h was created at the same time as tiger.tab.c"

errormsg.o: errormsg.c errormsg.h util.h
	$(CC) $(CFLAGS) -c errormsg.c

lex.yy.o: lex.yy.c tiger.tab.h errormsg.h util.h
	$(CC) $(CFLAGS) -c lex.yy.c

lex.yy.c: tiger.lex
	flex tiger.lex

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

escape.o: escape.c
	$(CC) $(CFLAGS) -c escape.c

translate.o: translate.c
	$(CC) $(CFLAGS) -c translate.c

temp.o: temp.c
	$(CC) $(CFLAGS) -c temp.c

riscvframe.o: riscvframe.c
	$(CC) $(CFLAGS) -c riscvframe.c

tree.o: tree.c
	$(CC) $(CFLAGS) -c tree.c

printtree.o: printtree.c
	$(CC) $(CFLAGS) -c printtree.c

clean: 
	rm -f a.out *.o lex.yy.c tiger.output tiger.tab.c tiger.tab.h
