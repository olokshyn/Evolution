//
// Created by Oleg on 9/27/16.
//

#include "GALib.h"

#include <math.h>

#include "GAParameters.h"
#include "Common.h"
#include "Logging/Logging.h"

int CountSpeciesLinks(List* fitness_list) {
    if (SPECIES_LINK_PROBABILITY == 0.0) {
        return 1;
    }

    int* counted = (int*)calloc(fitness_list->length, sizeof(int));
    if (!counted) {
        return 0;
    }

    size_t i = 0;
    for (ListIterator it1 = begin(fitness_list);
            !isIteratorExhausted(it1);
            next(&it1), ++i) {
        if (counted[i]) {
            continue;
        }
        size_t j = 0;
        for (ListIterator it2 = begin(fitness_list);
                !isIteratorExhausted(it2) && !counted[i];
                next(&it2), ++j) {
            if (it1.current == it2.current
                    || counted[j]
                    || !doWithProbability(SPECIES_LINK_PROBABILITY)) {
                continue;
            }
            *((double*)it1.current->value) +=
                    getRand(SPECIES_LINK_MIN, SPECIES_LINK_MAX)
                    *  *((double*)it2.current->value);
            counted[i] = 1;
            counted[j] = 1;
        }
        *((double*)it1.current->value) = MAX(*((double*)it1.current->value),
                                             1.0);
    }
    free(counted);

    Normalize(fitness_list);
    return 1;
}

double NonUniformMutationDelta(size_t t, double y,
                               const GAParameters* parameters) {
    double r = getRand(0.0, 1.0);
    return y * (1 - pow(r,
                        pow(1 - t / (double)parameters->max_generations_count,
                            parameters->mutation_on_iteration_dependence)));
}