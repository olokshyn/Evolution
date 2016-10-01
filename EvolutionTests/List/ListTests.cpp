//
// Created by Oleg on 8/29/16.
//

#include "gtest/gtest.h"

extern "C" {
#include "List/List.h"
}

using namespace std;

namespace {
    const size_t elems_count = 10;

    int double_comparator(const void* p1, const void* p2) {
        double res = *(double*)p1 - *(double*)p2;
        return (res > 0) - (res < 0);
    }

    static void* double_copier(void* value) {
        double* new_v = (double*)malloc(sizeof(double));
        if (new_v) {
            *new_v = *(double*)value;
        }
        return new_v;
    }
}

TEST(ListTest, Creation) {
    List list;
    ASSERT_EQ(1, initList(&list, NULL, free));
    ASSERT_EQ((size_t)0, list.length);
    size_t i = 0;
    FOR_EACH_IN_LIST(&list) {
        ++i;
    }
    ASSERT_EQ((size_t)0, i);
    ASSERT_EQ(1, destroyList(&list));

    List* list_p = (List*)malloc(sizeof(List));
    ASSERT_NE((void*)0, list_p);
    ASSERT_EQ(1, initList(list_p, NULL, free));
    ASSERT_EQ((size_t)0, list_p->length);
    i = 0;
    FOR_EACH_IN_LIST(list_p) {
        ++i;
    }
    ASSERT_EQ((size_t)0, i);
    ASSERT_EQ(1, destroyListPointer(list_p));
}

TEST(ListTest, PushBack) {
    List* list_p = (List*)malloc(sizeof(List));
    ASSERT_NE((void*)0, list_p);
    initList(list_p, NULL, free);

    double* el = NULL;
    for (size_t i = 0; i < elems_count; ++i) {
        el = (double*)malloc(sizeof(double));
        ASSERT_NE((void*)0, el);
        *el = i + 0.1;
        ASSERT_EQ(1, pushBack(list_p, el));
    }
    ASSERT_EQ(elems_count, list_p->length);

    size_t i = 0;
    FOR_EACH_IN_LIST(list_p) {
        ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
        ++i;
    }
    ASSERT_EQ(elems_count, i);

    i = elems_count - 1;
    size_t pass_count = 0;
    FOR_EACH_IN_LIST_BACK(list_p) {
        ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
        --i;
        ++pass_count;
    }
    ASSERT_EQ(elems_count, pass_count);

    destroyListPointer(list_p);
}

TEST(ListTest, Insert) {
    List* list_p = (List*)malloc(sizeof(List));
    ASSERT_NE((void*)0, list_p);
    initList(list_p, NULL, free);

    double* el = NULL;
    for (size_t i = 0; i < elems_count; ++i) {
        el = (double*)malloc(sizeof(double));
        ASSERT_NE((void*)0, el);
        *el = i + 0.1;
        ASSERT_EQ(1, insert(begin(list_p), el));
    }
    ASSERT_EQ(elems_count, list_p->length);

    el = (double*)malloc(sizeof(double));
    ASSERT_NE((void*)0, el);
    *el = 2.2;
    ASSERT_EQ(1, insert(end(list_p), el));
    ASSERT_EQ(elems_count + 1, list_p->length);

    ListIterator it = {
            list_p,
            list_p->head->next->next // [2]
    };

    el = (double*)malloc(sizeof(double));
    ASSERT_NE((void*)0, el);
    *el = 3.3;
    ASSERT_EQ(1, insert(it, el));
    ASSERT_EQ(elems_count + 2, list_p->length);

    size_t i = elems_count - 1;
    size_t index = 0;
    FOR_EACH_IN_LIST(list_p) {
        if (index == 2) {
            ASSERT_EQ(3.3, LIST_IT_VALUE(double));
        }
        else if (index == elems_count + 1) {
            ASSERT_EQ(2.2, LIST_IT_VALUE(double));
        }
        else {
            ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
            --i;
        }
        ++index;
    }
    ASSERT_EQ(elems_count + 2, index);

    i = 0;
    index = elems_count + 1;
    size_t pass_count = 0;
    FOR_EACH_IN_LIST_BACK(list_p) {
        if (index == 2) {
            ASSERT_EQ(3.3, LIST_IT_VALUE(double));
        }
        else if (index == elems_count + 1) {
            ASSERT_EQ(2.2, LIST_IT_VALUE(double));
        }
        else {
            ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
            ++i;
        }
        --index;
        ++pass_count;
    }
    ASSERT_EQ(elems_count + 2, pass_count);

    destroyListPointer(list_p);
}

TEST(ListTest, EmptyList) {
    List* list_p = (List*)malloc(sizeof(List));
    ASSERT_NE((void*)0, list_p);
    initList(list_p, NULL, free);

    double* el = NULL;
    for (size_t i = 0; i < elems_count; ++i) {
        el = (double*)malloc(sizeof(double));
        ASSERT_NE((void*)0, el);
        *el = i + 0.1;
        ASSERT_EQ(1, pushBack(list_p, el));
    }

    ASSERT_EQ(1, clearList(list_p));
    ASSERT_EQ((size_t)0, list_p->length);

    size_t i = 0;
    FOR_EACH_IN_LIST(list_p) {
        ++i;
    }
    ASSERT_EQ((size_t)0, i);

    for (i = 0; i < elems_count; ++i) {
        el = (double*)malloc(sizeof(double));
        ASSERT_NE((void*)0, el);
        *el = i + 0.1;
        ASSERT_EQ(1, pushBack(list_p, el));
    }
    ASSERT_EQ(elems_count, list_p->length);

    i = 0;
    FOR_EACH_IN_LIST(list_p) {
        ++i;
    }
    ASSERT_EQ(elems_count, i);

    destroyListPointer(list_p);
}

TEST(ListTest, CopyList) {
    List* list_p = (List*)malloc(sizeof(List));
    ASSERT_NE((void*)0, list_p);
    initList(list_p, NULL, free);

    double* el = NULL;
    for (size_t i = 0; i < elems_count; ++i) {
        el = (double*)malloc(sizeof(double));
        ASSERT_NE((void*)0, el);
        *el = i + 0.1;
        ASSERT_EQ(1, pushBack(list_p, el));
    }

    List* list_p_2 = (List*)malloc(sizeof(List));
    ASSERT_NE((void*)0, list_p_2);
    initList(list_p_2, NULL, free);

    ASSERT_EQ(1, copyList(list_p_2, list_p, double_copier));
    ASSERT_EQ(elems_count, list_p_2->length);

    ASSERT_NE(list_p->head, list_p_2->head);
    ASSERT_NE(list_p->tail, list_p_2->tail);

    size_t i = 0;
    FOR_EACH_IN_LIST(list_p_2) {
        ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
        ++i;
    }
    ASSERT_EQ(elems_count, i);

    i = elems_count - 1;
    size_t pass_count = 0;
    FOR_EACH_IN_LIST_BACK(list_p_2) {
        ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
        --i;
        ++pass_count;
    }
    ASSERT_EQ(elems_count, pass_count);

    ASSERT_EQ(1, copyList(list_p, list_p_2, double_copier));
    ASSERT_EQ(2 * elems_count, list_p->length);

    i = 0;
    pass_count = 0;
    FOR_EACH_IN_LIST(list_p) {
        ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
        if (i == elems_count - 1) {
            i = 0;
        }
        else {
            ++i;
        }
        ++pass_count;
    }
    ASSERT_EQ(2 * elems_count, pass_count);

    i = elems_count - 1;
    pass_count = 0;
    FOR_EACH_IN_LIST_BACK(list_p) {
        ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
        if (i == 0) {
            i = elems_count - 1;
        }
        else {
            --i;
        }
        ++pass_count;
    }
    ASSERT_EQ(2 * elems_count, pass_count);

    destroyListPointer(list_p);

    i = 0;
    FOR_EACH_IN_LIST(list_p_2) {
        ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
        ++i;
    }
    ASSERT_EQ(elems_count, i);

    i = elems_count - 1;
    pass_count = 0;
    FOR_EACH_IN_LIST_BACK(list_p_2) {
        ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
        --i;
        ++pass_count;
    }
    ASSERT_EQ(elems_count, pass_count);

    destroyListPointer(list_p_2);
}

TEST(ListTest, MoveList) {
    List* list_p = (List*)malloc(sizeof(List));
    ASSERT_NE((void*)0, list_p);
    initList(list_p, NULL, free);

    double* el = NULL;
    for (size_t i = 0; i < elems_count; ++i) {
        el = (double*)malloc(sizeof(double));
        ASSERT_NE((void*)0, el);
        *el = i + 0.1;
        ASSERT_EQ(1, pushBack(list_p, el));
    }

    List* list_p_2 = (List*)malloc(sizeof(List));
    ASSERT_NE((void*)0, list_p_2);
    initList(list_p_2, NULL, free);

    ASSERT_EQ(1, moveList(list_p_2, list_p));

    ASSERT_EQ((void*)0, list_p->head);
    ASSERT_EQ((void*)0, list_p->tail);
    ASSERT_EQ((size_t)0, list_p->length);

    size_t i = 0;
    FOR_EACH_IN_LIST(list_p_2) {
        ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
        ++i;
    }
    ASSERT_EQ(elems_count, i);

    i = elems_count - 1;
    size_t pass_count = 0;
    FOR_EACH_IN_LIST_BACK(list_p_2) {
        ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
        --i;
        ++pass_count;
    }
    ASSERT_EQ(elems_count, pass_count);

    for (i = 0; i < elems_count; ++i) {
        el = (double*)malloc(sizeof(double));
        ASSERT_NE((void*)0, el);
        *el = i + 0.1;
        ASSERT_EQ(1, pushBack(list_p, el));
    }

    ASSERT_EQ(1, moveList(list_p, list_p_2));
    ASSERT_EQ(2 * elems_count, list_p->length);

    ASSERT_EQ((void*)0, list_p_2->head);
    ASSERT_EQ((void*)0, list_p_2->tail);
    ASSERT_EQ((size_t)0, list_p_2->length);

    i = 0;
    pass_count = 0;
    FOR_EACH_IN_LIST(list_p) {
        ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
        if (i == elems_count - 1) {
            i = 0;
        }
        else {
            ++i;
        }
        ++pass_count;
    }
    ASSERT_EQ(2 * elems_count, pass_count);

    i = elems_count - 1;
    pass_count = 0;
    FOR_EACH_IN_LIST_BACK(list_p) {
        ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
        if (i == 0) {
            i = elems_count - 1;
        }
        else {
            --i;
        }
        ++pass_count;
    }
    ASSERT_EQ(2 * elems_count, pass_count);

    destroyListPointer(list_p);
    destroyListPointer(list_p_2);
}

TEST(ListTest, FindByVal) {
    List* list_p = (List*)malloc(sizeof(List));
    ASSERT_NE((void*)0, list_p);
    initList(list_p, double_comparator, free);

    double* el = NULL;
    for (size_t i = 0; i < elems_count; ++i) {
        el = (double*)malloc(sizeof(double));
        ASSERT_NE((void*)0, el);
        *el = i + 0.1;
        ASSERT_EQ(1, pushBack(list_p, el));
    }

    el = (double*)malloc(sizeof(double));
    ASSERT_NE((void*)0, el);
    *el = 0.1;
    ListIterator it = findByVal(list_p, el);
    free(el);
    ASSERT_EQ(list_p->head, it.current);
    ASSERT_EQ(0.1, *(double*)it.current->value);

    el = (double*)malloc(sizeof(double));
    ASSERT_NE((void*)0, el);
    *el = 2.1;
    it = findByVal(list_p, el);
    free(el);
    ASSERT_EQ(list_p->head->next->next, it.current);
    ASSERT_EQ(2.1, *(double*)it.current->value);

    el = (double*)malloc(sizeof(double));
    ASSERT_NE((void*)0, el);
    *el = elems_count - 1 + 0.1;
    it = findByVal(list_p, el);
    free(el);
    ASSERT_EQ(list_p->tail, it.current);
    ASSERT_EQ(elems_count - 1 + 0.1, *(double*)it.current->value);

    el = (double*)malloc(sizeof(double));
    ASSERT_NE((void*)0, el);
    *el = 5.5;
    it = findByVal(list_p, el);
    free(el);
    ASSERT_EQ((void*)0, it.current);

    destroyListPointer(list_p);
}

TEST(ListTest, FindByIndex) {
    List* list_p = (List*)malloc(sizeof(List));
    ASSERT_NE((void*)0, list_p);
    initList(list_p, NULL, free);

    double* el = NULL;
    for (size_t i = 0; i < elems_count; ++i) {
        el = (double*)malloc(sizeof(double));
        ASSERT_NE((void*)0, el);
        *el = i + 0.1;
        ASSERT_EQ(1, pushBack(list_p, el));
    }

    ListIterator it = findByIndex(list_p, 0);
    ASSERT_EQ(list_p->head, it.current);

    it = findByIndex(list_p, 3);
    ASSERT_EQ(list_p->head->next->next->next, it.current);

    it = findByIndex(list_p, elems_count - 1);
    ASSERT_EQ(list_p->tail, it.current);

    it = findByIndex(list_p, elems_count);
    ASSERT_EQ((void*)0, it.current);

    destroyListPointer(list_p);
}

TEST(ListTest, RemoveFromList) {
    List* list_p = (List*)malloc(sizeof(List));
    ASSERT_NE((void*)0, list_p);
    initList(list_p, NULL, free);

    double* el = NULL;
    for (size_t i = 0; i < elems_count; ++i) {
        el = (double*)malloc(sizeof(double));
        ASSERT_NE((void*)0, el);
        *el = i + 0.1;
        ASSERT_EQ(1, pushBack(list_p, el));
    }

    ASSERT_EQ(1, removeByIndex(list_p, 0));
    ASSERT_EQ(elems_count - 1, list_p->length);

    size_t i = 0;
    FOR_EACH_IN_LIST(list_p) {
        ASSERT_EQ(i + 1 + 0.1, LIST_IT_VALUE(double));
        ++i;
    }
    ASSERT_EQ(elems_count - 1, i);

    i = elems_count - 1;
    size_t pass_count = 0;
    FOR_EACH_IN_LIST_BACK(list_p) {
        ASSERT_EQ(i + 0.1, LIST_IT_VALUE(double));
        --i;
        ++pass_count;
    }
    ASSERT_EQ(elems_count - 1, pass_count);

    ASSERT_EQ(1, removeByIndex(list_p, elems_count - 2));
    ASSERT_EQ(elems_count - 2, list_p->length);

    i = 0;
    FOR_EACH_IN_LIST(list_p) {
        ASSERT_EQ(i + 1 + 0.1, LIST_IT_VALUE(double));
        ++i;
    }
    ASSERT_EQ(elems_count - 2, i);

    i = elems_count - 1;
    pass_count = 0;
    FOR_EACH_IN_LIST_BACK(list_p) {
        ASSERT_EQ(i - 1 + 0.1, LIST_IT_VALUE(double));
        --i;
        ++pass_count;
    }
    ASSERT_EQ(elems_count - 2, pass_count);

    ASSERT_EQ(1, removeByIndex(list_p, 2));
    ASSERT_EQ(elems_count - 3, list_p->length);

    i = 0;
    pass_count = 0;
    FOR_EACH_IN_LIST(list_p) {
        if (i == 2) {
            ++i;
        }
        ASSERT_EQ(i + 1 + 0.1, LIST_IT_VALUE(double));
        ++i;
        ++pass_count;
    }
    ASSERT_EQ(elems_count - 3, pass_count);

    i = elems_count - 1;
    pass_count = 0;
    FOR_EACH_IN_LIST_BACK(list_p) {
        if (i == 4) {
            --i;
        }
        ASSERT_EQ(i - 1 + 0.1, LIST_IT_VALUE(double));
        --i;
        ++pass_count;
    }
    ASSERT_EQ(elems_count - 3, pass_count);

    destroyListPointer(list_p);
}