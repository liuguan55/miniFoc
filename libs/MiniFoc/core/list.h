//
// Created by 邓志君 on 2023/3/3.
//

#ifndef MINIFOC_F4_LIBS_SIMPLEFOC_CORE_LIST_H_
#define MINIFOC_F4_LIBS_SIMPLEFOC_CORE_LIST_H_
#include <stdlib.h>

typedef struct node{
  void *data;
  struct node *prev ;
  struct node *next;
}node;


typedef struct {
  size_t size;
  node *front;
  node *back;
}list;

list *makeList();

void listInit(list *);
void listDeinit(list *l);

void listInsert(list *, void *);
void listRemove(list *l, void *val);
void listRemoveByindex(list *l, int index);

void* listFind(list *l, void *val);
void* listFindByindex(list *l, int index);

void *listPop(list *l);
void *listPush(list *l, void *data);

size_t listLength(list *l);
#endif //MINIFOC_F4_LIBS_SIMPLEFOC_CORE_LIST_H_
