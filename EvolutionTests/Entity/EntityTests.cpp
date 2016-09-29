//
// Created by Oleg on 9/2/16.
//

#include "gtest/gtest.h"

extern "C" {
#include "Entity/Entity.h"
#include "Functions/TestFunctions.h"
}

#include "TestsCommon.h"

using namespace std;

namespace {
    const size_t chr_size = 200;
    const size_t entities_count = 1000;
    Objective* obj = &RastriginFuncObjective;
}

TEST(EntityTest, Creation) {
    Entity* entity = CreateEntity(chr_size);
    ASSERT_NE((void*)0, entity);
    DestroyEntity(entity);
}

TEST(EntityTest, Copy) {
    Entity* en1 = CreateEntity(chr_size);
    ASSERT_NE((void*)0, en1);
    Entity* en2 = CopyEntity(en1, chr_size);
    ASSERT_NE((void*)0, en2);

    ASSERT_NE(en1, en2);
    ASSERT_NE(en1->chr, en2->chr);

    DestroyEntity(en1);
    DestroyEntity(en2);
}

TEST(EntityTest, EntityList) {
    EntitiesList* en_list = CreateEntitiesList();
    ASSERT_NE((void*)0, en_list);

    for (size_t i = 0; i < entities_count; ++i) {
        Entity* temp = CreateEntity(chr_size);
        ASSERT_NE((void*)0, temp);
        ASSERT_EQ(1, pushBack(en_list, temp));
    }

    MarkAllAsNew(en_list);

    size_t i = 0;
    FOR_EACH_IN_ENTITIES(en_list) {
        ASSERT_EQ(0, ENTITIES_IT_P->old);
        ++i;
    }
    ASSERT_EQ(entities_count, i);

    MarkAllAsOld(en_list);

    FOR_EACH_IN_ENTITIES(en_list) {
        ASSERT_EQ(1, ENTITIES_IT_P->old);
    }

    DestroyEntitiesList(en_list);
}

TEST(EntityTest, NormalizeEntitiesFitnesses) {
    EntitiesList* en_list = CreateEntitiesList();
    ASSERT_NE((void*)0, en_list);

    for (size_t i = 0; i < entities_count; ++i) {
        Entity* temp = MockCreateEntity(chr_size, obj);
        ASSERT_NE((void*)0, temp);
        ASSERT_EQ(1, pushBack(en_list, temp));
    }

    List* fitness_list = NormalizeEntitiesFitnesses(en_list);
    ASSERT_NE((void*)0, fitness_list);
    ASSERT_EQ(en_list->length, fitness_list->length);

    size_t i = 0;
    double sum = 0.0;
    FOR_EACH_IN_LIST(fitness_list) {
        ++i;
        sum += LIST_IT_VALUE(double);
    }
    ASSERT_EQ(en_list->length, i);

    ASSERT_FLOAT_EQ(1.0, sum);

    destroyListPointer(fitness_list);
    DestroyEntitiesList(en_list);
}