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

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#ifndef __STDC__
// port to XV6
#include "user.h"
#define STRCPY strcpy
#define STRLEN strlen
#define STRCMP strcmp
#define MEMCPY memcpy
#define ATOI atoi

#define STRDUP(s)                                                              \
  ({                                                                           \
    const char *_src = (s);                                                    \
    size_t _len = strlen(_src) + 1;                                            \
    char *_dest = (char *) malloc(_len);                                       \
    if (_dest)                                                                 \
      memcpy(_dest, _src, _len);                                               \
    _dest;                                                                     \
  })
#else
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#define STRCPY strcpy
#define STRDUP strdup
#define STRLEN strlen
#define STRCMP strcmp
#define MEMCPY memcpy
#define ATOI atoi
#endif

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
