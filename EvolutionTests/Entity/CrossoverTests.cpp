//
// Created by Oleg on 9/13/16.
//

#include "gtest/gtest.h"

#include <utility>

extern "C" {
#include "Entity/CrossoverLib.h"
#include "Entity/Crossovers.h"
#include "Functions/TestFunctions.h"
}

#include "TestsCommon.h"

using namespace std;

namespace {
    const double accuracy = 1e-10;
    const double a = -0.2;
    const double b = 1.0;
    const size_t chr_size = 100;
    const size_t individuals_size = 10;
    const size_t max_generations_count = 100;
    Objective* obj = &RastriginFuncObjective;
}

TEST(CrossoverLibTest, dcF) {
    for (size_t chr_numb = 0; chr_numb < chr_size; ++chr_numb) {
        double c1 = getRand(a, b);
        double c2 = getRand(a, b);

        double Ft1 = dcF(1, max_generations_count,
                         c1, c2, a, b);
        for (size_t gen_numb = 1; gen_numb < max_generations_count; ++gen_numb) {
            double Ft = Ft1;
            Ft1 = dcF(gen_numb + 1, max_generations_count,
                      c1, c2, a, b);
            ASSERT_LE(Ft, Ft1);
        }
        // Use float instead of double because we do not have enough accuracy
        ASSERT_FLOAT_EQ(MIN(c1, c2), Ft1);
    }
}

TEST(CrossoverLibTest, dcS) {
    for (size_t chr_numb = 0; chr_numb < chr_size; ++chr_numb) {
        double c1 = getRand(a, b);
        double c2 = getRand(a, b);

        double St1 = dcS(1, max_generations_count,
                         c1, c2, a, b);
        for (size_t gen_numb = 1; gen_numb < max_generations_count; ++gen_numb) {
            double St = St1;
            St1 = dcS(gen_numb + 1, max_generations_count,
                      c1, c2, a, b);
            ASSERT_GE(St, St1);
        }
        // Use float instead of double because we do not have enough accuracy
        ASSERT_FLOAT_EQ(MAX(c1, c2), St1);
    }
}

TEST(CrossoverLibTest, dcMplus) {
    for (size_t chr_numb = 0; chr_numb < chr_size; ++chr_numb) {
        double c1 = getRand(a, b);
        double c2 = getRand(a, b);

        double Mpt1 = dcMplus(1, max_generations_count,
                              c1, c2, a, b);
        for (size_t gen_numb = 1; gen_numb < max_generations_count; ++gen_numb) {
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

TEST(CrossoverLibTest, dcMminus) {
    for (size_t chr_numb = 0; chr_numb < chr_size; ++chr_numb) {
        double c1 = getRand(a, b);
        double c2 = getRand(a, b);

        double Mmt1 = dcMminus(1, max_generations_count,
                               c1, c2, a, b);
        for (size_t gen_numb = 1; gen_numb < max_generations_count; ++gen_numb) {
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

TEST(CrossoverLibTest, dbcM) {
    const size_t crossovers_count = 3;
    for (size_t crossovers_numb = 0;
            crossovers_numb < crossovers_count;
            ++crossovers_numb) {
        Entity* en1 = CreateEntity(chr_size);
        ASSERT_NE((void*)0, en1);
        Entity* en2 = CreateEntity(chr_size);
        ASSERT_NE((void*)0, en2);
        for (size_t i = 0; i < chr_size; ++i) {
            en1->chr[i] = getRand(-1, 0);
            en2->chr[i] = getRand(-1, 0);
        }
        double fitness1 = getRand(0, 1);
        double fitness2 = getRand(0, 1);

        for (size_t i = 0; i < chr_size; ++i) {
            double Mt1 = dbcM(1, max_generations_count,
                              en1->chr[i],
                              en2->chr[i],
                              fitness1, fitness2);
            ASSERT_FLOAT_EQ((en1->chr[i] + en2->chr[i]) / 2, Mt1);
            for (size_t gen_numb = 1;
                    gen_numb < max_generations_count;
                    ++gen_numb) {
                double Mt = Mt1;
                Mt1 = dbcM(gen_numb + 1, max_generations_count,
                           en1->chr[i],
                           en2->chr[i],
                           fitness1, fitness2);
                if ((fitness1 >= fitness2 && en1->chr[i] <= en2->chr[i])
                        || (fitness1 < fitness2 && en1->chr[i] >= en2->chr[i])) {
                    ASSERT_GE((float)Mt, (float)Mt1);

                }
                else {
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

TEST(CrossoverTest, OnePointCrossover) {
    List* en_list = CreateEntitiesList();
    ASSERT_NE((void*)0, en_list);

    for (size_t i = 0; i < individuals_size; ++i) {
        Entity* temp = MockCreateEntity(chr_size, obj);
        ASSERT_NE((void*)0, temp);
        ASSERT_EQ(1, pushBack(en_list, temp));
    }

    Entity* child1 = MockCreateEntity(chr_size, obj);
    ASSERT_NE((void*)0, child1);
    Entity* child2 = MockCreateEntity(chr_size, obj);
    ASSERT_NE((void*)0, child2);

    FOR_EACH_IN_LIST_N(en_list, it1) {
        FOR_EACH_IN_LIST_N(en_list, it2) {
            if (LIST_IT_VALUE_P_N(it1, Entity)
                == LIST_IT_VALUE_P_N(it2, Entity)) {
                continue;
            }

            Entity* parent1 = LIST_IT_VALUE_P_N(it1, Entity);
            Entity* parent2 = LIST_IT_VALUE_P_N(it2, Entity);
            OnePointCrossover(parent1,
                              parent2,
                              child1,
                              child2,
                              obj,
                              chr_size);

            size_t crossover_point = (chr_size % 2 == 0) ?
                                     (chr_size / 2) :
                                     (chr_size / 2 + 1);
            for (size_t i = 0; i < crossover_point; ++i) {
                ASSERT_EQ(parent1->chr[i], child1->chr[i]);
            }
            for (size_t i = crossover_point; i < chr_size; ++i) {
                ASSERT_EQ(parent2->chr[i], child1->chr[i]);
            }
            ASSERT_FLOAT_EQ(obj->func(child1->chr, static_cast<int>(chr_size)),
                            child1->fitness);
            ASSERT_EQ(0, child1->old);

            for (size_t i = 0; i < crossover_point; ++i) {
                ASSERT_EQ(parent2->chr[i], child2->chr[i]);
            }
            for (size_t i = crossover_point; i < chr_size; ++i) {
                ASSERT_EQ(parent1->chr[i], child2->chr[i]);
            }
            ASSERT_FLOAT_EQ(obj->func(child2->chr, static_cast<int>(chr_size)),
                            child2->fitness);
            ASSERT_EQ(0, child2->old);
        }
    }

    DestroyEntity(child1);
    DestroyEntity(child2);
    destroyListPointer(en_list);
}

TEST(CrossoverTest, DHXCrossover) {
    Species* sp = MockCreateSpecies(individuals_size,
                                    chr_size,
                                    obj);
    ASSERT_NE((void*)0, sp);
    Entity* child1t = MockCreateEntity(chr_size, obj);
    ASSERT_NE((void*)0, child1t);
    Entity* child2t = MockCreateEntity(chr_size, obj);
    ASSERT_NE((void*)0, child2t);
    Entity* child1t1 = MockCreateEntity(chr_size, obj);
    ASSERT_NE((void*)0, child1t1);
    Entity* child2t1 = MockCreateEntity(chr_size, obj);
    ASSERT_NE((void*)0, child2t1);

    int* crossed = (int*)calloc(SPECIES_LENGTH(sp), sizeof(int));
    ASSERT_NE((void*)0, crossed);

    List* fitness_list = NormalizeEntitiesFitnesses(sp->entitiesList);
    ASSERT_NE((void*)0, fitness_list);

    size_t i = 0;
    for (ListIterator it1 = begin(sp->entitiesList),
                 ft_it1 = begin(fitness_list);
            !isIteratorExhausted(it1) && !isIteratorExhausted(ft_it1);
            next(&it1), next(&ft_it1), ++i) {
        if (crossed[i]) {
            continue;
        }
        size_t j = 0;
        for (ListIterator it2 = begin(sp->entitiesList),
                     ft_it2 = begin(fitness_list);
                !isIteratorExhausted(it2)
                    && !isIteratorExhausted(ft_it2)
                    && !crossed[i];
                next(&it2), next(&ft_it2), ++j) {
            if (it1.current == it2.current || crossed[j]) {
                continue;
            }
            Entity* parent1 = (Entity*)it1.current->value;
            Entity* parent2 = (Entity*)it2.current->value;
            double fitness1 = *(double*)ft_it1.current->value;
            double fitness2 = *(double*)ft_it2.current->value;

            DHXCrossover(parent1, parent2,
                         child1t1, child2t1,
                         obj,
                         fitness1, fitness2,
                         chr_size,
                         1, max_generations_count);
            for (size_t gen_numb = 1;
                    gen_numb < max_generations_count;
                    ++gen_numb) {
                swap(child1t, child1t1);
                swap(child2t, child2t1);
                DHXCrossover(parent1, parent2,
                             child1t1, child2t1,
                             obj,
                             fitness1, fitness2,
                             chr_size,
                             gen_numb + 1, max_generations_count);

                if (fitness1 >= fitness2) {
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
                else {
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
            if (fitness1 >= fitness2) {
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
            else {
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
            ASSERT_FLOAT_EQ(obj->func(child1t1->chr, (int)chr_size),
                            child1t1->fitness);
            ASSERT_EQ(0, child1t1->old);
            ASSERT_FLOAT_EQ(obj->func(child2t1->chr, (int)chr_size),
                            child2t1->fitness);
            ASSERT_EQ(0, child1t1->old);

            crossed[i] = 1;
            crossed[j] = 1;
        }
    }

    DestroySpecies(sp);
    DestroyEntity(child1t);
    DestroyEntity(child2t);
    DestroyEntity(child1t1);
    DestroyEntity(child2t1);
    free(crossed);
    destroyListPointer(fitness_list);
}