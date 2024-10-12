/* translate.c */
#include "translate.h"
#include "frame.h"
#include "temp.h"

struct Tr_access_ {
  Tr_level level;
  F_access access;
};

static Tr_level OUTER_MOST;

Tr_level Tr_outermost(void) {
	debug("init outermost level\n");
  Tr_level t = checked_malloc(sizeof(*t));
	t->frame = F_newFrame(Temp_namedlabel("global"), NULL);
  t->parent = NULL;
  t->name = Temp_namedlabel("prelude");
  t->formals = NULL;
	OUTER_MOST = t;
  return t;
}

Tr_access Tr_allocLocal(Tr_level level, bool escape) {
  Tr_access t = checked_malloc(sizeof(*t));
#if DEBUG
	if(level == OUTER_MOST)
		debug("alloc to global scape\n");
#endif
  t->level = level;
  t->access = F_allocLocal(level->frame, escape);
  return t;
}

Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals) {
  Tr_level level = checked_malloc(sizeof(*level));
  level->parent = parent;
  level->name = name;
  level->formals = formals;
  // add the static links to the first place, rest is in order
  level->frame = F_newFrame(name, U_BoolList(TRUE, formals));
  return level;
}

Tr_accessList Tr_formals(Tr_level level) {
  F_accessList l = F_formals(level->frame);
  F_access p = NULL;
  Tr_accessList L = checked_malloc(sizeof(*L));
  Tr_accessList L_head = L;
  while (l) {
    p = l->head;
    L->head = checked_malloc(sizeof(*L->head));
    L->head->level = level;
    L->head->access = p;
    L->tail = checked_malloc(sizeof(*L));
    l = l->tail;
    L = L->tail;
  }
  return L_head;
}

void Tr_printFormals(Tr_accessList formals){
	debug("-----\n");
	fflush(stderr);
  Tr_access p = NULL;
	assert(formals->head);
	printf("static links: ");
	F_printAccess(formals->head->access);
	formals = formals->tail;
	while(formals->head){
		p = formals->head;
		F_printAccess(p->access);
		formals=formals->tail;
	}
	fflush(stdout);
	debug("-----\n");
}
