//
// Created by Oleg on 12/5/15.
//

#ifndef EVOLUTION_LIST_H
#define EVOLUTION_LIST_H

#include <stdlib.h>

typedef struct node {
    void* value;
    struct node* next;
    struct node* prev;
} Node;

typedef int (*comparator)(const void* p1, const void* p2);

typedef void* (*copier)(void* value);

typedef void (*destructor)(void* value);

typedef struct list {
    Node* head;
    Node* tail;
    size_t length;
    comparator c;
    destructor d;
} List;

typedef struct list_iterator {
    List* list;
    Node* current;
} ListIterator;

short initList(List* list, comparator c, destructor d);

short clearList(List* list);

short clearListPointer(List* list);

short pushBack(List* list, void* value);

short insert(ListIterator next, void* value);

short copyList(List* destination, List* source, copier cp);

short moveList(List* destination, List* source);

ListIterator findByVal(List* list, void* value);

ListIterator findByIndex(List* list, size_t index);

short removeByValue(List* list, void* value);

short removeByIndex(List* list, size_t index);

short removeFromList(ListIterator it);

ListIterator begin(List* list);

ListIterator end(List* list);

void next(ListIterator* it);

void prev(ListIterator* it);

short isIteratorExhausted(ListIterator it);

#define FOR_EACH_IN_LIST_N(LIST_P, LIST_IN_IT) \
for (ListIterator LIST_IN_IT = begin(LIST_P); \
     !isIteratorExhausted(LIST_IN_IT); \
     next(&LIST_IN_IT))

#define FOR_EACH_IN_LIST(LIST_P) FOR_EACH_IN_LIST_N(LIST_P, list_in_it)

#define FOR_EACH_IN_LIST_BACK_N(LIST_P, LIST_IN_IT) \
for (ListIterator LIST_IN_IT = {LIST_P, LIST_P->tail}; \
     !isIteratorExhausted(LIST_IN_IT); \
     prev(&LIST_IN_IT))

#define FOR_EACH_IN_LIST_BACK(LIST_P) FOR_EACH_IN_LIST_BACK_N(LIST_P, list_in_it)

#define LIST_IT_VALUE_P_N(LIST_IN_IT, TYPE) \
((TYPE*)LIST_IN_IT.current->value)

#define LIST_IT_VALUE_P(TYPE) LIST_IT_VALUE_P_N(list_in_it, TYPE)

#define LIST_IT_VALUE_N(LIST_IN_IT, TYPE) \
(*LIST_IT_VALUE_P_N(LIST_IN_IT, TYPE))

#define LIST_IT_VALUE(TYPE) LIST_IT_VALUE_N(list_in_it, TYPE)

#endif //EVOLUTION_LIST_H
