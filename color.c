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
  Set stmt_instr_set; // Set<stmt_instr<stmt, AS_instr>>
  G_nodeList block_end_list; // List<G_node<AS_instr>>
  Set simplifyWorklist; // Set<G_Node<Temp_temp>>
  Set freezeWorklist; // Set<G_node<Temp_temp>>
  Set spillWorklist; // Set<>
  Set spilledNodes; // Set<>
  Set coalescedNodes; // Set<>
  Set coalescedMoves; // Set<AS_instr>
  Set constrainedMoves; // Set<AS_instr>
  Set frozenMoves; // Set<AS_instr>
  Set worklistMoves; // Set<AS_instr>
  Set activeMoves; // Set<AS_instr>
  Set initial; // Set<G_node<Temp_temp>>
  G_nodeList selectStack; // Stack<G_node<Temp_temp>>
  G_graph flowgraph; // G_graph<AS_instr>
  Temp_map precolored; // Temp_map<Temp_temp, String>
  struct Live_graph
      *live_graph; // Live_graph<G_graph<Temp_temp>,Live_moveList<>>
  TAB_table temp2Node; // TAB_table<Temp_temp, G_node<Temp_temp>>

  // G_table adjList; // Map<G_Node<Temp_temp>, <G_Node<Temp_temp>>>
  // G_table degree; // Map<G_Node<Temp_temp>, int>
  Set adjSet; // Set<Adj(Temp_temp, Temp_temp)>
  int K;
} *Main_struct;
typedef struct Adj_ {
  Temp_temp u;
  Temp_temp v;
} *Adj;

void build(Main_struct S);
void AddEdge(Temp_temp u, Temp_temp v, Main_struct S);
void MakeWorkList(Main_struct S);
int MoveRelated(G_node n, Main_struct S);
Set NodeMoves(G_node n, Main_struct S);
void EnableMoves(Set nodes, Main_struct S);
Set Adjacent(G_node n, Main_struct S);
void Simplify(Main_struct S);
void DecrementDegree(G_node m, Main_struct S);
void FreezeMoves(G_node u, Main_struct S);

/*
 * generate block_end_list for every instr in stmt_instr_set, find the node in
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
        SET_FOREACH(defAndUse, nptr) { // forall n ∈ def(I) ∪ use(I)
          Temp_temp n = *nptr;
          G_node node = G_findNodeWithInfo(S->live_graph->graph, n);
          Set moveList = G_look(S->moveList, node);
          SET_insert(moveList, cur_instr); // moveList[n] ← moveList[n] ∪ {I}
        }
        SET_insert(S->worklistMoves,
                   cur_instr); // worklistMoves ← worklistMoves ∪ {I}
      }
      live = SET_union(live, FG_def(cur_node)); // live ← live ∪ def(I)
      SET_FOREACH(FG_def(cur_node), dptr) { // forall d ∈ def(I)
        SET_FOREACH(live, lptr) { // forall l ∈ live
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
function NodeMoves (n)
  moveList[n] ∩ (activeMoves ∪ worklistMoves)
*/
Set NodeMoves(G_node n, Main_struct S) {
  return SET_intersect(G_look(S->moveList, n),
                       SET_union(S->activeMoves, S->worklistMoves));
}

/*
function MoveRelated(n)
  NodeMoves(n) ̸= {}
*/
int MoveRelated(G_node n, Main_struct S) { return SET_size(NodeMoves(n, S)); }

/*
procedure Simplify()
  let n ∈ simplifyWorklist
  simplifyWorklist ← simplifyWorklist \ {n}
  push(n, selectStack)
  forall m ∈ Adjacent(n)
    DecrementDegree(m)
*/
void Simplify(Main_struct S) {
  G_node n = SET_pop(S->simplifyWorklist); // let n ∈ simplifyWorklist
  // simplifyWorklist ← simplifyWorklist \ {n}
  S->selectStack = G_push(S->selectStack, n);
  Set adj = Adjacent(n, S);
  SET_FOREACH(adj, mptr) {
    G_node m = *mptr;
    DecrementDegree(m, S);
  }
}

/*
function Adjacent(n)
  adjList[n] \ (selectStack ∪ coalescedNodes)
*/
Set Adjacent(G_node n, Main_struct S) {
  return SET_difference(G_toSet(G_adj(n)),
                        SET_union(G_toSet(S->selectStack), S->coalescedNodes));
}

/*
procedure DecrementDegree(m)
  let d = degree[m]
  degree[m] ← d-1
  if d = K then
    EnableMoves({m} ∪ Adjacent(m))
    spillWorklist ← spillWorklist \ {m}
    if MoveRelated(m) then
      freezeWorklist ← freezeWorklist ∪ {m}
    else
      simplifyWorklist ← simplifyWorklist ∪ {m}
*/
void DecrementDegree(G_node m, Main_struct S) {
  int d = G_degree(m); // let d = degree[m]
  if (d != S->K)
    return;

  EnableMoves(SET_with(Adjacent(m, S), m), S); // EnableMoves({m} ∪ Adjacent(m))
  SET_delete(S->spillWorklist, m); // spillWorklist ← spillWorklist \ {m}
  if (MoveRelated(m, S)) {
    SET_insert(S->freezeWorklist, m);
  } else {
    SET_insert(S->simplifyWorklist, m);
  }
}

/*
procedure EnableMoves(nodes)
  forall n ∈ nodes
    forall m ∈ NodeMoves(n)
      if m ∈ activeMoves then
        activeMoves ← activeMoves \ {m}
        worklistMoves ← worklistMoves ∪ {m}
*/
void EnableMoves(Set nodes, Main_struct S) {
  SET_FOREACH(nodes, nptr) { // forall n ∈ nodes
    G_node n = *nptr;
    Set nodeMoves = NodeMoves(n, S);
    SET_FOREACH(nodeMoves, mptr) {
      G_node m = *mptr;
      if (SET_contains(S->activeMoves, m)) {
        SET_delete(S->activeMoves, m);
        SET_insert(S->worklistMoves, m);
      }
    }
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
void MakeWorkList(Main_struct S) {
  SET_FOREACH(S->initial, ptr) { // forall n ∈ initial
    G_node n = *ptr;
    if (G_degree(n) >= S->K) { // if degree[n] ≥ K then
      SET_insert(S->spillWorklist, n); // spillWorklist ← spillWorklist ∪ {n}
    } else if (MoveRelated(n, S)) { // else if MoveRelated(n) then
      SET_insert(S->freezeWorklist, n); // freezeWorklist ← freezeWorklist ∪ {n}
    } else {
      SET_insert(S->simplifyWorklist,
                 n); // simplifyWorklist ← simplifyWorklist ∪ {n}
    }
  }
  S->initial = SET_empty(SET_default_cmp); // initial ← initial \ {n}
}

/*
procedure AddEdge(u, v)
  if ((u, v) ∉ adjSet) ∧ (u ̸= v) then
    adjSet ← adjSet ∪ {(u, v), (v, u)}
    if u ∉ precolored then
      adjList[u] ← adjList[u] ∪ {v}
      degree[u] ← degree[u] + 1
    if v ∉ precolored then
      adjList[v] ← adjList[v] ∪ {u}
      degree[v] ← degree[v] + 1
*/
// we don't talk about performance yet :)
void AddEdge(Temp_temp u, Temp_temp v, Main_struct S) {
  G_addBiEdge(TAB_look(S->temp2Node, u), TAB_look(S->temp2Node, v));
}

/*
procedure Coalesce()
  let m_(=copy(x,y)) ∈ worklistMoves
  x ← GetAlias(x)
  y ← GetAlias(y)
  if y ∈ precolored then
    let (u, v) = (y, x)
  else
    let (u, v) = (x, y)
  worklistMoves ← worklistMoves \ {m}
  if (u = v) then
    coalescedMoves ← coalescedMoves ∪ {m}
    AddWorkList(u)
  else if v ∈ precolored ∨ (u, v) ∈ adjSet then
    constrainedMoves ← constrainedMoves ∪ {m}
    AddWorkList(u)
    AddWorkList(v)
  else if u ∈ precolored ∧ (∀t ∈ Adjacent(v), OK(t, u))
          ∨ u ∉ precolored ∧
          Conservative(Adjacent(u) ∪ Adjacent(v)) then
    coalescedMoves ← coalescedMoves ∪ {m}
    Combine(u,v)
    AddWorkList(u)
  else
    activeMoves ← activeMoves ∪ {m}
*/

/*
procedure AddWorkList(u)
  if (u ∉ precolored ∧ not(MoveRelated(u)) ∧ degree[u] < K) then
    freezeWorklist ← freezeWorklist \ {u}
    simplifyWorklist ← simplifyWorklist ∪ {u}
*/
void AddWorkList(G_node u, Main_struct S) {
  if (Temp_look(S->precolored, TAB_look(S->temp2Node, u)) || MoveRelated(u, S) || G_degree(u) >= S->K)
    return;

  SET_delete(S->freezeWorklist, u);
  SET_insert(S->simplifyWorklist, u);
}

/*
function OK(t,r )
  degree[t] < K ∨ t ∈ precolored ∨ (t, r ) ∈ adjSet
*/

int OK(G_node t, G_node r, Main_struct S) {
  Temp_temp tTemp = G_nodeInfo(t);
  return G_degree(t) < S->K || Temp_look(S->precolored, tTemp) != NULL ||
         G_goesTo(t, r);
}

/*
function Conservative(nodes)
  let k = 0
  forall n ∈ nodes
    if degree[n] ≥ K then k ← k + 1
  return (k < K)
*/

int Conservative(Set nodes, Main_struct S) {
  int k = 0;
  SET_FOREACH(nodes, nptr) {
    G_node n = *nptr;
    if (G_degree(n) >= S->K)
      k++;
  }
  return k < S->K;
}

/*
function GetAlias (n)
  if n ∈ coalescedNodes then
    GetAlias(alias[n])
  else n
 */
G_node GetAlias(G_node n, Main_struct S) {
  if (SET_contains(S->coalescedMoves, n)) {
    return GetAlias(G_look(S->alias, n), S);
  }
  return n;
}

/*
procedure Combine(u,v)
  if v ∈ freezeWorklist then
    freezeWorklist ← freezeWorklist \ {v}
  else
    spillWorklist ← spillWorklist \ {v}
  coalescedNodes ← coalescedNodes ∪ {v}
  alias[v] ← u
  moveList[u] ← moveList[u] ∪ moveList[v]
  EnableMoves(v)
  forall t ∈ Adjacent(v)
    AddEdge(t,u)
    DecrementDegree(t)
  if degree[u] ≥ K ∧ u ∈ freezeWorkList
    freezeWorkList ← freezeWorkList \ {u}
    spillWorkList ← spillWorkList ∪ {u}
*/
void Combine(G_node u, G_node v, Main_struct S) {
  if (!SET_delete(S->freezeWorklist, v))
    SET_delete(S->spillWorklist, v);

  SET_insert(S->coalescedNodes, v);
  G_enter(S->alias, v, u);
  Set moveListU = G_look(S->moveList, u);
  Set moveListV = G_look(S->moveList, v);
  G_enter(S->moveList, u, SET_union(moveListU, moveListV));
  Set singletonV = SET_empty(SET_default_cmp);
  SET_insert(singletonV, v);
  EnableMoves(singletonV, S);

  Set adjV = Adjacent(v, S);
  SET_FOREACH(adjV, tptr) {
    G_node t = *tptr;
    AddEdge(TAB_look(S->temp2Node, t), TAB_look(S->temp2Node, u), S);
    DecrementDegree(t, S);
  }

  if (G_degree(u) >= S->K && SET_contains(S->freezeWorklist, u)) {
    SET_delete(S->freezeWorklist, u);
    SET_insert(S->spillWorklist, u);
  }
}

/*
procedure Freeze()
  let u ∈ freezeWorklist
  freezeWorklist ← freezeWorklist \ {u}
  simplifyWorklist ← simplifyWorklist ∪ {u}
  FreezeMoves(u)
*/

void Freeze(Main_struct S) {
  G_node u = SET_pop(S->freezeWorklist);
  SET_insert(S->simplifyWorklist, u);
  FreezeMoves(u, S);
}

/*
procedure FreezeMoves(u)
  forall m_(=copy(x,y)) ∈ NodeMoves(u)
    if GetAlias(y)=GetAlias(u) then
      v ← GetAlias(x)
    else
      v ← GetAlias(y)
    activeMoves ← activeMoves \ {m}
    frozenMoves ← frozenMoves ∪ {m}
    if NodeMoves(v) = {} ∧ degree[v] < K then
      freezeWorklist ← freezeWorklist \ {v}
      simplifyWorklist ← simplifyWorklist ∪ {v}
*/
void FreezeMoves(G_node u, Main_struct S) {
  // TODO
}

/*
procedure SelectSpill()
  let m ∈ spillWorklist selected using favorite heuristic
    Note: avoid choosing nodes that are the tiny live ranges
    resulting from the fetches of previously spilled registers
  spillWorklist ← spillWorklist \ {m}
  simplifyWorklist ← simplifyWorklist ∪ {m}
  FreezeMoves(m)
 */

/*
procedure AssignColors()
  while SelectStack not empty
    let n = pop(SelectStack)
    okColors ← {0, ... , K-1}
    forall w ∈ adjList[n]
      if GetAlias(w) ∈ (coloredNodes ∪ precolored) then
        okColors ← okColors \ {color[GetAlias(w)]}
    if okColors = {} then
      spilledNodes ← spilledNodes ∪ {n}
    else
      coloredNodes ← coloredNodes ∪ {n}
      let c ∈ okColors
      color[n] ← c
  forall n ∈ coalescedNodes
    color[n] ← color[GetAlias(n)]
*/

/*
procedure RewriteProgram()
  Allocate memory locations for each v ∈ spilledNodes,
  Create a new temporary v_i for each definition and each use,
  In the program (instructions), insert a store after each
  definition of a v_i , a fetch before each use of a v_i .
  Put all the v_i into a set newTemps.
  spilledNodes ← {}
  initial ← coloredNodes ∪ coalescedNodes ∪ newTemps
  coloredNodes ← {}
  coalescedNodes ← {}
 */