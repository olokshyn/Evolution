//
// Created by Oleg on 9/13/16.
//

#include "gtest/gtest.h"

#include <utility>

extern "C"
{
#include "Entity/CrossoverLib.h"
#include "Entity/Crossovers.h"
}

#include "TestsCommon.h"

using namespace std;

namespace
{
    const double accuracy = 1e-10;
    const double a = -0.2;
    const double b = 1.0;
    const size_t chr_size = 100;
    const size_t individuals_size = 10;
    const size_t max_generations_count = 100;
    const Objective* obj = &square_objective;
}

TEST(CrossoverLibTest, dcF)
{
    for (size_t chr_numb = 0; chr_numb < chr_size; ++chr_numb)
    {
        double c1 = getRand(a, b);
        double c2 = getRand(a, b);

        double Ft1 = dcF(1, max_generations_count,
                         c1, c2, a, b);
        for (size_t gen_numb = 1; gen_numb < max_generations_count; ++gen_numb)
        {
            double Ft = Ft1;
            Ft1 = dcF(gen_numb + 1, max_generations_count,
                      c1, c2, a, b);
            ASSERT_LE(Ft, Ft1);
        }
        // Use float instead of double because we do not have enough accuracy
        ASSERT_FLOAT_EQ(MIN(c1, c2), Ft1);
    }
}

TEST(CrossoverLibTest, dcS)
{
    for (size_t chr_numb = 0; chr_numb < chr_size; ++chr_numb)
    {
        double c1 = getRand(a, b);
        double c2 = getRand(a, b);

        double St1 = dcS(1, max_generations_count,
                         c1, c2, a, b);
        for (size_t gen_numb = 1; gen_numb < max_generations_count; ++gen_numb)
        {
            double St = St1;
            St1 = dcS(gen_numb + 1, max_generations_count,
                      c1, c2, a, b);
            ASSERT_GE(St, St1);
        }
        // Use float instead of double because we do not have enough accuracy
        ASSERT_FLOAT_EQ(MAX(c1, c2), St1);
    }
}

TEST(CrossoverLibTest, dcMplus)
{
    for (size_t chr_numb = 0; chr_numb < chr_size; ++chr_numb)
    {
        double c1 = getRand(a, b);
        double c2 = getRand(a, b);

        double Mpt1 = dcMplus(1, max_generations_count,
                              c1, c2, a, b);
        for (size_t gen_numb = 1; gen_numb < max_generations_count; ++gen_numb)
        {
            double Mpt = Mpt1;
            Mpt1 = dcMplus(gen_numb + 1, max_generations_count,
                           c1, c2, a, b);
            // Use float instead of double because we do not have enough accuracy
            ASSERT_GE((float)Mpt, (float)Mpt1);
        }
        // Use float instead of double because we do not have enough accuracy
        ASSERT_FLOAT_EQ((c1 + c2) / 2, Mpt1);
    }
}

TEST(CrossoverLibTest, dcMminus)
{
    for (size_t chr_numb = 0; chr_numb < chr_size; ++chr_numb)
    {
        double c1 = getRand(a, b);
        double c2 = getRand(a, b);

        double Mmt1 = dcMminus(1, max_generations_count,
                               c1, c2, a, b);
        for (size_t gen_numb = 1; gen_numb < max_generations_count; ++gen_numb)
        {
            double Mmt = Mmt1;
            Mmt1 = dcMminus(gen_numb + 1, max_generations_count,
                            c1, c2, a, b);
            // Use float instead of double because we do not have enough accuracy
            ASSERT_LE((float)Mmt, (float)Mmt1);
        }
        // Use float instead of double because we do not have enough accuracy
        ASSERT_FLOAT_EQ((c1 + c2) / 2, Mmt1);
    }
}

TEST(CrossoverLibTest, dbcM)
{
    const size_t crossovers_count = 3;
    for (size_t crossovers_numb = 0;
            crossovers_numb < crossovers_count;
            ++crossovers_numb)
    {
        Entity* en1 = CreateEntity(chr_size);
        ASSERT_NE(nullptr, en1);
        Entity* en2 = CreateEntity(chr_size);
        ASSERT_NE(nullptr, en2);
        double min = -1;
        double max = 0;
        for (size_t i = 0; i < chr_size; ++i)
        {
            en1->chr[i] = getRand(min, max);
            en2->chr[i] = getRand(min, max);
        }
        double fitness1 = getRand(0, 1);
        double fitness2 = getRand(0, 1);

        for (size_t i = 0; i < chr_size; ++i)
        {
            double Mt1 = dbcM(1, max_generations_count,
                              en1->chr[i],
                              en2->chr[i],
                              fitness1, fitness2,
                              min, max);
            ASSERT_FLOAT_EQ((en1->chr[i] + en2->chr[i]) / 2, Mt1);
            for (size_t gen_numb = 1;
                    gen_numb < max_generations_count;
                    ++gen_numb)
            {
                double Mt = Mt1;
                Mt1 = dbcM(gen_numb + 1, max_generations_count,
                           en1->chr[i],
                           en2->chr[i],
                           fitness1, fitness2,
                           min, max);
                if ((fitness1 >= fitness2 && en1->chr[i] <= en2->chr[i])
                        || (fitness1 < fitness2 && en1->chr[i] >= en2->chr[i]))
                {
                    ASSERT_GE((float)Mt, (float)Mt1);

                }
                else
                {
                    ASSERT_LE((float)Mt, (float)Mt1);
                }
            }
            double lambda = fitness1 / (fitness1 + fitness2);
            ASSERT_FLOAT_EQ(lambda * en1->chr[i] + (1 - lambda) * en2->chr[i],
                            Mt1);
            ASSERT_GE(MAX(en1->chr[i], en2->chr[i]), Mt1);
            ASSERT_LE(MIN(en1->chr[i], en2->chr[i]), Mt1);
        }
        DestroyEntity(en1);
        DestroyEntity(en2);
    }
}

TEST(CrossoverTest, OnePointCrossover)
{
    LIST_TYPE(EntityPtr) entities = list_create(EntityPtr);
    ASSERT_NE(nullptr, entities);

    for (size_t i = 0; i < individuals_size; ++i)
    {
        Entity* temp = MockCreateEntity(chr_size, obj);
        ASSERT_NE(nullptr, temp);
        ASSERT_TRUE(list_push_back(EntityPtr, entities, temp));
    }

    Entity* child1 = MockCreateEntity(chr_size, obj);
    ASSERT_NE(nullptr, child1);
    Entity* child2 = MockCreateEntity(chr_size, obj);
    ASSERT_NE(nullptr, child2);

    list_for_each(EntityPtr, entities, var1)
    {
        list_for_each(EntityPtr, entities, var2)
        {
            if (list_var_eq(var1, var2))
            {
                continue;
            }

            Entity* parent1 = list_var_value(var1);
            Entity* parent2 = list_var_value(var2);
            OnePointCrossover(parent1,
                              parent2,
                              child1,
                              child2,
                              obj,
                              0.0,
                              0.0,
                              chr_size,
                              0,
                              0);

            size_t crossover_point = (chr_size % 2 == 0) ?
                                     (chr_size / 2) :
                                     (chr_size / 2 + 1);
            for (size_t i = 0; i < crossover_point; ++i)
            {
                ASSERT_EQ(parent1->chr[i], child1->chr[i]);
            }
            for (size_t i = crossover_point; i < chr_size; ++i)
            {
                ASSERT_EQ(parent2->chr[i], child1->chr[i]);
            }
            ASSERT_FLOAT_EQ(obj->func(child1->chr, chr_size),
                            child1->fitness);
            ASSERT_EQ(0, child1->old);

            for (size_t i = 0; i < crossover_point; ++i)
            {
                ASSERT_EQ(parent2->chr[i], child2->chr[i]);
            }
            for (size_t i = crossover_point; i < chr_size; ++i)
            {
                ASSERT_EQ(parent1->chr[i], child2->chr[i]);
            }
            ASSERT_FLOAT_EQ(obj->func(child2->chr, chr_size),
                            child2->fitness);
            ASSERT_EQ(0, child2->old);
        }
    }
    DestroyEntity(child2);
    DestroyEntity(child1);
    DestroyEntitiesList(entities);
}

TEST(CrossoverTest, DHXCrossover)
{
    Species* sp = MockCreateSpecies(individuals_size,
                                    chr_size,
                                    obj);
    ASSERT_NE(nullptr, sp);
    Entity* child1t = MockCreateEntity(chr_size, obj);
    ASSERT_NE(nullptr, child1t);
    Entity* child2t = MockCreateEntity(chr_size, obj);
    ASSERT_NE(nullptr, child2t);
    Entity* child1t1 = MockCreateEntity(chr_size, obj);
    ASSERT_NE(nullptr, child1t1);
    Entity* child2t1 = MockCreateEntity(chr_size, obj);
    ASSERT_NE(nullptr, child2t1);

    int* crossed = (int*)calloc(list_len(sp->entities), sizeof(int));
    ASSERT_NE(nullptr, crossed);

    LIST_TYPE(double) fitnesses = NormalizeEntitiesFitnesses(sp->entities);
    ASSERT_NE(nullptr, fitnesses);

    size_t i = 0;
    LIST_ITER_TYPE(EntityPtr) it1 = list_begin(EntityPtr, sp->entities);
    LIST_ITER_TYPE(double) ft_it1 = list_begin(double, fitnesses);
    for (; list_iter_valid(it1) && list_iter_valid(ft_it1);
            list_next(EntityPtr, it1), list_next(double, ft_it1), ++i)
    {
        if (crossed[i])
        {
            continue;
        }
        size_t j = 0;
        LIST_ITER_TYPE(EntityPtr) it2 = list_begin(EntityPtr, sp->entities);
        LIST_ITER_TYPE(double) ft_it2 = list_begin(double, fitnesses);
        for (; list_iter_valid(it2) && list_iter_valid(ft_it2) && !crossed[i];
                list_next(EntityPtr, it2), list_next(double, ft_it2), ++j)
        {
            if (list_iter_eq(it1, it2) || crossed[j])
            {
                continue;
            }
            Entity* parent1 = list_iter_value(it1);
            Entity* parent2 = list_iter_value(it2);
            double fitness1 = list_iter_value(ft_it1);
            double fitness2 = list_iter_value(ft_it2);

            DHXCrossover(parent1, parent2,
                         child1t1, child2t1,
                         obj,
                         fitness1, fitness2,
                         chr_size,
                         1, max_generations_count);
            for (size_t gen_numb = 1;
                    gen_numb < max_generations_count;
                    ++gen_numb)
            {
                swap(child1t, child1t1);
                swap(child2t, child2t1);
                DHXCrossover(parent1, parent2,
                             child1t1, child2t1,
                             obj,
                             fitness1, fitness2,
                             chr_size,
                             gen_numb + 1, max_generations_count);

                if (fitness1 >= fitness2)
                {
                    ASSERT_LE((float)EuclidMeasure(child1t1->chr,
                                                   parent1->chr,
                                                   chr_size),
                              (float)EuclidMeasure(child1t->chr,
                                                   parent1->chr,
                                                   chr_size));
                    ASSERT_LE((float)EuclidMeasure(child2t1->chr,
                                                   parent1->chr,
                                                   chr_size),
                              (float)EuclidMeasure(child2t->chr,
                                                   parent1->chr,
                                                   chr_size));
                }
                else
                {
                    ASSERT_LE((float)EuclidMeasure(child1t1->chr,
                                                   parent2->chr,
                                                   chr_size),
                              (float)EuclidMeasure(child1t->chr,
                                                   parent2->chr,
                                                   chr_size));
                    ASSERT_LE((float)EuclidMeasure(child2t1->chr,
                                                   parent2->chr,
                                                   chr_size),
                              (float)EuclidMeasure(child2t->chr,
                                                   parent2->chr,
                                                   chr_size));
                }
            }
            double lambda = MAX(fitness1, fitness2) / (fitness1 + fitness2);
            double parents_dist = EuclidMeasure(parent1->chr,
                                                parent2->chr,
                                                chr_size);
            if (fitness1 >= fitness2)
            {
                ASSERT_NEAR(0.0,
                            EuclidMeasure(child1t1->chr,
                                          parent1->chr,
                                          chr_size),
                            accuracy);
                ASSERT_NEAR(1 - lambda,
                            EuclidMeasure(child2t1->chr,
                                          parent1->chr,
                                          chr_size) / parents_dist,
                            accuracy);
            }
            else
            {
                ASSERT_NEAR(0.0,
                            EuclidMeasure(child1t1->chr,
                                          parent2->chr,
                                          chr_size),
                            accuracy);
                ASSERT_NEAR(1 - lambda,
                            EuclidMeasure(child2t1->chr,
                                          parent2->chr,
                                          chr_size) / parents_dist,
                            accuracy);
            }
            ASSERT_FLOAT_EQ(obj->func(child1t1->chr, chr_size),
                            child1t1->fitness);
            ASSERT_EQ(0, child1t1->old);
            ASSERT_FLOAT_EQ(obj->func(child2t1->chr, chr_size),
                            child2t1->fitness);
            ASSERT_EQ(0, child1t1->old);

            crossed[i] = 1;
            crossed[j] = 1;
        }
    }

    list_destroy(double, fitnesses);
    free(crossed);
    DestroyEntity(child2t1);
    DestroyEntity(child1t1);
    DestroyEntity(child2t);
    DestroyEntity(child1t);
    DestroySpecies(sp);

}