#include "util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct tree *T_tree;
struct tree {
  T_tree left;
  string key;
  void *binding;
  T_tree right;
};
T_tree Tree(T_tree l, string k, void *b, T_tree r) {
  T_tree t = checked_malloc(sizeof(*t));
  t->left = l;
  t->key = k;
  t->binding = b;
  t->right = r;
  return t;
}

T_tree insert(string key, void *binding, T_tree t) {
  if (t == NULL)
    return Tree(NULL, key, binding, NULL);
  else if (strcmp(key, t->key) < 0)
    return Tree(insert(key, binding, t->left), t->key, t->binding, t->right);
  else if (strcmp(key, t->key) > 0)
    return Tree(t->left, t->key, t->binding, insert(key, binding, t->right));
  else
    return Tree(t->left, key, binding, t->right);
}

void *lookup(string key, T_tree t) {
  if (t == NULL)
    return NULL;
  if (strcmp(key, t->key) < 0)
    return lookup(key, t->left);
  else if (strcmp(key, t->key) > 0)
    return lookup(key, t->right);
  else
    return t->binding;
}

void a() { printf("a\n"); }
void b() { printf("b\n"); }
void c() { printf("c\n"); }

int main() {
  T_tree t;
  t = insert("a", &a, NULL);
  t = insert("b", &b, t);
  t = insert("c", &c, t);
  ((void (*)())lookup("c", t))();
  ((void (*)())lookup("a", t))();
  ((void (*)())lookup("b", t))();
}
