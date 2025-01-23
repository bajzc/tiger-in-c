#include <gtest/gtest.h>
extern "C" {
#include "color.h"
#include "graph.h"
#include "flowgraph.h"
}

#define L(h, t) Temp_TempList((Temp_temp) h, (Temp_tempList) t)
#define S(format, ...) snprintf(buf, 80, format, ##__VA_ARGS__)

/*
0   L0:
1     li  a0, 1
2     mv  t6, a0
3     beq a0, t6, L1
4     j   L2
5   L2:
6     li  a0, 3
7     ret
8   L1:
9     li  a0, 4
10    ret
*/
Main_struct isBlockStart_test_init() {
  Main_struct S = static_cast<Main_struct>(checked_malloc(sizeof(*S)));
  AS_instrList iList = nullptr;
  Set last_instr = SET_empty(SET_default_cmp);
  char buf[80];

  S->iList = iList =
      AS_InstrList(AS_Label("L0:\n", Temp_namedlabel("L0")), nullptr);
  iList->tail = AS_InstrList(
      AS_Oper("li `d0, 1 # a0", L(F_RV(), NULL), nullptr, nullptr), nullptr);
  iList = iList->tail;
  Temp_temp t6 = Temp_newtemp();
  iList->tail = AS_InstrList(
      AS_Move("mv `d0, `s0 # t6, a0", L(t6, nullptr), L(F_RV(), nullptr)),
      nullptr);
  iList = iList->tail;
  Temp_label L1, L2;
  L1 = Temp_namedlabel("L1");
  L2 = Temp_namedlabel("L2");
  iList->tail = AS_InstrList(
      AS_Oper("beq `s0, `s1, `j0 # a0, t6, L1", nullptr,
              L(t6, L(F_RV(), nullptr)),
              AS_Targets(Temp_LabelList(L1, Temp_LabelList(L2, nullptr)))),
      NULL);
  iList = iList->tail;
  SET_insert(last_instr, iList->head);
  iList->tail = AS_InstrList(AS_Oper("j `j0 # L2", nullptr, nullptr,
                                     AS_Targets(Temp_LabelList(L2, nullptr))),
                             nullptr);
  iList = iList->tail;
  SET_insert(last_instr, iList->head);
  iList->tail = AS_InstrList(AS_Label("L2:", L2), nullptr);
  iList = iList->tail;
  iList->tail = AS_InstrList(
      AS_Oper("li `d0, 3 # a0", L(F_RV(), nullptr), nullptr, nullptr), nullptr);
  iList = iList->tail;
  iList->tail = AS_InstrList(
      AS_Oper("ret", nullptr, L(F_RV(), nullptr), nullptr), nullptr);
  iList = iList->tail;
  SET_insert(last_instr, iList->head);
  iList->tail = AS_InstrList(AS_Label("L1:", L1), nullptr);
  iList = iList->tail;
  iList->tail = AS_InstrList(
      AS_Oper("li `d0, 4", L(F_RV(), nullptr), nullptr, nullptr), nullptr);
  iList = iList->tail;
  iList->tail = AS_InstrList(
      AS_Oper("ret", nullptr, L(F_RV(), nullptr), nullptr), nullptr);
  iList = iList->tail;
  SET_insert(last_instr, iList->head);

  S->last_instr = last_instr;

  return S;
}

TEST(color, test_jump) {
  auto S = isBlockStart_test_init();

  Color_Main(S->last_instr, S->iList, nullptr);
}
