CC = gcc
CFLAGS ?= -g -O2 -Wall -D DEBUG=0 -D DEBUG2=0
#CFLAGS += -fsanitize=address
#CFLAGS += -fanalyzer

BUILD_DIR = ./out

SRC_FILES = $(wildcard ./*.c) ./tiger.tab.c ./lex.yy.c
SRC_FILES := $(sort $(SRC_FILES))
SRC_FILES := $(filter-out ./xv6_tiger.tab.c ./xv6_lex.yy.c, $(SRC_FILES))
OBJ_FILES = $(patsubst ./%.c, $(BUILD_DIR)/%.o, $(SRC_FILES))

XV6_SRC_FILES = $(wildcard ./*.c) ./xv6_tiger.tab.c ./xv6_lex.yy.c
XV6_SRC_FILES := $(sort $(XV6_SRC_FILES))
XV6_SRC_FILES := $(filter-out ./tiger.tab.c ./lex.yy.c, $(XV6_SRC_FILES))
XV6_OBJ_FILES = $(patsubst ./%.c, $(BUILD_DIR)/%.o, $(XV6_SRC_FILES))

all: create_dir tiger.tab.c lex.yy.c a.out

objs: create_dir xv6_tiger.tab.c xv6_lex.yy.c $(XV6_OBJ_FILES)

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
