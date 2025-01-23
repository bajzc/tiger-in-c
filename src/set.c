#include "set.h"
#include "graph.h"
#include "util.h"

#define INITIAL_CAPACITY 4

struct Set_ {
  int size;
  int capacity;
  Comparer cmp;
  void **elements;
};

void SET_foreach(Set a, void (*f)(void *)) {
  for (int i = 0; i < a->size; i++) {
    f(a->elements[i]);
  }
}

void SET_ensure_capacity(Set s, int capacity) {
  int new_capacity = s->capacity;
  while (new_capacity < capacity) {
    new_capacity = MAX(new_capacity * 2, 1);
  }
  if (new_capacity == s->capacity)
    return;
  void **new = checked_malloc(new_capacity * sizeof(void *));
  memcpy(new, s->elements, s->capacity * sizeof(void *));
  s->elements = new;
  s->capacity = new_capacity;
}

Set SET_Set(int size, int capacity, Comparer cmp) {
  Set s = checked_malloc(sizeof(*s));
  s->size = size;
  s->capacity = capacity;
  s->cmp = cmp;
  s->elements = checked_malloc(s->capacity * sizeof(void *));
  return s;
}

Set SET_empty(Comparer cmp) {
  Set s = SET_Set(0, INITIAL_CAPACITY, cmp);
  s->elements = checked_malloc(s->capacity * sizeof(void *));
  return s;
}

Set SET_union(Set a, Set b) {
  if (a->cmp != b->cmp)
    debug("Set %p %p are using different cmp function\n", a->cmp, b->cmp);
  Set s = SET_copy(a);
  SET_ensure_capacity(s, a->size + b->size + 1);
  for (int i = 0; i < b->size; i++)
    SET_insert(s, b->elements[i]);
  return s;
}

Set SET_intersect(Set a, Set b) {
  if (a->cmp != b->cmp)
    debug("Set %p %p are using different cmp function", a->cmp, b->cmp);
  Set s = SET_Set(a->size, a->capacity, a->cmp);
  int j = 0;
  for (int i = 0; i < a->size; i++) {
    if (SET_contains(b, a->elements[i])) {
      s->elements[j++] = a->elements[i];
    } else {
      s->size--;
    }
  }
  return s;
}
Set SET_difference(Set a, Set b) {
  if (a->cmp != b->cmp)
    debug("Set %p %p are using different cmp function", a->cmp, b->cmp);
  Set s = SET_copy(a);
  SET_FOREACH(b, uptr) { SET_delete(s, *uptr); }
  return s;
}

int SET_insert(Set a, void *element) {
  if (SET_contains(a, element))
    return 0;
  SET_ensure_capacity(a, MAX(a->capacity, a->size + 1));
  int j = a->size - 1;
  while (j >= 0 && a->cmp(element, a->elements[j]) < 0) {
    a->elements[j + 1] = a->elements[j];
    j--;
  }
  a->elements[j + 1] = element;
  a->size++;
  return 1;
}
int SET_index_of(Set a, void *element) {
  int lo = 0;
  int hi = a->size - 1;
  int mid;
  while (lo <= hi) {
    mid = (lo + hi) / 2;
    int c = a->cmp(a->elements[mid], element);
    if (c == 0)
      return mid;
    if (c > 0)
      hi = mid - 1;
    else
      lo = mid + 1;
  }
  return -1;
}
int SET_delete(Set a, void *element) {
  int i = SET_index_of(a, element);
  if (i < 0)
    return 0;
  for (int j = i; j < a->size - 1; j++) {
    a->elements[j] = a->elements[j + 1];
  }
  a->size--;
  return 1;
}

int SET_iseq(Set a, Set b) {
  if (a->cmp != b->cmp)
    debug("Set %p %p are using different cmp function", a->cmp, b->cmp);
  if (a->size != b->size)
    return 0;
  for (int i = 0; i < a->size; i++) {
    if (a->cmp(a->elements[i], b->elements[i]) != 0)
      return 0;
  }
  return 1;
}
int SET_contains(Set a, void *element) { return SET_index_of(a, element) >= 0; }

Set SET_with(Set a, void *element) {
  Set r = SET_copy(a);
  SET_insert(r, element);
  return r;
}

Set SET_without(Set a, void *element) {
  Set r = SET_copy(a);
  SET_delete(r, element);
  return r;
}

Set SET_copy(Set a) {
  Set r = SET_Set(a->size, a->capacity, a->cmp);
  for (int i = 0; i < a->size; i++)
    r->elements[i] = a->elements[i];
  return r;
}

int SET_size(Set a) { return a->size; }

int SET_isEmpty(Set a) { return a->size == 0; }

void **SET_begin(Set a) { return a->elements; }

void **SET_end(Set a) { return a->elements + a->size; }

int SET_default_cmp(void *a, void *b) { return a - b; }

void *SET_pop(Set a) {
  if (a->size == 0)
    return NULL;
  else
    return a->elements[--(a->size)];
}
