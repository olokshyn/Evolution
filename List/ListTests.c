//
// Created by Oleg on 12/5/15.
//

#include <stdlib.h>
#include <stdio.h>

#include "ListTests.h"

static void printSuccessMsg(const char* testName) {
    printf("Test %30s: SUCCESS\n", testName);
}

static void printFailureMsg(const char* testName) {
    printf("Test %30s: FAILURE\n", testName);
}

void runAllListTests() {
    testInsertInEmptyList();
    testPushBackInEmptyList();
    testFindByValInEmptyList();
    testInsert();
    testPushBack();
    testFindByVal();
    testRemoveByVal();
}

int comp(const void* p1, const void* p2) {
    return *(int*)p1 - *(int*)p2;
}

void dest(void* value) {
    free(value);
}

void testInsertInEmptyList() {
    const char* testName = "testInsertInEmptyList";
    List list;
    if (!initList(&list, comp, dest)) {
        printFailureMsg(testName);
        return;
    }
    int* value = (int*)malloc(sizeof(int));
    *value = 10;
    if (!insert(begin(&list), value)) {
        printFailureMsg(testName);
    }
    if (!clearList(&list)) {
        printFailureMsg(testName);
        return;
    }
    printSuccessMsg(testName);
}

void testPushBackInEmptyList() {
    const char* testName = "testPushBackInEmptyList";
    List list;
    if (!initList(&list, comp, dest)) {
        printFailureMsg(testName);
        return;
    }
    int* value = (int*)malloc(sizeof(int));
    *value = 10;
    if (!pushBack(&list, value)) {
        printFailureMsg(testName);
    }
    if (!clearList(&list)) {
        printFailureMsg(testName);
        return;
    }
    printSuccessMsg(testName);
}

void testFindByValInEmptyList() {
    const char* testName = "testFindByValInEmptyList";
    List list;
    if (!initList(&list, comp, dest)) {
        printFailureMsg(testName);
        return;
    }
    int* value = (int*)malloc(sizeof(int));
    *value = 10;
    if (!isIteratorAtEnd(findByVal(&list, value))) {
        printFailureMsg(testName);
    }

    free(value);
    if (!clearList(&list)) {
        printFailureMsg(testName);
        return;
    }
    printSuccessMsg(testName);
}

void testInsert() {
    const char* testName = "testInsert";
    List list;
    if (!initList(&list, comp, dest)) {
        printFailureMsg(testName);
        return;
    }
    int* value;
    int i, k = 20;
    value = (int*)malloc(sizeof(int));
    *value = k;
    insert(begin(&list), value);
    for (i = 5; i < 11; ++i) {
        value = (int*)malloc(sizeof(int));
        *value = i;
        if (!insert(begin(&list), value)) {
            printFailureMsg(testName);
        }
    }
    i = 10;
    for (ListIterator it = begin(&list);
         !isIteratorAtEnd(it);
         next(&it)) {
        if (i > 4 && comp(it.current->value, &i) != 0) {
            printFailureMsg(testName);
            return;
        }
        else if (i == 4
                 && comp(it.current->value, &k) != 0) {
            printFailureMsg(testName);
            return;
        }
        --i;
    }
    if (!clearList(&list)) {
        printFailureMsg(testName);
        return;
    }
    printSuccessMsg(testName);
}

void testPushBack() {
    const char* testName = "testPushBack";
    List list;
    if (!initList(&list, comp, dest)) {
        printFailureMsg(testName);
        return;
    }
    int* value;
    int i;
    for (i = 0; i < 5; ++i) {
        value = (int*)malloc(sizeof(int));
        *value = i;
        if (!pushBack(&list, value)) {
            printFailureMsg(testName);
            return;
        }
    }
    i = 0;
    for (ListIterator it = begin(&list);
         !isIteratorAtEnd(it);
         next(&it), ++i) {
        if (comp(it.current->value, &i) != 0) {
            printFailureMsg(testName);
            return;
        }
    }
    if (!clearList(&list)) {
        printFailureMsg(testName);
        return;
    }
    printSuccessMsg(testName);
}

void testFindByVal() {
    const char* testName = "testFindByVal";
    List list;
    if (!initList(&list, comp, dest)) {
        printFailureMsg(testName);
        return;
    }
    int* value;
    int i;
    for (i = 0; i < 5; ++i) {
        value = (int*)malloc(sizeof(int));
        *value = i;
        if (!pushBack(&list, value)) {
            printFailureMsg(testName);
            return;
        }
    }
    i = 3;
    ListIterator it = findByVal(&list, &i);
    if (comp(it.current->value, &i) != 0) {
        printFailureMsg(testName);
        return;
    }
    i = 10;
    it = findByVal(&list, &i);
    if (it.current) {
        printFailureMsg(testName);
        return;
    }
    if (!clearList(&list)) {
        printFailureMsg(testName);
        return;
    }
    printSuccessMsg(testName);
}

void testRemoveByVal() {
    const char* testName = "testRemoveByVal";
    List list;
    if (!initList(&list, comp, dest)) {
        printFailureMsg(testName);
        return;
    }
    int* value;
    int i;
    for (i = 0; i < 5; ++i) {
        value = (int*)malloc(sizeof(int));
        *value = i;
        if (!pushBack(&list, value)) {
            printFailureMsg(testName);
            return;
        }
    }
    i = 3;
    if (!removeByVal(&list, &i)) {
        printFailureMsg(testName);
        return;
    }
    ListIterator it = findByVal(&list, &i);
    if (it.current != NULL) {
        printFailureMsg(testName);
        return;
    }
    if (!clearList(&list)) {
        printFailureMsg(testName);
        return;
    }
    printSuccessMsg(testName);
}