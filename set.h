#ifndef SET_H
#define SET_H

#include <stdio.h>

typedef struct Set_* Set;

typedef int (*Comparer)(void*, void*);

Set SET_empty(Comparer cmp);

Set SET_union(Set a, Set b);

Set SET_intersect(Set a, Set b);

Set SET_difference(Set a, Set b);

int SET_insert(Set a, void* element);

int SET_delete(Set a, void* element);

Set SET_with(Set a, void* element);

Set SET_without(Set a, void* element);

Set SET_copy(Set a);

int SET_contains(Set a, void* element);

int SET_eq(Set a, Set b);

void SET_foreach(Set a, void (*f)(void*));

#endif //SET_H
