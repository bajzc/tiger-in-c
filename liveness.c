#include "liveness.h"
#include "flowgraph.h"
#include "frame.h"
#include "graph.h"
#include "set.h"
#include "table.h"


static void enterLiveMap(G_table t, G_node flowNode, Set temps) {
  G_enter(t, flowNode, temps);
}

static Set lookupLiveMap(G_table t, G_node flowNode) {
  return G_look(t, flowNode);
}

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail) {
  Live_moveList t = checked_malloc(sizeof(*t));
  t->src = src;
  t->dst = dst;
  t->tail = tail;
  return t;
}
Temp_temp Live_gtemp(G_node n) { return (Temp_temp) G_nodeInfo(n); }

struct Live_graph Live_Liveness(G_graph flow) {
  G_table in_map, out_map;
  in_map = G_empty(), out_map = G_empty();
  for (G_nodeList l = G_nodes(flow); l; l = l->tail) {
    G_node n = l->head;
    enterLiveMap(in_map, n, SET_empty(Temp_temp_cmp));
    enterLiveMap(out_map, n, SET_empty(Temp_temp_cmp));
  }

  bool changed = TRUE;
  while (changed) {
    changed = FALSE;
    for (G_nodeList l = G_nodes(flow); l; l = l->tail) {
      G_node n = l->head;
      Set in_ = lookupLiveMap(in_map, n); // copy
      Set out_ = lookupLiveMap(out_map, n); // copy
      Set out = SET_copy(out_);
      Set in = SET_union(FG_use(n), SET_difference(out, FG_def(n)));
      enterLiveMap(in_map, n, in);
      for (G_nodeList s = G_succ(n); s; s = s->tail) {
        out = SET_union(out, lookupLiveMap(in_map, s->head));
      }
      if (!(SET_iseq(in_, in) && SET_iseq(out_, out))) {
        changed = TRUE;
      }
      enterLiveMap(in_map, n, in);
      enterLiveMap(out_map, n, out);
    }
  }
#if DEBUG
  for (G_nodeList l = G_nodes(flow); l; l = l->tail) {
    Set in = lookupLiveMap(in_map, l->head);
    Set out = lookupLiveMap(out_map, l->head);
    AS_instr instr = G_nodeInfo(l->head);
    fprintf(stderr, "%s\n\tin = ",
            instr->u.LABEL.assem); // the string will always be there
    SET_FOREACH(in, tptr) {
      Temp_temp t = *tptr;
      if (!Temp_look(F_tempMap, t))
        fprintf(stderr, "%d, ", ((Temp_temp) *tptr)->num);
    }
    fprintf(stderr, "\n\tout = ");
    SET_FOREACH(out, tptr) {
      Temp_temp t = *tptr;
      if (!Temp_look(F_tempMap, t))
        fprintf(stderr, "%d, ", ((Temp_temp) *tptr)->num);
    }
    fprintf(stderr, "\n");
    for (G_nodeList succL = G_succ(l->head); succL; succL = succL->tail) {
      AS_instr child = G_nodeInfo(succL->head);
      fprintf(stderr, "\tsucc: %s\n", child->u.LABEL.assem);
    }
    fprintf(stderr, "\n");
  }
#endif

  return (struct Live_graph) {out_map, in_map};
}
