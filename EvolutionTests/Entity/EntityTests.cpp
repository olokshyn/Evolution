//
// Created by Oleg on 9/2/16.
//

#include "gtest/gtest.h"

extern "C" {
#include "Entity/Entity.h"
}

using namespace std;

namespace {
    const size_t chr_size = 200;
    const size_t entities_count = 1000;
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

    for (size_t i = 0; i < entities_count; ++i) {
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