/*
 * errormsg.c - functions used in all phases of the compiler to give
 *              error messages about the Tiger program.
 *
 */
#include "absyn.h"
#include "errormsg.h"
#include "util.h"

bool anyErrors = FALSE;

static string fileName = "";

extern OUT_TYPE yyin;

#ifndef XV6
/**
 * @brief print error message and quit
 * @param pos position of current token(s), updated by Bison
 * @param message error message to print
 * @param ... vargs
 */
_Noreturn void EM_error(A_pos pos, char *message, ...) {
  va_list ap;
  anyErrors = TRUE;

  fflush(stdout);
  fflush(stderr);

  fprintf(stderr, "ERROR:");
  if (fileName)
    fprintf(stderr, "%s:", fileName);
  if (pos.first_line != 0)
    fprintf(stderr, "%d.%d-%d.%d: ", pos.first_line, pos.first_column,
            pos.last_line, pos.last_column);
  va_start(ap, message);
  vfprintf(stderr, message, ap);
  va_end(ap);
  fprintf(stderr, "\n");
  exit(-1);
}
#else
void EM_error(A_pos pos, char *message, ...) {
  fprintf(stderr, "%s", message);
  exit(1);
}
#endif

/**
 * @brief redirect file descriptor of parser
 * @param fname file name(path)
 */
void EM_reset(string fname) {
  anyErrors = FALSE;
  fileName = fname;
  yyin = FOPEN_READ(fname);
  if (!yyin) {
    EM_error((A_pos) {0}, "cannot open");
    exit(1);
  }
}
