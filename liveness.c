#include "liveness.h"
#include "flowgraph.h"
#include "frame.h"
#include "graph.h"
#include "set.h"
#include "table.h"

static G_table in_map, out_map;

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
  in_map = G_empty(), out_map = G_empty();
  for (G_nodeList l = G_nodes(flow); l; l = l->tail) {
    G_node n = l->head;
    enterLiveMap(in_map, n, SET_empty(SET_default_cmp));
    enterLiveMap(out_map, n, SET_empty(SET_default_cmp));
  }

  bool changed = FALSE;
  while (!changed) {
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
      if (SET_iseq(in_, in) != 1 || SET_iseq(out_, out) != 1) {
        changed = TRUE;
      }
    }
  }

  return (struct Live_graph) {in_map, out_map};
}
