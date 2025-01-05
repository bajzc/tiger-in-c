/*
 * main.c
 */

#include <stdio.h>

#include "absyn.h"
#include "assem.h"
#include "canon.h"
#include "codegen.h"
#include "errormsg.h"
#include "escape.h"
#include "flowgraph.h"
#include "frame.h" /* needed by translate.h and printfrags prototype */
#include "graph.h"
#include "parse.h"
#include "prabsyn.h"
#include "printtree.h"
#include "semant.h" /* function prototype for transProg */
#include "symbol.h"
#include "temp.h" /* needed by translate.h */
#include "tree.h" /* needed by frame.h */
#include "util.h"
#include "set.h"


extern bool anyErrors;

static void printFlowgraph(FILE *out, G_graph graph, Temp_map m) {
  G_nodeList nodes = G_nodes(graph);
  while (nodes) {
    AS_instr instr = G_nodeInfo(nodes->head);
    AS_print(out, instr, m);

    G_nodeList adj = G_succ(nodes->head);
    while (adj) {
      AS_instr adj_instr = G_nodeInfo(adj->head);
      fprintf(out, " -> ");
      AS_print(out, adj_instr, m);
      adj = adj->tail;
    }
    fprintf(out, "\n");

    nodes = nodes->tail;
  }
}

/* print the assembly language instructions to filename.s */
static void doProc(FILE *out, F_frame frame, T_stm body) {
  AS_proc proc;
  // struct RA_result allocation;
  T_stmList stmList;
  AS_instrList iList;

  stmList = C_linearize(body);
  stmList = C_traceSchedule(C_basicBlocks(stmList));
  /* printStmList(stdout, stmList);*/
  iList = F_codegen(frame, stmList); /* 9 */
  iList = F_procEntryExit2(iList);
  proc = F_procEntryExit3(frame,iList);

  fprintf(out, "BEGIN %s\n", Temp_labelstring(F_name(frame)));
  Temp_map m = Temp_layerMap(F_tempMap, Temp_name());
  AS_printInstrList(out, proc->body, m);
  fprintf(out, "END %s\n\n", Temp_labelstring(F_name(frame)));
  G_graph graph = FG_AssemFlowGraph(iList);
  printFlowgraph(stdout, graph, m);
}

int main(int argc, string *argv) {
  A_exp absyn_root;
  S_table base_env, base_tenv;
  F_fragList frags;
  char outfile[100];
  FILE *out = stdout;

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
    /* Chapter 8, 9, 10, 11 & 12 */
    for (; frags; frags = frags->tail) {
      if (frags->head->kind == F_procFrag)
        doProc(out, frags->head->u.proc.frame, frags->head->u.proc.body);
      else if (frags->head->kind == F_stringFrag)
        fprintf(out, "%s\n", frags->head->u.stringg.str);

    }

    fclose(out);
    return 0;
  }
  EM_error((A_pos) {0}, "usage: tiger file.tig");
  return 1;
}
