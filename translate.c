/* translate.c */
#include "translate.h"
#include "frame.h"
#include "temp.h"

struct Tr_access_ {
  Tr_level level;
  F_access access;
};

Tr_level Tr_outermost(void) {
  Tr_level t = checked_malloc(sizeof(Tr_level));
  t->parent = NULL;
  t->name = Temp_namedlabel("prelude");
  t->formals = NULL;
  return t;
}

Tr_access Tr_allocLocal(Tr_level level, bool escape) {
  Tr_access t = checked_malloc(sizeof(*t));
  t->level = level;
  t->access = F_allocLocal(F_newFrame(level->name, level->formals), escape);
  return t;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals) {
  Tr_level level = checked_malloc(sizeof(*level));
  level->parent = parent;
  level->name = name;
  level->formals = formals;
  level->frame = F_newFrame(
      name, U_BoolList(TRUE, formals)); // add the static links to the first
                                        // place, rest is in order
  return level;
}

Tr_accessList Tr_formals(Tr_level level){
  // TODO generate new list
	assert(0);
}
