/*
 * main.c
 */
#include "absyn.h"
#include "assem.h"
#include "canon.h"
#include "codegen.h"
#include "color.h"
#include "errormsg.h"
#include "escape.h"
#include "flowgraph.h"
#include "frame.h" /* needed by translate.h and printfrags prototype */
#include "graph.h"
#include "liveness.h"
#include "parse.h"
#include "prabsyn.h"
#include "printtree.h"
#include "semant.h" /* function prototype for transProg */
#include "set.h"
#include "symbol.h"
#include "temp.h" /* needed by translate.h */
#include "tree.h" /* needed by frame.h */
#include "util.h"

extern bool anyErrors;

static void printInterGraph(OUT_TYPE out, G_graph graph) {
  fprintf(out, "strict graph {\n");
  G_nodeList nodes = G_nodes(graph);
  while (nodes) {
    Temp_temp instr = Live_gtemp(nodes->head);
    G_nodeList adj = G_succ(nodes->head);
    while (adj) {
      Temp_temp adj_instr = Live_gtemp(adj->head);
      fprintf(out, "t%d -- t%d;\n", instr->num, adj_instr->num);
      adj = adj->tail;
    }
    nodes = nodes->tail;
  }
  fprintf(out, "}\n");
}

/* print the assembly language instructions to filename.s */
static void doProc(OUT_TYPE out, char *outfile, F_frame frame, T_stm body) {
  AS_proc proc;
  // struct RA_result allocation;
  T_stmList stmList = NULL;
  AS_instrList iList = NULL;

  stmList = C_linearize(body);
  stmList = C_traceSchedule(C_basicBlocks(stmList));
  Set last_instr = SET_empty(SET_default_cmp);
  /* printStmList(stdout, stmList);*/
  iList = F_codegen(last_instr, frame, stmList); /* 9 */
  iList = F_procEntryExit2(iList, frame, last_instr);

  // add the last instruction appended in F_procEntryExit2 to last_instr
   AS_instrList p;
  for (p = iList; p->tail != NULL; p = p->tail)
    ;
  SET_insert(last_instr, p->head);

  // G_graph graph = FG_AssemFlowGraph(iList);
  // printFlowgraph(stderr, graph, Temp_layerMap(F_tempMap, Temp_name()),
  //                Temp_labelstring(F_name(frame)));

  // AS_printInstrList(out, iList, Temp_layerMap(F_tempMap, Temp_name()));

  Temp_map color_map = Color_Main(last_instr, iList, frame);
  proc = F_procEntryExit3(frame, iList);


  fprintf(out, ".text\n%s:\n", Temp_labelstring(F_name(frame)));
  AS_printInstrList(out, proc->body, color_map);
  // fprintf(out, "END %s\n\n", Temp_labelstring(F_name(frame)));
  //
  // G_graph inter_graph = Live_Liveness(graph).graph;
  //
  // char graph_file[100];
  // snprintf(graph_file, 100, "%s-%s.dot", outfile,
  //          Temp_labelstring(F_name(frame)));
  // FILE *graph_out = fopen(graph_file, "w");
  // printInterGraph(graph_out, inter_graph);
  // fclose(graph_out);
}

char *escape(const char *input) {
  if (!input)
    return NULL;

  size_t len = 0;
  const char *ptr = input;
  while (*ptr) {
    switch (*ptr) {
      case '\n':
      case '\t':
      case '\\':
      case '\"':
      case '\'':
      case '\r':
      case '\0': len += 2; break;
      default: len++; break;
    }
    ptr++;
  }

  char *output = (char *) malloc(len + 1);
  if (!output)
    return NULL;

  char *dest = output;
  ptr = input;
  while (*ptr) {
    switch (*ptr) {
      case '\n':
        *dest++ = '\\';
        *dest++ = 'n';
        break;
      case '\t':
        *dest++ = '\\';
        *dest++ = 't';
        break;
      case '\\':
        *dest++ = '\\';
        *dest++ = '\\';
        break;
      case '\"':
        *dest++ = '\\';
        *dest++ = '\"';
        break;
      case '\'':
        *dest++ = '\\';
        *dest++ = '\'';
        break;
      case '\r':
        *dest++ = '\\';
        *dest++ = 'r';
        break;
      case '\0':
        *dest++ = '\\';
        *dest++ = '0';
        break;
      default: *dest++ = *ptr; break;
    }
    ptr++;
  }
  *dest = '\0';

  return output;
}

static void doString(OUT_TYPE out, Temp_label lab, char *str) {
  fprintf(out, ".data\n");
  fprintf(out, "  %s:\n", Temp_labelstring(lab));
  fprintf(out, "    .word %lu\n", STRLEN(str));
  fprintf(out, "    .asciz \"%s\"\n", escape(str));
}

int main(int argc, string *argv) {
  A_exp absyn_root;
  F_fragList frags;
  char outfile[100];
  OUT_TYPE out = stdout;

  if (argc == 2) {
    absyn_root = parse(argv[1]);
    if (!absyn_root)
      return 1;

#if DEBUG2
    pr_exp(out, absyn_root, 0); /* print absyn data structure */
    fprintf(out, "\n");
#endif

    Esc_findEscape(absyn_root); /* set varDec's escape field */

    frags = SEM_transProg(absyn_root);
    if (anyErrors)
      return 1; /* don't continue */

    /* convert the filename */
    sprintf(outfile, "%s.s", argv[1]);
    out = fopen(outfile, "w");
    fprintf(out, ".global\t_start\n");
    /* Chapter 8, 9, 10, 11 & 12 */
    for (; frags; frags = frags->tail) {
      if (frags->head->kind == F_procFrag)
        doProc(out, argv[1], frags->head->u.proc.frame,
               frags->head->u.proc.body);
      else if (frags->head->kind == F_stringFrag)
        doString(out, frags->head->u.stringg.label, frags->head->u.stringg.str);
    }

    fclose(out);
    return 0;
  }
  EM_error((A_pos) {0}, "usage: tiger file.tig");
  return 1;
}
