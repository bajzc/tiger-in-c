#include "color.h"

#include "codegen.h"
#include "flowgraph.h"
#include "frame.h"
#include "graph.h"
#include "liveness.h"
#include "set.h"

extern Temp_map F_tempMap;

typedef struct Main_struct_ {
  G_table liveOut, liveIn; // Map<G_Node<AS_instr>, Set<G_Node<Temp_temp>>>
  G_table moveList; // Map<G_Node<Temp_temp>, Set<AS_instr>>
  G_table alias; // Map<G_Node, G_Node>
  Set stmt_instr_set;
  G_nodeList block_end_list; // a.k.a blocks
  Set simplifyWorklist; // Set<G_Node<Temp_temp>>
  Set worklistMoves; // Set<AS_instr>
  Set freezeWorklist; // Set<>
  Set spillWorklist; // Set<>
  Set spilledNodes; // Set<>
  Set coalescedNodes; // Set<>
  G_graph flowgraph;
  struct Live_graph *live_graph;

  // G_table adjList; // Map<G_Node<Temp_temp>, <G_Node<Temp_temp>>>
  // G_table degree; // Map<G_Node<Temp_temp>, int>
  Set adjSet; // Set<(G_node, G_node)>
  Set precolored; // Set<G_node>
} *Main_struct;

typedef struct edge_ {
  G_node source;
  G_node target;
} *edge;

edge Edge(G_node source, G_node target) {
  edge e = checked_malloc(sizeof(*e));
  e->source = source;
  e->target = target;
  return e;
}

// We need special comparer for tuple, because we need to construct a tuple to check existence
int edge_cmp(void* a, void* b) {
  edge ea = a;
  edge eb = b;
  if (ea->source == eb->source)
    return ea->target - eb->target;
  return ea->source - eb->source;
}

void setup(Main_struct S) {
  S->adjSet = SET_empty(edge_cmp);
}


/*
 * generate block_end_list for every instr in stmt_isntr_set, find the node in
 * the flowgraph that has the instr, add it to the block_end_list
 */

void buildupLiveOut(Main_struct S) {
  S->block_end_list = NULL;
  for (void **instr = SET_begin(S->stmt_instr_set);
       instr < SET_end(S->stmt_instr_set); instr++) {
    AS_instr last = ((struct stmt_instr *) *instr)->last;
    for (G_nodeList node = G_nodes(S->flowgraph); node; node = node->tail) {
      if (G_nodeInfo(node->head) == last) {
        S->block_end_list = G_NodeList(node->head, S->block_end_list);
        break;
      }
    }
  }
}

// worklistMoves: moves enabled for possible coalescing
int isBlockStart(Main_struct S, G_node node) {
  G_nodeList preds = G_pred(node);
  return preds->head == NULL || G_inNodeList(preds->head, S->block_end_list);
}

/*
procedure Build ()
  forall b ∈ blocks in program
    let live = liveOut(b)
    forall I ∈ instructions(b) in reverse order
      if isMoveInstruction(I) then
        live ← live \ use(I)
        forall n ∈ def(I) ∪ use(I)
          moveList[n] ← moveList[n] ∪ {I}
        worklistMoves ← worklistMoves ∪ {I}
      live ← live ∪ def(I)
      forall d ∈ def(I)
        forall l ∈ live
          AddEdge(l, d)
      live ← use(I) ∪ (live \ def(I))
*/
void build(Main_struct S) {
  buildupLiveOut(S);
  G_nodeList cur_block_end_list = S->block_end_list;
  while (cur_block_end_list) { // forall b ∈ blocks in program
    G_node cur_node = cur_block_end_list->head;
    Set live = G_look(S->liveOut, cur_node); // let live = liveOut(b)
    while (cur_node &&
           !isBlockStart(S, cur_node)) { // forall I ∈ instructions(b)
      AS_instr cur_instr = G_nodeInfo(cur_node);
      if (FG_isMove(cur_node)) { // if isMoveInstruction(I) then
        live = SET_difference(live, FG_use(cur_node)); // live ← live \ use(I)
        Set defAndUse = SET_union(FG_use(cur_node), FG_def(cur_node));
        for (void **nptr = SET_begin(defAndUse); nptr < SET_end(defAndUse);
             nptr++) { // forall n ∈ def(I) ∪ use(I)
          Temp_temp n = *nptr;
          G_node node = G_findNodeWithInfo(S->live_graph->graph, n);
          Set moveList = G_look(S->moveList, node);
          if (moveList == NULL)
            G_enter(S->moveList, node, moveList = SET_empty(SET_default_cmp));
          SET_insert(moveList, cur_instr); // moveList[n] ← moveList[n] ∪ {I}
        }
        SET_insert(S->worklistMoves,
                   cur_instr); // worklistMoves ← worklistMoves ∪ {I}
      }
      live = SET_union(live, FG_def(cur_node)); // live ← live ∪ def(I)
      for (void **dptr = SET_begin(FG_def(cur_node));
           dptr < SET_end(FG_def(cur_node)); dptr++) { // forall d ∈ def(I)
        for (void **lptr = SET_begin(live); lptr < SET_end(live);
             lptr++) { // forall l ∈ live
          AddEdge(*lptr, *dptr, S); // AddEdge(l, d)
        }
      }
      live = // live ← use(I) ∪ (live \ def(I))
          SET_union(FG_use(cur_node), SET_difference(live, FG_def(cur_node)));
      assert(G_pred(cur_node)->tail == NULL);
      // Traverse backwards
      cur_node = G_pred(cur_node)->head;
    }
    cur_block_end_list = cur_block_end_list->tail;
  }
}
/*
procedure MakeWorklist()
  forall n ∈ initial
    initial ← initial \ {n}
    if degree[n] ≥ K then
      spillWorklist ← spillWorklist ∪ {n}
    else if MoveRelated(n) then
      freezeWorklist ← freezeWorklist ∪ {n}
    else
      simplifyWorklist ← simplifyWorklist ∪ {n}
*/
void MakeWorkList();

/*
procedure AddEdge(u, v)
  if ((u, v) ̸∈ adjSet) ∧ (u ̸= v) then
    adjSet ← adjSet ∪ {(u, v), (v, u)}
    if u ̸∈ precolored then
      adjList[u] ← adjList[u] ∪ {v}
      degree[u] ← degree[u] + 1
    if v ̸∈ precolored then
      adjList[v] ← adjList[v] ∪ {u}
      degree[v] ← degree[v] + 1
*/
void AddEdge(Main_struct S, G_node u, G_node v) {
  if (SET_contains(S->adjSet, Edge(u, v)))
    return;

  SET_insert(S->adjSet, Edge(u, v));
  SET_insert(S->adjSet, Edge(v, u));

  if (!SET_contains(S->precolored, u)) {
    // adjList[u] ← adjList[u] ∪ {v}
    // degree[u] ← degree[u] + 1
  }
}