//
// Created by Oleg on 12/5/15.
//

#ifndef WISHART_LIST_H
#define WISHART_LIST_H

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

short isIteratorAtEnd(ListIterator it);

short checkList(List* list);

#endif //WISHART_LIST_H
