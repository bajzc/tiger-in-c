CC = gcc
CFLAGS = -g -O0 -Wall -D DEBUG=1 -D DEBUG2=1
#CFLAGS += -fsanitize=address
#CFLAGS += -fanalyzer

BUILD_DIR = ./out

SRC_FILES = $(wildcard ./*.c) ./tiger.tab.c ./lex.yy.c
SRC_FILES := $(sort $(SRC_FILES))
OBJ_FILES = $(patsubst ./%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

all: create_dir tiger.tab.c lex.yy.c a.out

create_dir:
	mkdir -p $(BUILD_DIR)

a.out: $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES)

$(BUILD_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

tiger.tab.c: tiger.y
	bison -d tiger.y

lex.yy.c: tiger.lex
	flex tiger.lex

clean: 
	rm -fr $(BUILD_DIR) lex.yy.c tiger.tab.c tiger.tab.h a.out *.dot *.s

.PHONY: clean create_dir