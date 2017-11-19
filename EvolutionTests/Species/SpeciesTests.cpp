//
// Created by Oleg on 9/10/16.
//

#include "gtest/gtest.h"

extern "C"
{
#include "Species/Species.h"
}

#include "TestsCommon.h"

using namespace std;

namespace
{
    const size_t initial_size = 1000;
    const size_t entities_count = 1000;
    const size_t chr_size = 1000;
    const size_t species_count = 4;
}

TEST(SpeciesTest, Creation)
{
    Species* sp = CreateSpecies(initial_size);
    ASSERT_NE(nullptr, sp);
    ASSERT_EQ(static_cast<size_t>(0), list_len(sp->entities));
    ASSERT_EQ(initial_size, sp->initial_size);
    ASSERT_EQ(static_cast<size_t>(0), sp->died);

    size_t i = 0;
    list_for_each(EntityPtr, sp->entities, var)
    {
        ++i;
    }
    ASSERT_EQ(static_cast<size_t>(0), i);

    DestroySpecies(sp);
}

TEST(SpeciesTest, NormalizeSpeciesFitnesses)
{
    LIST_TYPE(SpeciesPtr) population = list_create(SpeciesPtr);
    ASSERT_NE(nullptr, population);
    for (size_t i = 0; i < species_count; ++i)
    {
        Species* sp = MockCreateSpecies(entities_count, chr_size);
        ASSERT_NE(nullptr, sp);
        ASSERT_TRUE(list_push_back(SpeciesPtr, population, sp));
    }

    LIST_TYPE(double) fitnesses = NormalizePopulationFitnesses(population);
    ASSERT_NE(nullptr, fitnesses);
    ASSERT_EQ(species_count, list_len(fitnesses));

    size_t i = 0;
    LIST_ITER_TYPE(SpeciesPtr) it = list_begin(SpeciesPtr, population);
    LIST_ITER_TYPE(double) ft_it = list_begin(double, fitnesses);
    for (; list_iter_valid(it);
            list_next(SpeciesPtr, it), list_next(double, ft_it), ++i)
    {
        EXPECT_LE(0.1, list_iter_value(ft_it));
        EXPECT_GE(0.9, list_iter_value(ft_it));
    }
    ASSERT_EQ(species_count, i);

    list_destroy(double, fitnesses);
    DestroyPopulation(population);
}