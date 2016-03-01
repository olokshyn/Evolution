#include "Simulation.h"


int RunSimulation(size_t iterations_count,
                   size_t individuals_count,
                   size_t chromosome_size,
                   double mutation_probability,
                   double min_limit,
                   double max_limit,
                   Objective Ofunc,
                   size_t k_neighbour,
                   double cluster_height) {
    int k = 0;
    double max_fitness = 0, cur_fitness = 0;
    World world;
    CreateWorld(&world,
                individuals_count,
                chromosome_size,
                mutation_probability,
                min_limit,
                max_limit,
                Ofunc,
                k_neighbour,
                cluster_height);
    if (GetLastError()) {
        printf("Error creating world\n");
        goto end;
    }
    max_fitness = GetMaxFitness(&world);
    cur_fitness = max_fitness;
    printf("Initial fitness: %.3f\n\n", max_fitness);

    for (int i = 0; i < iterations_count; ++i) {
        cur_fitness = Step(&world);
        if (GetLastError()) {
            printf("Error occured, terminating...\n");
            goto end;
        }
        k++;
        if (cur_fitness - max_fitness > 0.001) {
            printf("%d\t%d\t%.3f\n", k, i, cur_fitness);
            k = 0;
            max_fitness = cur_fitness;
        }
    }

end:
    printf("---\nResult:\n%d\t%.3f\n", k, cur_fitness);
    ClearWorld(&world);
    return GetLastError();
}