/* escape.c */

#include "escape.h"
#include "absyn.h"
#include "table.h"

struct S_symbol_ {
  string name;
  S_symbol next;
};

static struct S_symbol_ marksym = {"<mark>", 0};
typedef struct E_escapeEntry_ *E_escapeEntry;
struct E_escapeEntry_ {
  int depth;
  bool *escape;
};

static void E_TAB_enter(S_table env, S_symbol sym, E_escapeEntry bind) {
  TAB_enter(env, sym, bind);
}

static void E_beginScope(S_table env) { E_TAB_enter(env, &marksym, NULL); }

static void E_endScope(S_table env) {
  S_symbol s;
  do {
    s = TAB_pop(env);
  } while (s != &marksym);
}

E_escapeEntry EscapeEntry(int depth, bool *escape) {
  E_escapeEntry entry = checked_malloc(sizeof(*entry));
  entry->depth = depth;
  entry->escape = escape;
  return entry;
}

static void traverseExp(S_table env, int depth, A_exp a);
static void traverseDec(S_table env, int depth, A_dec d);
static void traverseVar(S_table env, int depth, A_var v);

U_boolList genEscapeList(E_enventry funEntry) {
  Ty_tyList formals = funEntry->u.fun.formals;
  U_boolList l = NULL, l_head = NULL;
  if (formals) {
    l = U_BoolList(TRUE, NULL);
    l_head = l;
    formals = formals->tail;
  }
  while (formals) {
    l->tail = U_BoolList(TRUE, NULL);
    l = l->tail;
    formals = formals->tail;
  }
  return l_head;
}

void Esc_findEscape(A_exp exp) { traverseExp(S_empty(), 0, exp); }

static void traverseVar(S_table env, int depth, A_var v) {
  switch (v->kind) {
    case A_simpleVar: { // ID
      E_escapeEntry entry = TAB_look(env, v->u.simple);
      if (entry->depth < depth) {
        *(entry->escape) = TRUE;
        debug("In depth %d, set '%s' (defined in depth %d) to be escape\n",
              depth, S_name(v->u.simple), entry->depth);
      }
      break;
    }
    case A_fieldVar: { // lvalue DOT ID
      traverseVar(env, depth, v->u.field.var);
      break;
    }
    case A_subscriptVar: { // ID LBRACK exp RBRACK
      traverseVar(env, depth, v->u.subscript.var);
      traverseExp(env, depth, v->u.subscript.exp);
      break;
    }
    default:
      assert(0);
  }
}

static void traverseDec(S_table env, int depth, A_dec d) {
  switch (d->kind) {
    case A_functionDec: {
      for (A_fundecList l = d->u.function; l; l = l->tail) {
        A_fundec f = l->head;
        depth += 1;
        E_beginScope(env);
        // instasll formals to env
        for (A_fieldList L = f->params; L; L = L->tail) {
          L->head->escape = FALSE;
          debug("set function '%s' argument '%s' to be non-escape\n",
                S_name(f->name), S_name(L->head->name));
          E_TAB_enter(env, L->head->name, EscapeEntry(depth, &L->head->escape));
        }
        traverseExp(env, depth, l->head->body);
        depth -= 1;
        E_endScope(env);
      }
      break;
    } // end case A_functionDec
    case A_varDec: {
      traverseExp(env, depth, d->u.var.init);
      d->u.var.escape = FALSE;
      E_TAB_enter(env, d->u.var.var, EscapeEntry(depth, &d->u.var.escape));
      break;
    }
    case A_typeDec: {
      break;
    }
    default:
      assert(0);
  }
}

static void traverseExp(S_table env, int depth, A_exp a) {
  if (a == NULL)
    return;
  switch (a->kind) {
    case A_varExp: {
      traverseVar(env, depth, a->u.var);
      return;
    }
    case A_nilExp:
    case A_intExp:
    case A_stringExp:
      break;
    case A_callExp: {
      A_expList e = a->u.call.args;
      while (e) {
        debug("set all arguments be escaped\n");
        traverseExp(env, depth + 1,
                    e->head); // all variable used should be escaped
        debug("\n");
        e = e->tail;
      }
      break;
    }
    case A_opExp: {
      traverseExp(env, depth, a->u.op.left);
      traverseExp(env, depth, a->u.op.right);
      break;
    } // end case A_opExp
    case A_recordExp: {
      for (A_efieldList l = a->u.record.fields; l; l = l->tail) {
        traverseExp(env, depth, l->head->exp);
      }
      break;
    }
    case A_seqExp: {
      for (A_expList l = a->u.seq; l; l = l->tail) {
        traverseExp(env, depth, l->head);
      }
      break;
    }
    case A_assignExp: {
      traverseVar(env, depth, a->u.assign.var);
      traverseExp(env, depth, a->u.assign.exp);
      break;
    }
    case A_ifExp: {
      traverseExp(env, depth, a->u.iff.test);
      traverseExp(env, depth, a->u.iff.then);
      if (a->u.iff.elsee) { // if-then-else
        traverseExp(env, depth, a->u.iff.elsee);
        return;
      }
      break;
    }
    case A_whileExp: {
      traverseExp(env, depth, a->u.whilee.test);
      traverseExp(env, depth, a->u.whilee.body);
      break;
    }
    case A_forExp: {
      traverseExp(env, depth, a->u.forr.lo);
      traverseExp(env, depth, a->u.forr.hi);
      depth += 1;
      E_beginScope(env);
      a->u.forr.escape = FALSE;
      debug("set iterator '%s' in for-loop be non-escape\n",
            S_name(a->u.forr.var));
      E_TAB_enter(env, a->u.forr.var, EscapeEntry(depth, &(a->u.forr.escape)));
      {
        traverseExp(env, depth, a->u.forr.body);
      }
      E_endScope(env);
      depth -= 1;
      break;
    }
    case A_breakExp:
      break;
    case A_letExp: {
      depth += 1;
      E_beginScope(env);
      for (A_decList d = a->u.let.decs; d; d = d->tail)
        traverseDec(env, depth, d->head);
      traverseExp(env, depth, a->u.let.body);
      depth -= 1;
      E_endScope(env);
      break;
    }
    case A_arrayExp: {
      traverseExp(env, depth, a->u.array.size);
      traverseExp(env, depth, a->u.array.init);
      break;
    }
    default:
      assert(0);
  } // end switch(a->kind)
}
