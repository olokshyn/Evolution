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
    const size_t species_count = 4;
}

TEST(SpeciesTest, Creation) {
    Species* sp = CreateSpecies(initial_size);
    ASSERT_NE((void*)0, sp);
    ASSERT_EQ((size_t)0, SPECIES_LENGTH(sp));
    ASSERT_EQ(initial_size, sp->initial_size);
    ASSERT_EQ((size_t)0, sp->died);

    size_t i = 0;
    FOR_EACH_IN_SPECIES(sp) {
        ++i;
    }
    ASSERT_EQ((size_t)0, i);

    DestroySpecies(sp);
}

TEST(SpeciesTest, GetMidFitness) {
    Species* sp = MockCreateSpecies(entities_count, chr_size);
    ASSERT_NE((void*)0, sp);
    ASSERT_EQ(entities_count, SPECIES_LENGTH(sp));

    size_t i = 0;
    double fitness = 0.0;
    FOR_EACH_IN_SPECIES(sp) {
        fitness += ENTITIES_IT_P->fitness;
        ++i;
    }
    fitness /= SPECIES_LENGTH(sp);
    ASSERT_EQ(entities_count, i);

    ASSERT_EQ(GetMidFitness(sp), fitness);

    DestroySpecies(sp);
}

TEST(SpeciesTest, NormalizeSpeciesFitnesses) {
    SpeciesList* sp_list = CreateSpeciesList();
    ASSERT_NE((void*)0, sp_list);
    Species* sp = NULL;
    for (size_t i = 0; i < species_count; ++i) {
        sp = MockCreateSpecies(entities_count, chr_size, NULL);
        ASSERT_NE((void*)0, sp);
        ASSERT_EQ(1, pushBack(sp_list, sp));
    }

    List* fitness_list = NormalizeSpeciesFitnesses(sp_list);
    ASSERT_NE((void*)0, fitness_list);
    ASSERT_EQ(species_count, fitness_list->length);

    size_t i = 0;
    for (ListIterator sp_list_it = begin(sp_list),
                 ft_it = begin(fitness_list);
            !isIteratorExhausted(sp_list_it);
            next(&sp_list_it), next(&ft_it)) {
        ASSERT_FLOAT_EQ(GetMidFitness((Species*)sp_list_it.current->value),
                        *(double*)ft_it.current->value);
        ++i;
    }
    ASSERT_EQ(species_count, i);

    DestroySpeciesList(sp_list);
    destroyListPointer(fitness_list);
}