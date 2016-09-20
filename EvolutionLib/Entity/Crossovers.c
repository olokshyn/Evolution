//
// Created by Oleg on 9/13/16.
//

#include "Crossovers.h"

#include "CrossoverLib.h"

void DHXCrossover(Entity* parent1,
                  Entity* parent2,
                  Entity* child1,
                  Entity* child2,
                  Objective* obj,
                  double fitness1,  // normalized
                  double fitness2,  // normalized
                  size_t chromosome_size,
                  size_t gen_numb,
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
            child1->chr[i] = dcF(gen_numb, max_generations_count,
                                 parent1->chr[i], parent2->chr[i],
                                 obj->min, obj->max);

            // Dynamic biased crossover (exploitation): right side
            child2->chr[i] = dbcM(gen_numb, max_generations_count,
                                  parent1->chr[i], parent2->chr[i],
                                  fitness1, fitness2);
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
            child1->chr[i] = dcS(gen_numb, max_generations_count,
                                 parent1->chr[i], parent2->chr[i],
                                 obj->min, obj->max);

            // Dynamic biased crossover (exploitation): left side
            child2->chr[i] = dbcM(gen_numb, max_generations_count,
                                  parent1->chr[i], parent2->chr[i],
                                  fitness1, fitness2);
        }
    }
    child1->fitness = obj->func(child1->chr, (int)chromosome_size);
    child1->old = 0;
    child2->fitness = obj->func(child2->chr, (int)chromosome_size);
    child2->old = 0;
}