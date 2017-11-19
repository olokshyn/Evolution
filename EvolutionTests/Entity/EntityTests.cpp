//
// Created by Oleg on 9/2/16.
//

#include "gtest/gtest.h"

extern "C"
{
#include "Entity/Entity.h"
#include "Functions/TestFunctions.h"
}

#include "TestsCommon.h"

using namespace std;

namespace
{
    const size_t chr_size = 200;
    const size_t entities_count = 1000;
    Objective* obj = &RastriginFuncObjective;
}

TEST(EntityTest, Creation)
{
    Entity* entity = CreateEntity(chr_size);
    ASSERT_NE(nullptr, entity);
    DestroyEntity(entity);
}

TEST(EntityTest, Copy)
{
    Entity* en1 = CreateEntity(chr_size);
    ASSERT_NE(nullptr, en1);
    Entity* en2 = CopyEntity(en1, chr_size);
    ASSERT_NE(nullptr, en2);

    ASSERT_NE(en1, en2);
    ASSERT_NE(en1->chr, en2->chr);

    DestroyEntity(en1);
    DestroyEntity(en2);
}

TEST(EntityTest, EntityList)
{
    LIST_TYPE(EntityPtr) entities = list_create(EntityPtr);
    ASSERT_NE(nullptr, entities);

    for (size_t i = 0; i != entities_count; ++i)
    {
        Entity* temp = CreateEntity(chr_size);
        ASSERT_NE(nullptr, temp);
        ASSERT_TRUE(list_push_back(EntityPtr, entities, temp));
    }

    SetEntitiesStatus(entities, false);

    size_t i = 0;
    list_for_each(EntityPtr, entities, var)
    {
        ASSERT_FALSE(list_var_value(var)->old);
        ++i;
    }
    ASSERT_EQ(entities_count, i);

    SetEntitiesStatus(entities, true);

    list_for_each(EntityPtr, entities, var)
    {
        ASSERT_TRUE(list_var_value(var)->old);
    }

    DestroyEntitiesList(entities);
}

TEST(EntityTest, GetFitnesses)
{
    LIST_TYPE(EntityPtr) entities = MockCreateEntities(entities_count, chr_size);
    ASSERT_NE(nullptr, entities);
    ASSERT_EQ(entities_count, list_len(entities));

    size_t i = 0;
    double _mid_fitness = 0.0;
    double _min_fitness = list_first(entities)->fitness;
    double _max_fitness = list_first(entities)->fitness;
    list_for_each(EntityPtr, entities, var)
    {
        _mid_fitness += list_var_value(var)->fitness;
        if (list_var_value(var)->fitness < _min_fitness) {
            _min_fitness = list_var_value(var)->fitness;
        }
        if (list_var_value(var)->fitness > _max_fitness) {
            _max_fitness = list_var_value(var)->fitness;
        }
        ++i;
    }
    _mid_fitness /= list_len(entities);
    ASSERT_EQ(entities_count, i);

    double mid_fitness;
    double min_fitness;
    double max_fitness;
    GetFitnesses(entities, &mid_fitness, &min_fitness, &max_fitness);
    ASSERT_DOUBLE_EQ(_mid_fitness, mid_fitness);
    ASSERT_DOUBLE_EQ(_min_fitness, min_fitness);
    ASSERT_DOUBLE_EQ(_max_fitness, max_fitness);

    mid_fitness = 0.0;
    GetFitnesses(entities, &mid_fitness, NULL, NULL);
    ASSERT_DOUBLE_EQ(_mid_fitness, mid_fitness);

    min_fitness = 0.0;
    GetFitnesses(entities, NULL, &min_fitness, NULL);
    ASSERT_DOUBLE_EQ(_min_fitness, min_fitness);

    max_fitness = 0.0;
    GetFitnesses(entities, NULL, NULL, &max_fitness);
    ASSERT_DOUBLE_EQ(_max_fitness, max_fitness);

    DestroyEntitiesList(entities);
}

TEST(EntityTest, NormalizeEntitiesFitnesses)
{
    LIST_TYPE(EntityPtr) entities = list_create(EntityPtr);
    ASSERT_NE(nullptr, entities);

    for (size_t i = 0; i != entities_count; ++i)
    {
        Entity* temp = MockCreateEntity(chr_size, obj);
        ASSERT_NE(nullptr, temp);
        ASSERT_TRUE(list_push_back(EntityPtr, entities, temp));
    }

    LIST_TYPE(double) fitnesses = NormalizeEntitiesFitnesses(entities);
    ASSERT_NE(nullptr, fitnesses);
    ASSERT_EQ(list_len(entities), list_len(fitnesses));

    size_t i = 0;
    double sum = 0.0;
    list_for_each(double, fitnesses, var)
    {
        ++i;
        sum += list_var_value(var);

        ASSERT_LE(0.1, list_var_value(var));
        ASSERT_GE(0.9, list_var_value(var));
    }
    ASSERT_EQ(list_len(entities), i);

    list_destroy(double, fitnesses);
    DestroyEntitiesList(entities);
}