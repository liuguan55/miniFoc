//
// Created by 邓志君 on 2023/3/3.
//

#include "list.h"

list *makeList() {
  list *l = calloc(1, sizeof(list));
  l->size = 0;
  l->front = 0;
  l->back = 0;
  return l;
}

void listInit(list *l) {
  l->size = 0;
  l->front = 0;
  l->back = 0;
}

void *listPop(list *l) {
  if (!l->back) return 0;

  node *b = l->back;
  void *val = b->data;
  l->back = b->prev;
  if (l->back) l->back->next = 0;
  free(b);
  --l->size;

  return val;
}

void *listPush(list *l, void *data) {
  listInsert(l, data);

  return NULL;
}

void listInsert(list *l, void *val) {
  if (!val) return;

  node *new = malloc(sizeof(node));
  new->data = val;
  new->next = 0;

  if (!l->back) {
    l->front = new;
    new->prev = 0;
  } else {
    l->back->next = new;
    new->prev = l->back;
  }
  l->back = new;
  ++l->size;
}

void *listFind(list *l, void *val) {
  if (!val) return NULL;

  node *n = l->front;

  while (n) {
    if (n->data == val) {
      return n->data;
    }

    n = n->next;
  }

  return NULL;
}

void *listFindByindex(list *l, int index) {
  node *n = l->front;
  int curIndex = 0;

  while (n) {
    if (curIndex == index) {
      return n->data;
    }

    curIndex++;
    n = n->next;
  }

  return NULL;
}

void listRemove(list *l, void *val) {
  if (!val) return;

  node *n = l->front;
  while (n) {
    if (n->data == val) {
      n->prev->next = n->next;
      if (n->next) {
        n->next->prev = n->prev;
      }

      free(n);
      l->size--;
    }
    n = n->next;
  }
}

void listRemoveByindex(list *l, int index) {
  node *n = l->front;

  int curIndex = 0;
  while (n) {
    if (curIndex == index) {
      n->prev->next = n->next;
      if (n->next) {
        n->next->prev = n->prev;
      }

      free(n);
      l->size--;
    }

    curIndex++;
    n = n->next;
  }
}

void listDeinit(list *l) {
  while (l->back) {
    listPop(l);
  }
}

size_t listLength(list *l) {
  return l->size;
}
