#include "Header.h"

#define INDIVIDUALS 100
#define MUTATION_PROB 0.5
#define ITERATIONS_COUNT 10000

int main(int argc, char* argv[]) {
    World world;
    int k = 0;
    double max_fitness = 0, cur_fitness = 0;
    CreateWorld(&world,
                INDIVIDUALS,
                -100,
                100,
                PathologicalFunc);
    if (GetLastError()) {
        printf("Error creating world\n");
        goto end;
    }
    max_fitness = GetMaxFitness(&world);
    cur_fitness = max_fitness;
    printf("Initial fitness: %.3f\n\n", max_fitness);

    for (int i = 0; i < ITERATIONS_COUNT; i++) {
        cur_fitness = Step(&world, MUTATION_PROB);
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
    system("pause");
    return GetLastError();
}