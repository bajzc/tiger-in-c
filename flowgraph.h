/*
 * flowgraph.h - Function prototypes to represent control flow graphs.
 */

#ifndef FLOWGRAPH_H
#define FLOWGRAPH_H

#include "assem.h"
#include "graph.h"
#include "set.h"

Set FG_def(G_node n);
Set FG_use(G_node n);
bool FG_isMove(G_node n);
G_graph FG_AssemFlowGraph(AS_instrList il);
void printFlowgraph(OUT_TYPE out, G_graph graph, Temp_map m, char *name);

#endif