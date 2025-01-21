/*
 * util.c - commonly used utility functions.
 */

#include "util.h"

int errno;

void *checked_malloc(int len) {
  void *p = malloc(len);
  if (!p) {
    fprintf(stderr, "\nRan out of memory!\n");
    exit(1);
  }
  return p;
}

string String(char *s) {
  string p = checked_malloc(STRLEN(s) + 1);
  STRCPY(p, s);
  return p;
}

U_boolList U_BoolList(bool head, U_boolList tail) {
  U_boolList list = checked_malloc(sizeof(*list));
  list->head = head;
  list->tail = tail;
  return list;
}
