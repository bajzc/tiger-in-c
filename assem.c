/*
 * mipscodegen.c - Functions to translate to Assem-instructions for
 *             the Jouette assembly language using Maximal Munch.
 */

#include <stdio.h>

#include "absyn.h"
#include "assem.h"
#include "symbol.h"
#include "temp.h"
#include "util.h"

AS_targets AS_Targets(Temp_labelList labels) {
  AS_targets p = checked_malloc(sizeof *p);
  p->labels = labels;
  return p;
}

AS_instr AS_Oper(string a, Temp_tempList d, Temp_tempList s, AS_targets j) {
  AS_instr p = (AS_instr) checked_malloc(sizeof *p);
  p->kind = I_OPER;
  p->u.OPER.assem = a;
  p->u.OPER.dst = d;
  p->u.OPER.src = s;
  p->u.OPER.jumps = j;
  return p;
}

AS_instr AS_Label(string a, Temp_label label) {
  AS_instr p = (AS_instr) checked_malloc(sizeof *p);
  p->kind = I_LABEL;
  p->u.LABEL.assem = a;
  p->u.LABEL.label = label;
  return p;
}

AS_instr AS_Move(string a, Temp_tempList d, Temp_tempList s) {
  AS_instr p = (AS_instr) checked_malloc(sizeof *p);
  p->kind = I_MOVE;
  p->u.MOVE.assem = a;
  p->u.MOVE.dst = d;
  p->u.MOVE.src = s;
  return p;
}

AS_instrList AS_InstrList(AS_instr head, AS_instrList tail) {
  AS_instrList p = (AS_instrList) checked_malloc(sizeof *p);
  p->head = head;
  p->tail = tail;
  return p;
}

/* put list b at the end of list a */
AS_instrList AS_splice(AS_instrList a, AS_instrList b) {
  AS_instrList p;
  if (a == NULL)
    return b;
  for (p = a; p->tail != NULL; p = p->tail)
    ;
  p->tail = b;
  return a;
}

static Temp_temp nthTemp(Temp_tempList list, int i) {
  assert(list);
  if (i == 0)
    return list->head;
  else
    return nthTemp(list->tail, i - 1);
}

static Temp_label nthLabel(Temp_labelList list, int i) {
  assert(list);
  if (i == 0)
    return list->head;
  else
    return nthLabel(list->tail, i - 1);
}


/* first param is string created by this function by reading 'assem' string
 * and replacing `d `s and `j stuff.
 * Last param is function to use to determine what to do with each temp.
 */
static void format(char *result, string assem, Temp_tempList dst,
                   Temp_tempList src, AS_targets jumps, Temp_map m) {
  char *p;
  int i = 0; /* offset to result string */
  bool flag_first_space = TRUE;
  for (p = assem; p && *p != '\0'; p++) {
    if (*p == ' ' && flag_first_space) {
      flag_first_space = FALSE;
      while (i < 9)
        result[i++] = ' ';
    } else if (*p == '#') {
      while (i < 30)
        result[i++] = ' ';
      result[i++] = '#';
    } else if (*p == '`')
      switch (*(++p)) {
        case 's': {
          int n = ATOI(++p);
          string s = Temp_look(m, nthTemp(src, n));
          STRCPY(result + i, s);
          i += STRLEN(s);
        } break;
        case 'd': {
          int n = ATOI(++p);
          string s = Temp_look(m, nthTemp(dst, n));
          STRCPY(result + i, s);
          i += STRLEN(s);
        } break;
        case 'j':
          assert(jumps);
          {
            int n = ATOI(++p);
            string s = Temp_labelstring(nthLabel(jumps->labels, n));
            STRCPY(result + i, s);
            i += STRLEN(s);
          }
          break;
        case '`':
          result[i] = '`';
          i++;
          break;
        default: assert(0);
      }
    else {
      result[i] = *p;
      i++;
    }
  }
  result[i++] = '\n';
  result[i] = '\0';
}

static void format_graph(char *result, string assem, Temp_tempList dst,
                         Temp_tempList src, AS_targets jumps, Temp_map m) {
  format(result, assem, dst, src, jumps, m);
  int i = 0;
  while (result[i] != '\0') {
    if (result[i] == '#' || result[i] == '\n') {
      result[i] = '\0';
      break;
    }
    i++;
  }
}

void AS_print_graph(FILE *out, AS_instr i, Temp_map m) {
  char r[200]; /* result */
  switch (i->kind) {
    case I_OPER:
      format_graph(r, i->u.OPER.assem, i->u.OPER.dst, i->u.OPER.src,
                   i->u.OPER.jumps, m);
      fprintf(out, "%s", r);
      break;
    case I_LABEL:
      format_graph(r, i->u.LABEL.assem, NULL, NULL, NULL, m);
      fprintf(out, "%s", r);
      /* i->u.LABEL->label); */
      break;
    case I_MOVE:
      format_graph(r, i->u.MOVE.assem, i->u.MOVE.dst, i->u.MOVE.src, NULL, m);
      fprintf(out, "%s", r);
      break;
  }
}

void AS_print(FILE *out, AS_instr i, Temp_map m) {
  char r[200]; /* result */
  switch (i->kind) {
    case I_OPER:
      format(r, i->u.OPER.assem, i->u.OPER.dst, i->u.OPER.src, i->u.OPER.jumps,
             m);
      fprintf(out, "\t%s", r);
      break;
    case I_LABEL:
      format(r, i->u.LABEL.assem, NULL, NULL, NULL, m);
      fprintf(out, "%s", r);
      /* i->u.LABEL->label); */
      break;
    case I_MOVE:
      // if (Temp_look(m, i->u.MOVE.src->head) ==
      //     Temp_look(m, i->u.MOVE.dst->head)) { // redundant move filter
      //   assert(i->u.MOVE.src->tail == NULL && i->u.MOVE.dst->tail == NULL);
      //   break;
      // }
      format(r, i->u.MOVE.assem, i->u.MOVE.dst, i->u.MOVE.src, NULL, m);
      fprintf(out, "\t%s", r);
      break;
  }
}

/* c should be COL_color; temporarily it is not */
void AS_printInstrList(FILE *out, AS_instrList iList, Temp_map m) {
  for (; iList; iList = iList->tail) {
    AS_print(out, iList->head, m);
  }
  fprintf(out, "\n");
}

AS_proc AS_Proc(string p, AS_instrList b, string e) {
  AS_proc proc = checked_malloc(sizeof(*proc));
  proc->prolog = p;
  proc->body = b;
  proc->epilog = e;
  return proc;
}
