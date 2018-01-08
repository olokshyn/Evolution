//
// Created by Oleg on 9/13/16.
//

#include "Crossovers.h"

#include "CrossoverLib.h"
#include "Logging/Logging.h"

bool OnePointCrossover(const Entity* parent1,
                       const Entity* parent2,
                       Entity* child1,
                       Entity* child2,
                       const Objective* obj,
                       double fitness1,  // normalized
                       double fitness2,  // normalized
                       size_t chromosome_size,
                       size_t generation_number,
                       size_t max_generations_count) {
    size_t crossover_point = (chromosome_size % 2 == 0) ?
                             (chromosome_size / 2) :
                             (chromosome_size / 2 + 1);
    for (size_t i = 0; i < crossover_point; ++i) {
        child1->chr[i] = parent1->chr[i];
    }
    for (size_t i = crossover_point; i < chromosome_size; ++i) {
        child1->chr[i] = parent2->chr[i];
    }
    child1->fitness = obj->func(child1->chr, chromosome_size);
    child1->old = 0;

    for (size_t i = 0; i < crossover_point; ++i) {
        child2->chr[i] = parent2->chr[i];
    }
    for (size_t i = crossover_point; i < chromosome_size; ++i) {
        child2->chr[i] = parent1->chr[i];
    }
    child2->fitness = obj->func(child2->chr, chromosome_size);
    child2->old = 0;

    return true;
}

bool DHXCrossover(const Entity* parent1,
                  const Entity* parent2,
                  Entity* child1,
                  Entity* child2,
                  const Objective* obj,
                  double fitness1,  // normalized
                  double fitness2,  // normalized
                  size_t chromosome_size,
                  size_t generation_number,
                  size_t max_generations_count) {
    LOG_ASSERT(parent1);
    LOG_ASSERT(parent2);
    LOG_ASSERT(child1);
    LOG_ASSERT(child2);
    LOG_ASSERT(obj);

    for (size_t i = 0; i < chromosome_size; ++i) {
        if ((parent1->fitness >= parent2->fitness
             && parent1->chr[i] <= parent2->chr[i])
                || (parent1->fitness < parent2->fitness
                    && parent1->chr[i] >= parent2->chr[i])) {
            // Converge to the left gen from both sides

            // Dynamic dominated crossover (exploration): left side
            child1->chr[i] = dcF(generation_number, max_generations_count,
                                 parent1->chr[i], parent2->chr[i],
                                 obj->min, obj->max);

            // Dynamic biased crossover (exploitation): right side
            child2->chr[i] = dbcM(generation_number, max_generations_count,
                                  parent1->chr[i], parent2->chr[i],
                                  fitness1, fitness2,
                                  obj->min, obj->max);
            LOG_ASSERT(child1->chr[i] == child1->chr[i]
                       && child2->chr[i] == child2->chr[i]);
        }
        /*
         * parent1->fitness >= parent2->fitness
         * && parent1->chr[i] > parent2->chr[i]
         * ||
         * parent1->fitness < parent2->fitness
         * && parent1->chr[i] < parent2->chr[i]
         * */
        else {
            // Converge to the right gen from both sides

            // Dynamic dominated crossover (exploration): right side
            child1->chr[i] = dcS(generation_number, max_generations_count,
                                 parent1->chr[i], parent2->chr[i],
                                 obj->min, obj->max);

            // Dynamic biased crossover (exploitation): left side
            child2->chr[i] = dbcM(generation_number, max_generations_count,
                                  parent1->chr[i], parent2->chr[i],
                                  fitness1, fitness2,
                                  obj->min, obj->max);
            LOG_ASSERT(child1->chr[i] == child1->chr[i]
                       && child2->chr[i] == child2->chr[i]);
        }
    }
    child1->fitness = obj->func(child1->chr, chromosome_size);
    child1->old = 0;
    child2->fitness = obj->func(child2->chr, chromosome_size);
    child2->old = 0;

    return true;
}