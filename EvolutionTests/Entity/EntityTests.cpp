//
// Created by Oleg on 9/2/16.
//

#include "gtest/gtest.h"

extern "C" {
#include "Common.h"
#include "Entity/Entity.h"
}

using namespace std;

namespace {
    const size_t chr_size = 200;
    const size_t elems_count = 1000;

    double TestObjective(double* args, int size) {
        double sum = 0.0;
        for (size_t i = 0; i < size; ++i) {
            sum += args[i];
        }
        return sum;
    }

    int ConfirmParents(Entity* parent1,
                       Entity* parent2,
                       Entity* child1,
                       Entity* child2,
                       size_t chr_size) {
        size_t crossover_point = (chr_size % 2 == 0) ?
                                 (chr_size / 2) :
                                 (chr_size / 2 + 1);
        for (size_t i = 0; i < crossover_point; ++i) {
            if (parent1->chr[i] != child1->chr[i]) {
                return -1;
            }
        }
        for (size_t i = crossover_point; i < chr_size; ++i) {
            if (parent2->chr[i] != child1->chr[i]) {
                return -2;
            }
        }
        if (child1->fitness != TestObjective(child1->chr,
                                           static_cast<int>(chr_size))) {
            return -3;
        }
        if (child1->old != 0) {
            return -4;
        }

        for (size_t i = 0; i < crossover_point; ++i) {
            if (parent2->chr[i] != child2->chr[i]) {
                return -5;
            }
        }
        for (size_t i = crossover_point; i < chr_size; ++i) {
            if (parent1->chr[i] != child2->chr[i]) {
                return -6;
            }
        }
        if (child2->fitness != TestObjective(child2->chr,
                                             static_cast<int>(chr_size))) {
            return -7;
        }
        if (child2->old != 0) {
            return -8;
        }
        return 0;
    }
}

TEST(EntityTest, Creation) {
    Entity* entity = CreateEntity(chr_size);
    ASSERT_NE((void*)0, entity);
    EntityDestructor(entity);
}

TEST(EntityTest, Copy) {
    Entity* en1 = CreateEntity(chr_size);
    ASSERT_NE((void*)0, en1);
    Entity* en2 = CopyEntity(en1, chr_size);
    ASSERT_NE((void*)0, en2);

    ASSERT_NE(en1, en2);
    ASSERT_NE(en1->chr, en2->chr);

    EntityDestructor(en1);
    EntityDestructor(en2);
}

TEST(EntityTest, EntityList) {
    List* en_list = CreateEntitiesList();
    ASSERT_NE((void*)0, en_list);

    for (size_t i = 0; i < elems_count; ++i) {
        Entity* temp = CreateEntity(chr_size);
        ASSERT_NE((void*)0, temp);
        ASSERT_EQ(1, pushBack(en_list, temp));
    }

    MarkAllAsNew(en_list);

    FOR_EACH_IN_LIST(en_list) {
        ASSERT_EQ(0, LIST_IT_VALUE_P(Entity)->old);
    }

    MarkAllAsOld(en_list);

    FOR_EACH_IN_LIST(en_list) {
        ASSERT_EQ(1, LIST_IT_VALUE_P(Entity)->old);
    }

    clearListPointer(en_list);
}

TEST(EntityTest, CrossEntities) {
    List* en_list = CreateEntitiesList();
    ASSERT_NE((void*)0, en_list);

    for (size_t i = 0; i < elems_count / 2; ++i) {
        Entity* temp = CreateEntity(chr_size);
        ASSERT_NE((void*)0, temp);
        for (size_t c = 0; c < chr_size; ++c) {
            temp->chr[c] = getRand(0, 1);
        }
        ASSERT_EQ(1, pushBack(en_list, temp));
    }

    FOR_EACH_IN_LIST_N(en_list, it1) {
        FOR_EACH_IN_LIST_N(en_list, it2) {
            if (LIST_IT_VALUE_P_N(it1, Entity)
                == LIST_IT_VALUE_P_N(it2, Entity)) {
                continue;
            }
            Entity* child1 = CreateEntity(chr_size);
            ASSERT_NE((void*)0, child1);
            Entity* child2 = CreateEntity(chr_size);
            ASSERT_NE((void*)0, child2);

            CrossEntities(LIST_IT_VALUE_P_N(it1, Entity),
                          LIST_IT_VALUE_P_N(it2, Entity),
                          child1,
                          child2,
                          TestObjective,
                          chr_size);
            ASSERT_EQ(0, ConfirmParents(LIST_IT_VALUE_P_N(it1, Entity),
                                        LIST_IT_VALUE_P_N(it2, Entity),
                                        child1,
                                        child2,
                                        chr_size));

            EntityDestructor(child1);
            EntityDestructor(child2);
        }
    }

    clearListPointer(en_list);
}