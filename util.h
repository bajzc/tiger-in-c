#ifndef UTIL_H
#define UTIL_H

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

#define MAX(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a > _b ? _a : _b;                                                         \
  })
#define MIN(a, b)                                                              \
  ({                                                                           \
    __typeof__(a) _a = (a);                                                    \
    __typeof__(b) _b = (b);                                                    \
    _a < _b ? _a : _b;                                                         \
  })

#ifdef XV6
// port to XV6
#include "kernel/types.h"
#include "user/user.h"
#define STRCPY strcpy
#define STRLEN strlen
#define STRCMP strcmp
#define MEMCPY memcpy
#define ATOI atoi
extern int errno;
#define errno errno
typedef uint64 size_t;
#define stdout 1
#define stderr 2
#define EOF -1
#define NULL (void *) 0
#define STRDUP(s)                                                              \
  ({                                                                           \
    const char *_src = (s);                                                    \
    size_t _len = strlen(_src) + 1;                                            \
    char *_dest = (char *) malloc(_len);                                       \
    if (_dest)                                                                 \
      memcpy(_dest, _src, _len);                                               \
    _dest;                                                                     \
  })

#define assert(e) (e)
#define OUT_TYPE int
#define FOPEN_WRITE(path) open(path,O_WRONLY|O_CREATE) 
#define FCLOSE(fd) close(fd)
#else
#include <assert.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define STRCPY strcpy
#define STRDUP strdup
#define STRLEN strlen
#define STRCMP strcmp
#define MEMCPY memcpy
#define ATOI atoi
#define OUT_TYPE FILE*
#define FOPEN_WRITE(path) fopen(path,"w")
#define FCLOSE(fd) fclose(fd)
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
