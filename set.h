#ifndef SET_H
#define SET_H

#define SET_FOREACH(set, elementVar)                                           \
  for (void *_set = set, **elementVar = SET_begin(_set);                       \
       elementVar < SET_end(_set); elementVar++)

typedef struct Set_ *Set;

typedef int (*Comparer)(void *, void *);

Set SET_empty(Comparer cmp);

Set SET_union(Set a, Set b);

Set SET_intersect(Set a, Set b);

Set SET_difference(Set a, Set b);

int SET_insert(Set a, void *element);

int SET_delete(Set a, void *element);

Set SET_with(Set a, void *element);

Set SET_without(Set a, void *element);

Set SET_copy(Set a);

int SET_contains(Set a, void *element);

int SET_iseq(Set a, Set b);

void SET_foreach(Set a, void (*f)(void *));

int SET_size(Set a);

int SET_isEmpty(Set a);

void **SET_begin(Set a);

void **SET_end(Set a);

int SET_default_cmp(void *a, void *b);

void *SET_pop(Set a);

#endif // SET_H
