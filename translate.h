/* translate.h */
#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "temp.h"
#include "frame.h"

typedef struct Tr_access_ *Tr_access;

typedef struct Tr_accessList_ *Tr_accessList;
struct Tr_accessList_ {
  Tr_access head;
  Tr_accessList tail;
};
Tr_accessList Tr_AccessList(Tr_access head, Tr_accessList tail);

typedef struct Tr_level_ *Tr_level;
struct Tr_level_ {
	F_frame frame;
  Tr_level parent;
  Temp_label name;
  U_boolList formals; // escape or not
};

Tr_level Tr_outermost(void);
Tr_level Tr_newLevel(Tr_level parent, Temp_label name, U_boolList formals);
Tr_accessList Tr_formals(Tr_level level);
Tr_access Tr_allocLocal(Tr_level level, bool escape);
void Tr_printFormals(Tr_accessList formals);

#endif
