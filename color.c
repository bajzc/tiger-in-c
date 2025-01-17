#include "color.h"
#include "codegen.h"
#include "flowgraph.h"
#include "frame.h"
#include "graph.h"
#include "liveness.h"
#include "set.h"

#define T(n) ((Temp_temp) n->info)
typedef struct Main_struct_ {
  G_table liveOut, liveIn; // G_table<G_Node<AS_instr>, Set<Temp_temp>>
  G_table moveList; // G_table<G_Node<Temp_temp>, Set<AS_instr>>
  G_table alias; // G_table<G_Node, G_Node>
  Set stmt_instr_set; // Set<stmt_instr<stmt, AS_instr>>
  G_nodeList block_end_list; // List<G_node<AS_instr>>
  Set simplifyWorklist; // Set<G_Node<Temp_temp>>
  Set freezeWorklist; // Set<G_node<Temp_temp>>
  Set spillWorklist; // Set<G_node<Temp_temp>>
  Set spilledNodes; // Set<G_node>
  Set coalescedNodes; // Set<G_node>
  Set coalescedMoves; // Set<AS_instr>
  Set constrainedMoves; // Set<AS_instr>
  Set frozenMoves; // Set<AS_instr>
  Set worklistMoves; // Set<AS_instr>
  Set activeMoves; // Set<AS_instr>
  Set initial; // Set<G_node<Temp_temp>>
  Set coloredNodes; // Set<G_node<Temp_temp>>
  Set adjSet; // Set<Adj>
  G_nodeList selectStack; // List<G_node<Temp_temp>>
  G_graph flowgraph; // G_graph<AS_instr>
  Temp_map precolored; // Temp_map<Temp_temp, String>
  Temp_map color; // Temp_map<Temp_temp, String>
  struct Live_graph
      live_graph; // Live_graph<G_graph<Temp_temp>,Live_moveList<>>
  G_graph interference_graph;
  TAB_table temp2Node; // Map<Temp_temp, G_node<Temp_temp>>
  G_table adjList; // G_table<G_node<Temp_temp>, Set<G_node<Temp_temp>>>
  G_table degree; // G_table<G_node<Temp_temp>, int>
  int K;
  F_frame frame;
  AS_instrList iList;
} *Main_struct;

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
G_node GetAlias(G_node n, Main_struct S);
void AddWorkList(G_node u, Main_struct S);
int OK(G_node t, G_node r, Main_struct S);
int Conservative(Set nodes, Main_struct S);
void Combine(G_node u, G_node v, Main_struct S);
void visual_color(Set temps, Main_struct S);

typedef struct Adj_ {
  G_node u;
  G_node v;
} *Adj;

int AdjComparer(void *a, void *b) {
  Adj x = a;
  Adj y = b;
  if (x->u == y->u)
    return (size_t) (x->v) - (size_t) (y->v);
  return (size_t) (x->u) - (size_t) (y->u);
}

Adj makeAdj(G_node u, G_node v) {
  Adj adj = checked_malloc(sizeof(*adj));
  adj->u = u;
  adj->v = v;
  return adj;
}


/*
 * generate block_end_list for every instr in stmt_instr_set, find the node in
 * the flowgraph that has the instr, add it to the block_end_list
 */
void buildupLiveOut(Main_struct S) {
  S->block_end_list = NULL;
  SET_FOREACH(S->stmt_instr_set, instrptr) {
    struct stmt_instr *instr = *instrptr;
    AS_instr last = instr->last;
    for (G_nodeList node = G_nodes(S->flowgraph); node; node = node->tail) {
      if (G_nodeInfo(node->head) == last) {
        S->block_end_list = G_NodeList(node->head, S->block_end_list);
        break;
      }
    }
  }
}

int isBlockStart(G_node node, Main_struct S) {
  G_nodeList preds = G_pred(node);
  return preds == NULL || preds->head == NULL ||
         G_inNodeList(preds->head, S->block_end_list);
}

/*
Degree invariant.
  (u ∈ simplifyWorklist ∪ freezeWorklist ∪ spillWorklist) ⇒
  degree(u) = |adjList(u) ∩ (precolored ∪ simplifyWorklist
  ∪ freezeWorklist ∪ spillWorklist)|

Simplify worklist invariant.
  (u ∈ simplifyWorklist) ⇒
  degree(u) < K ∧ moveList[u] ∩ (activeMoves ∪ worklistMoves) = {}

Freeze worklist invariant.
  (u ∈ freezeWorklist) ⇒
  degree(u) < K ∧ moveList[u] ∩ (activeMoves ∪ worklistMoves) ̸= {}

Spill worklist invariant.
  (u ∈ spillWorklist) ⇒ degree(u) ≥ K
*/
void checkInvariant(Main_struct S) {
  // Degree invariant
  Set unions_temps = SET_copy(F_regTemp);
  SET_FOREACH(S->simplifyWorklist, uptr) {
    SET_insert(unions_temps, G_nodeInfo(*uptr));
  }
  SET_FOREACH(S->freezeWorklist, uptr) {
    SET_insert(unions_temps, G_nodeInfo(*uptr));
  }
  SET_FOREACH(S->spillWorklist, uptr) {
    SET_insert(unions_temps, G_nodeInfo(*uptr));
  }
  SET_FOREACH(SET_union(SET_union(S->simplifyWorklist, S->freezeWorklist),
                        S->spillWorklist),
              uptr) {
    G_node u = *uptr;
    int degree = *(int *) G_look(S->degree, u);
    Set adjListU = G_look(S->adjList, u);
    Set adjListU_temp = SET_empty(Temp_temp_cmp);
    SET_FOREACH(adjListU, adjptr) {
      SET_insert(adjListU_temp, G_nodeInfo(*adjptr));
    }
    int size = SET_size(SET_intersect(adjListU_temp, unions_temps));
    // fprintf(stderr, "\nadjListU_temp = {");
    SET_FOREACH(adjListU_temp, adjptr) {
      // fprintf(stderr, "%d, ", ((Temp_temp) *adjptr)->num);
    }
    // fprintf(stderr, "}\n");
    // fprintf(stderr, "unions_temps = {");
    SET_FOREACH(unions_temps, adjptr) {
      // fprintf(stderr, "%d, ", ((Temp_temp) *adjptr)->num);
    }
    // fprintf(stderr, "}\n");
    assert(degree == size);
  }

  // Simplify worklist
  SET_FOREACH(S->simplifyWorklist, uptr) {
    G_node u = *uptr;
    Set moveListU = G_look(S->moveList, u);
    assert(*(int *) G_look(S->degree, u) < S->K);
    if (moveListU != NULL)
      assert(SET_isEmpty(SET_intersect(
          moveListU, SET_union(S->activeMoves, S->worklistMoves))));
  }

  // Freeze worklist
  SET_FOREACH(S->freezeWorklist, uptr) {
    G_node u = *uptr;
    Set moveListU = G_look(S->moveList, u);
    assert(*(int *) G_look(S->degree, u) < S->K);
    if (moveListU != NULL)
      assert(!SET_isEmpty(SET_intersect(
          moveListU, SET_union(S->activeMoves, S->worklistMoves))));
  }

  // Spill worklist
  SET_FOREACH(S->spillWorklist, uptr) {
    G_node u = *uptr;
    int k = *(int *) G_look(S->degree, u);
    assert(k >= S->K);
  }
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
    G_node cur_node = cur_block_end_list->head; // G_node<AS_instr>
    Set live = G_look(S->liveOut, cur_node); // let live = liveOut(b)
    while (cur_node &&
           !isBlockStart(cur_node, S)) { // forall I ∈ instructions(b)
      AS_instr cur_instr = G_nodeInfo(cur_node);
#if DEBUG2
      debug2("liveout = ");
      SET_FOREACH(live, sptr) {
        Temp_temp s = (*sptr);
        fprintf(stderr, "%d, ", s->num);
      }
      fprintf(stderr, "\n");
      fprintf(stderr, "\t%s\n", cur_instr->u.OPER.assem);
#endif
      if (FG_isMove(cur_node)) { // if isMoveInstruction(I) then
        live = SET_difference(live, FG_use(cur_node)); // live ← live \ use(I)
        Set defAndUse = SET_union(FG_use(cur_node), FG_def(cur_node));
        SET_FOREACH(defAndUse, nptr) { // forall n ∈ def(I) ∪ use(I)
          Temp_temp n = *nptr;
          G_node node = TAB_look(S->temp2Node, n);
          assert(node);
          Set moveList = G_look(S->moveList, node);
          if (moveList == NULL)
            moveList = SET_empty(SET_default_cmp);
          SET_insert(moveList, cur_instr); // moveList[n] ← moveList[n] ∪ {I}
          G_enter(S->moveList, node, moveList);
        }
        SET_insert(S->worklistMoves,
                   cur_instr); // worklistMoves ← worklistMoves ∪ {I}
      }
      live = SET_union(live, FG_def(cur_node)); // live ← live ∪ def(I)
      Set defI = FG_def(cur_node);
      SET_FOREACH(defI, dptr) { // forall d ∈ def(I)
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
  Set moveListN = G_look(S->moveList, n);
  if (!moveListN)
    return SET_empty(SET_default_cmp);
  return SET_intersect(moveListN, SET_union(S->activeMoves, S->worklistMoves));
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
  Set adjListN = G_look(S->adjList, n);
  return SET_difference(adjListN,
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
  int *d = G_look(S->degree, m); // let d = degree[m]
  *d -= 1;
  G_enter(S->degree, m, d);
  if (*d + 1 != S->K)
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
    if (*(int *) G_look(S->degree, n) >= S->K) { // if degree[n] ≥ K then
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
void AddEdge(Temp_temp u, Temp_temp v, Main_struct S) {
  G_node U = TAB_look(S->temp2Node, u), V = TAB_look(S->temp2Node, v);
  assert(!((U == V && u != v) || (U != V && u == v))); // they are both unique
  Adj tuple = makeAdj(U, V);
  Adj tuple2 = makeAdj(V, U);
  if (SET_contains(S->adjSet, tuple) || u == v)
    return;

  SET_insert(S->adjSet, tuple);
  SET_insert(S->adjSet, tuple2);
  if (!Temp_look(S->precolored, u)) {
    Set adjListU = G_look(S->adjList, U);
    SET_insert(adjListU, V);
    int *deg = G_look(S->degree, U);
    *deg += 1;
    G_enter(S->degree, U, deg);
  }
  if (!Temp_look(S->precolored, v)) {
    Set adjListV = G_look(S->adjList, V);
    SET_insert(adjListV, U);
    int *deg = G_look(S->degree, V);
    *deg += 1;
    G_enter(S->degree, V, deg);
  }
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
void Coalesce(Main_struct S) {
  debug2("\n");
  AS_instr m = NULL;
  SET_FOREACH(S->worklistMoves, mptr) {
    m = *mptr;
    if (m->kind == I_MOVE)
      break;
  }
  if (!m)
    assert(0);
  G_node x = GetAlias(TAB_look(S->temp2Node, m->u.MOVE.src->head), S);
  G_node y = GetAlias(TAB_look(S->temp2Node, m->u.MOVE.dst->head), S);
  G_node u, v;
  if (Temp_look(S->precolored, G_nodeInfo(y))) {
    u = y;
    v = x;
  } else {
    u = x;
    v = y;
  }
  SET_delete(S->worklistMoves, m);
  if (Temp_temp_cmp(G_nodeInfo(u), G_nodeInfo(v)) == 0) {
    SET_insert(S->coalescedMoves, m);
    AddWorkList(u, S);
    /*
     *   else if (Temp_look(S->precolored, G_nodeInfo(v)) || G_goesTo(u, v))
     *   TO
     *   else if (Temp_look(S->precolored, G_nodeInfo(v)) ||
     *            SET_contains(S->adjSet, makeAdj(u, v)))
     *   It takes about THREE DAYS to find this bug. However, countless bugs was
     *   found on the way of debugging
     *   This bug was caused by a mindless refactor from using G_node to record
     *   the interference graph information, to use a set of Adj to store all
     *   the edges
     */
  } else if (Temp_look(S->precolored, G_nodeInfo(v)) ||
             SET_contains(S->adjSet, makeAdj(u, v))) {
    SET_insert(S->constrainedMoves, m);
    AddWorkList(u, S);
    AddWorkList(v, S);
  } else if ((Temp_look(S->precolored, G_nodeInfo(u)) && ({
                int _temp = 1;
                SET_FOREACH(Adjacent(v, S), tptr) {
                  if (!OK(*tptr, u, S)) {
                    _temp = 0;
                    break;
                  }
                }
                _temp;
              })) ||
             (!Temp_look(S->precolored, G_nodeInfo(u)) &&
              Conservative(SET_union(Adjacent(u, S), Adjacent(v, S)), S))) {
    SET_insert(S->coalescedMoves, m);
    Combine(u, v, S);
    AddWorkList(u, S);
  } else {
    SET_insert(S->activeMoves, m);
  }
}

/*
procedure AddWorkList(u)
  if (u ∉ precolored ∧ not(MoveRelated(u)) ∧ degree[u] < K) then
    freezeWorklist ← freezeWorklist \ {u}
    simplifyWorklist ← simplifyWorklist ∪ {u}
*/
void AddWorkList(G_node u, Main_struct S) {
  if (Temp_look(S->precolored, G_nodeInfo(u)) || MoveRelated(u, S) ||
      *(int *) G_look(S->degree, u) >= S->K)
    return;

  SET_delete(S->freezeWorklist, u);
  SET_insert(S->simplifyWorklist, u);
}

/*
function OK(t,r)
  degree[t] < K ∨ t ∈ precolored ∨ (t, r) ∈ adjSet
*/
int OK(G_node t, G_node r, Main_struct S) {
  Temp_temp tTemp = G_nodeInfo(t);
  Adj tuple = makeAdj(t, r);
  return *(int *) G_look(S->degree, t) < S->K ||
         Temp_look(S->precolored, tTemp) != NULL ||
         SET_contains(S->adjSet, tuple);
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
    if (*(int *) G_look(S->degree, n) >= S->K)
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
  if (SET_contains(S->coalescedNodes, n)) {
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
  if (SET_contains(S->freezeWorklist, v))
    SET_delete(S->freezeWorklist, v);
  if (SET_contains(S->spillWorklist, v))
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
    AddEdge(G_nodeInfo(t), G_nodeInfo(u), S);
    DecrementDegree(t, S);
  }

  if (*(int *) G_look(S->degree, u) >= S->K &&
      SET_contains(S->freezeWorklist, u)) {
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
  Set nodeMoves = NodeMoves(u, S);

  SET_FOREACH(nodeMoves, mptr) {
    AS_instr instr = *mptr;

    // verify that we only need to take care of one src and dst
    assert(instr->u.MOVE.src->tail == NULL);
    assert(instr->u.MOVE.dst->tail == NULL);
    G_node x = TAB_look(S->temp2Node, instr->u.MOVE.src->head);
    G_node y = TAB_look(S->temp2Node, instr->u.MOVE.dst->head);

    G_node v;
    if (GetAlias(y, S) == GetAlias(u, S)) {
      v = GetAlias(x, S);
    } else {
      v = GetAlias(y, S);
    }

    SET_delete(S->activeMoves, instr);
    SET_insert(S->frozenMoves, instr);

    if (SET_isEmpty(NodeMoves(v, S)) && *(int *) G_look(S->degree, v) < S->K) {
      SET_delete(S->freezeWorklist, v);
      SET_insert(S->simplifyWorklist, v);
    }
  }
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
void SelectSpill(Main_struct S) {
  G_node m = SET_pop(S->spillWorklist); // FIXME
  SET_insert(S->simplifyWorklist, m);
  FreezeMoves(m, S);
}

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
void AssignColors(Main_struct S) {
  while (S->selectStack) {
    G_node n = G_top(S->selectStack);
    S->selectStack = S->selectStack->tail;
    Set okColors = SET_copy(F_regString); // Set<String>
    SET_FOREACH(G_look(S->adjList, n), wptr) {
      G_node w = *wptr;
      Set coloredNodes_temp = SET_empty(Temp_temp_cmp);
      SET_FOREACH(S->coloredNodes, cptr) {
        G_node c = *cptr;
        SET_insert(coloredNodes_temp, G_nodeInfo(c));
      }
      if (SET_contains(SET_union(coloredNodes_temp, F_regTemp),
                       G_nodeInfo(GetAlias(w, S)))) {
        SET_delete(okColors, Temp_look(S->color, G_nodeInfo(GetAlias(w, S))));
      }
    }
    if (SET_isEmpty(okColors))
      SET_insert(S->spilledNodes, n);
    else {
      SET_insert(S->coloredNodes, n);
      string c = SET_pop(okColors);
      Temp_enter(S->color, G_nodeInfo(n), c);
      SET_insert(okColors, c); // c was not deleted
    }
  }
  SET_FOREACH(S->coalescedNodes, nptr) {
    G_node n = *nptr;
    char *aliasColor = Temp_look(S->color, G_nodeInfo(GetAlias(n, S)));
    Temp_enter(S->color, G_nodeInfo(n), aliasColor);
  }
}


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
void RewriteProgram(Main_struct S) {
  Set newTemps = SET_empty(SET_default_cmp);
  SET_FOREACH(S->spilledNodes, vptr) { G_node v = *vptr; }
  assert(0);

  S->spilledNodes = SET_empty(SET_default_cmp);
  S->coloredNodes = SET_empty(SET_default_cmp);
  S->initial =
      SET_union(S->coloredNodes, SET_union(S->coalescedNodes, newTemps));
  S->coalescedNodes = SET_empty(SET_default_cmp);
}

void print_edges(Main_struct S) {
  static int graph_count = 0;
  char out_file[80];
  snprintf(out_file, 80, "%d-edges.dot", graph_count);
  FILE *fp = fopen(out_file, "w");
  fprintf(fp, "strict graph{\n");
  SET_FOREACH(S->adjSet, tptr) {
    Adj t = *tptr;
    Temp_temp u = ((Temp_temp) G_nodeInfo(t->u));
    Temp_temp v = ((Temp_temp) G_nodeInfo(t->v));
    if (Temp_look(F_tempMap, u) || Temp_look(F_tempMap, v))
      continue;
    fprintf(fp, "\tT%d -- T%d\n", u->num, v->num);
  }
  fprintf(fp, "}\n");
  fclose(fp);
  debug("generate %d-edges.dot\n", graph_count);
  graph_count++;
}

/*
procedure Main()
  LivenessAnalysis()
  Build()
  MakeWorklist()
  repeat
    if simplifyWorklist ̸= {} then Simplify()
    else if worklistMoves ̸= {} then Coalesce()
    else if freezeWorklist ̸= {} then Freeze()
    else if spillWorklist ̸= {} then SelectSpill()
  until simplifyWorklist = {} ∧ worklistMoves = {}
        ∧ freezeWorklist = {} ∧ spillWorklist = {}
  AssignColors()
  if spilledNodes ̸= {} then
    RewriteProgram(spilledNodes)
    Main()
*/
Temp_map Color_Main(Set stmt_instr_set, AS_instrList iList, F_frame frame) {
  Main_struct S = checked_malloc(sizeof(*S));
  S->iList = iList;
  S->frame = frame;
  S->stmt_instr_set = stmt_instr_set;
  S->K = F_numGPR;
  S->flowgraph = FG_AssemFlowGraph(iList);
  S->live_graph = Live_Liveness(S->flowgraph);
  S->liveIn = S->live_graph.liveIn;
  S->liveOut = S->live_graph.liveOut;
  S->precolored = F_tempMap;
  S->color = Temp_layerMap(Temp_empty(), S->precolored);

  // init to empty
  S->adjSet = SET_empty(AdjComparer);
  S->spilledNodes = SET_empty(G_node_cmp);
  S->adjList = G_empty();
  S->degree = G_empty();
  S->temp2Node = TAB_empty();
  S->initial = SET_empty(G_node_cmp);
  S->moveList = G_empty();
  S->worklistMoves = SET_empty(SET_default_cmp);
  S->alias = G_empty();
  S->simplifyWorklist = SET_empty(G_node_cmp);
  S->freezeWorklist = SET_empty(G_node_cmp);
  S->spillWorklist = SET_empty(G_node_cmp);
  S->coalescedNodes = SET_empty(G_node_cmp);
  S->coalescedMoves = SET_empty(SET_default_cmp);
  S->constrainedMoves = SET_empty(SET_default_cmp);
  S->frozenMoves = SET_empty(SET_default_cmp);
  S->activeMoves = SET_empty(SET_default_cmp);
  S->coloredNodes = SET_empty(G_node_cmp);
  S->selectStack = NULL;

  Set temps = SET_empty(Temp_temp_cmp); // Set<Temp_temp>
  for (G_nodeList l = G_nodes(S->flowgraph); l; l = l->tail) {
    G_node n = l->head;
    temps = SET_union(temps, FG_def(n));
    temps = SET_union(temps, FG_use(n));
  }
  // we probably won't construct edges, but we need the G_node to warp the data
  S->interference_graph = G_Graph();
  SET_FOREACH(temps, tptr) {
    Temp_temp t = *tptr;
    G_node node = G_Node(S->interference_graph, t);
    int *deg = checked_malloc(sizeof(*deg));
    *deg = 0;
    G_enter(S->adjList, node, SET_empty(G_node_cmp));
    G_enter(S->degree, node, deg);
    TAB_enter(S->temp2Node, t, node);
  }

  fprintf(stderr, "\ninitial = ");
  SET_FOREACH(SET_difference(temps, F_regTemp), tptr) {
    Temp_temp t = *tptr;
    SET_insert(S->initial, TAB_look(S->temp2Node, t));
    fprintf(stderr, "%d, ", t->num);
  }
  fprintf(stderr, "\n");

  fprintf(stderr, "\nF_regTemp = ");
  SET_FOREACH(F_regTemp, tptr) {
  fprintf(stderr, "%d, ", (*(Temp_temp *) tptr)->num);
  }
  fprintf(stderr, "\n");

  build(S);
  checkInvariant(S);
  // print_edges(S);
  MakeWorkList(S);
  checkInvariant(S);
  while (TRUE) {
    if (!SET_isEmpty(S->simplifyWorklist))
      Simplify(S);
    else if (!SET_isEmpty(S->worklistMoves))
      Coalesce(S);
    else if (!SET_isEmpty(S->freezeWorklist))
      Freeze(S);
    else if (!SET_isEmpty(S->spillWorklist))
      SelectSpill(S);

    // print_edges(S);
    checkInvariant(S);

    if (SET_isEmpty(S->simplifyWorklist) && SET_isEmpty(S->worklistMoves) &&
        SET_isEmpty(S->freezeWorklist) && SET_isEmpty(S->spillWorklist))
      break;
  }
  AssignColors(S);
  checkInvariant(S);
  if (!SET_isEmpty(S->spilledNodes)) {
    RewriteProgram(S);
    return Color_Main(stmt_instr_set, iList, frame); // TODO
  }
  visual_color(temps, S);
  return S->color;
}

void visual_color(Set temps, Main_struct S) {
  char out_file[80];
  snprintf(out_file, 80, "%s.dot", Temp_labelstring(F_name(S->frame)));
  FILE *out = fopen(out_file, "w");
  fprintf(out, "strict graph{\n");
  fprintf(out, "\tsplines=false;\n");
  fprintf(out, "\tnode [style=filled, shape=circle];\n");

  SET_FOREACH(temps, tptr) {
    Temp_temp t = *tptr;
    char *color = Temp_look(S->color, t);
    Temp_temp reg = TAB_lookOnString(F_color2reg, color);
    char *colorScheme = Temp_look(F_reg2colorscheme, reg);
    fprintf(out, "\t\"%d\" [colorscheme=%s, color=%s]\n", t->num, colorScheme,
            Temp_look(F_reg2color, reg));
  }
  fprintf(out, "\n\n");

  // show all nodes' color only
  // SET_FOREACH(temps, tptr) {
  //   Temp_temp t = *tptr;
  //   fprintf(out, "\t\"%d\"\n", t->num);
  // }

  SET_FOREACH(temps, tptr) {
    Temp_temp t = *tptr;
    G_node node = TAB_look(S->temp2Node, t);
    G_node alias = GetAlias(node, S);
    if (alias != node)
      continue;
    Set adjs = G_look(S->adjList, node);
    SET_FOREACH(adjs, uptr) {
      G_node u = *uptr;
      Temp_temp tt = G_nodeInfo(u);
      G_node alias2 = GetAlias(u, S);
      if (alias2 != u)
        continue;
      fprintf(out, "\t\"%d\" -- \"%d\"\n", t->num, tt->num);
    }
  }

  fprintf(out, "}\n");
}
