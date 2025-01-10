#include "liveness.h"
#include "flowgraph.h"
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

// calculate in{} and out{}
bool calcLiveness(G_nodeList l) {
  if (l == NULL)
    return TRUE;
  bool no_change = calcLiveness(l->tail);
  G_node n = l->head;
  Set in = lookupLiveMap(in_map, n);
  Set out = lookupLiveMap(out_map, n);
  Set out_ = SET_copy(out);
  Set in_ = SET_union(FG_use(n), SET_difference(out_, FG_def(n)));

  for (G_nodeList succs = G_succ(n); succs; succs = succs->tail) {
    out_ = SET_union(out_, lookupLiveMap(in_map, succs->head));
  }
  enterLiveMap(in_map, n, in_);
  enterLiveMap(out_map, n, out_);

  no_change &= SET_eq(in, in_) && SET_eq(out, out_);
  return no_change;
}

struct Live_graph Live_Liveness(G_graph flow) {
  in_map = G_empty(), out_map = G_empty();
  Set temps = SET_empty(SET_default_cmp);
  for (G_nodeList l = G_nodes(flow); l; l = l->tail) {
    G_node n = l->head;
    enterLiveMap(in_map, n, SET_empty(SET_default_cmp));
    enterLiveMap(out_map, n, SET_empty(SET_default_cmp));
    temps = SET_union(temps, FG_def(n));
    temps = SET_union(temps, FG_use(n));
  }

  while (!calcLiveness(G_nodes(flow)))
    ;

  G_graph intgraph = G_Graph();
  TAB_table tab = TAB_empty();
  Live_moveList ll = NULL;

  for (void **p = SET_begin(temps); p < SET_end(temps); p++) {
    Temp_temp d = *p;
    G_node n = G_Node(intgraph, d);
    TAB_enter(tab, d, n);
  }

  for (G_nodeList l = G_nodes(flow); l; l = l->tail) {
    G_node n = l->head;
    AS_instr instr = G_nodeInfo(n);
    Set defs = FG_def(n);
    if (!SET_isEmpty(defs)) {
      for (void **def = SET_begin(defs); def < SET_end(defs); def++) {
        Temp_temp d = *def;
        Set lives = lookupLiveMap(in_map, n);
        for (void **live = SET_begin(lives); live < SET_end(lives); live++) {
          G_node d_node = TAB_look(tab, d);
          G_node live_node = TAB_look(tab, *live);

          if (instr->kind == I_MOVE)
            ll = Live_MoveList(live_node, d_node, ll);

          if (d_node == live_node || G_goesTo(d_node, live_node) ||
              (instr->kind == I_MOVE && instr->u.MOVE.src->head == *live))
            continue;

          G_addBiEdge(d_node, live_node);
        }
      }
    }
  }
  return (struct Live_graph) {intgraph, ll, tab, in_map, out_map};
}
