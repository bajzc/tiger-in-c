#ifndef LIVENESS_H
#define LIVENESS_H

#include "graph.h"
#include "table.h"
#include "temp.h"

typedef struct Live_moveList_ *Live_moveList;

struct Live_moveList_ {
  G_node src, dst;
  Live_moveList tail;
};
struct Live_graph {
  G_table liveOut, liveIn;
};

Live_moveList Live_MoveList(G_node src, G_node dst, Live_moveList tail);
Temp_temp Live_gtemp(G_node n);
struct Live_graph Live_Liveness(G_graph flow);

#endif // LIVENESS_H
