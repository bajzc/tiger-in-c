#include "flowgraph.h"
#include "set.h"

static Set FG_TempList2Set(Temp_tempList l) {
  Set s = SET_empty(SET_default_cmp);
  for (; l; l = l->tail) {
    SET_insert(s, l->head);
  }
  return s;
}

/**
 * @param n G_node<AS_instr>
 * @return Set<Temp_temp>
 */
Set FG_def(G_node n) {
  AS_instr instr = G_nodeInfo(n);
  switch (instr->kind) {
    case I_OPER: return FG_TempList2Set(instr->u.OPER.dst);
    case I_LABEL: return FG_TempList2Set(NULL);
    case I_MOVE: return FG_TempList2Set(instr->u.MOVE.dst);
    default: assert(0);
  }
}

/**
 * @param n G_node<AS_instr>
 * @return Set<Temp_temp>
 */
Set FG_use(G_node n) {
  AS_instr instr = G_nodeInfo(n);
  switch (instr->kind) {
    case I_OPER: return FG_TempList2Set(instr->u.OPER.src);
    case I_LABEL: return FG_TempList2Set(NULL);
    case I_MOVE: return FG_TempList2Set(instr->u.MOVE.src);
    default: assert(0);
  }
}

/**
 * @param n G_node<AS_instr>
 */
bool FG_isMove(G_node n) { return ((AS_instr) G_nodeInfo(n))->kind == I_MOVE; }


G_node FG_AssemFlowGraph_Internal(AS_instrList il, G_graph graph,
                                  S_table label_map,
                                  Temp_labelList headLabels) {
  if (il == NULL)
    return NULL;
  if (il->head->kind == I_OPER || il->head->kind == I_MOVE) {
    G_node node = G_Node(graph, il->head);

    Temp_labelList curHeadLabels = headLabels;
    while (curHeadLabels != NULL) {
      S_enter(label_map, curHeadLabels->head, node);
      curHeadLabels = curHeadLabels->tail;
    }

    G_node fallthrough =
        FG_AssemFlowGraph_Internal(il->tail, graph, label_map, NULL);

    // all labels should be in label_map at this point
    if (il->head->kind == I_OPER && il->head->u.OPER.jumps != NULL) {
      Temp_labelList cur = il->head->u.OPER.jumps->labels;
      while (cur != NULL) {
        G_node next = S_look(label_map, cur->head);
        if (next != NULL)
          G_addEdge(node, next);
        cur = cur->tail;
      }
    } else if (fallthrough != NULL) {
      G_addEdge(node, fallthrough);
    }
    return node;
  } else if (il->head->kind == I_LABEL)
    return FG_AssemFlowGraph_Internal(
        il->tail, graph, label_map,
        Temp_LabelList(il->head->u.LABEL.label, headLabels));
  assert(0);
}

G_graph FG_AssemFlowGraph(AS_instrList il) {
  if (il == NULL)
    return NULL;
  S_table label_map = S_empty();
  G_graph graph = G_Graph();
  FG_AssemFlowGraph_Internal(il, graph, label_map, NULL);
  return graph;
}