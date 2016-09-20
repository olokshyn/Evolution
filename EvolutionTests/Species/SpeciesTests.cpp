//
// Created by Oleg on 9/10/16.
//

#include "gtest/gtest.h"

extern "C" {
#include "Species/Species.h"
}

#include "TestsCommon.h"

using namespace std;

namespace {
    const size_t initial_size = 1000;
    const size_t entities_count = 1000;
    const size_t chr_size = 1000;
}

TEST(SpeciesTest, Creation) {
    Species* sp = CreateSpecies(initial_size);
    ASSERT_NE((void*)0, sp);
    ASSERT_EQ(0, SPECIES_LENGTH(sp));
    ASSERT_EQ(initial_size, sp->initial_size);
    ASSERT_EQ(0, sp->died);

    size_t i = 0;
    FOR_EACH_IN_SPECIES(sp) {
        ++i;
    }
    ASSERT_EQ(0, i);

    ClearSpecies(sp);
}

TEST(SpeciesTest, GetMidFitness) {
    Species* sp = MockCreateSpecies(entities_count, chr_size);
    ASSERT_NE((void*)0, sp);
    ASSERT_EQ(entities_count, SPECIES_LENGTH(sp));

    size_t i = 0;
    double fitness = 0.0;
    FOR_EACH_IN_SPECIES(sp) {
        fitness += ENTITY_SP_IT->fitness;
        ++i;
    }
    fitness /= SPECIES_LENGTH(sp);
    ASSERT_EQ(entities_count, i);

    ASSERT_EQ(GetMidFitness(sp), fitness);

    ClearSpecies(sp);
}