/*
 * color.h - Data structures and function prototypes for coloring algorithm
 *             to determine register allocation.
 */
#ifndef COLOR_H
#define COLOR_H

#include "assem.h"
#include "frame.h"
#include "graph.h"
#include "temp.h"

struct COL_result {
  Temp_map coloring;
  Temp_tempList spills;
};
struct COL_result COL_color(G_graph ig, Temp_map initial, Temp_tempList regs);
Temp_map Color_Main(Set stmt_instr_set, AS_instrList iList, F_frame frame);

#endif
