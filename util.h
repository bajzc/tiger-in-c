#ifndef UTIL_H
#define UTIL_H
#include <assert.h>
#include <stdio.h>

#define debug(fmt, ...)                                                        \
  do {                                                                         \
    if (DEBUG)                                                                 \
      fprintf(stderr, "%s:%d: %s(): " fmt, __FILE__, __LINE__, __func__,       \
              ##__VA_ARGS__);                                                  \
  } while (0)

#define debug2(fmt, ...)                                                       \
  do {                                                                         \
    if (DEBUG2)                                                                \
      fprintf(stderr, "%s:%d: %s(): " fmt, __FILE__, __LINE__, __func__,       \
              ##__VA_ARGS__);                                                  \
  } while (0)

typedef char *string;
typedef char bool;

#define TRUE 1
#define FALSE 0

void *checked_malloc(int);
string String(char *);

typedef struct U_boolList_ *U_boolList;
struct U_boolList_ {
  bool head;
  U_boolList tail;
};
U_boolList U_BoolList(bool head, U_boolList tail);
#endif
