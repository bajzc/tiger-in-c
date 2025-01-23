/*
 * color.h - Data structures and function prototypes for coloring algorithm
 *             to determine register allocation.
 */
#ifndef COLOR_H
#define COLOR_H

#include "assem.h"
#include "frame.h"
#include "graph.h"
#include "liveness.h"
#include "temp.h"

typedef struct Main_struct_ {
    G_table liveOut, liveIn; // G_table<G_Node<AS_instr>, Set<Temp_temp>>
    G_table moveList; // G_table<G_Node<Temp_temp>, Set<AS_instr>>
    G_table alias; // G_table<G_Node, G_Node>
    Set last_instr; // Set<AS_instr>
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
struct COL_result {
  Temp_map coloring;
  Temp_tempList spills;
};

int isBlockStart(G_node node, Main_struct S);
void buildupLiveOut(Main_struct S);
Temp_map Color_Main(Set last_instr, AS_instrList iList, F_frame frame);

#endif
