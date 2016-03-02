//
// Created by Oleg on 12/5/15.
//

#include <stdio.h>
#include <assert.h>
#include <time.h>

#include "List/ListTests.h"

#include "Wishart/WishartTest.h"
#include "GeneticAlgorithm/Simulation.h"


void runForAllFucntions(size_t iterations_count,
                        size_t individuals_count,
                        size_t chromosome_size,
                        double mutation_probability,
                        size_t k_neighbour,
                        double cluster_height) {
    const char* functionNames[] = {
            "De Jong`s F1 function",
            "De Jong`s F2 function",
            "De Jong`s F3 function",
            "De Jong`s F4 function",
            "De Jong`s F5 function",
            "Rastrigin`s function",
            "Schwefel`s function",
            "Griewangk`s function",
            "Stretched V sine wave function",
            "Ackley`s function",
            "EggHolder function",
            "Rana`s function",
            "Pathological test function"
    };

    Objective objectiveFunctions[] = {
            DeJongF1Objective,
            DeJongF2Objective,
            DeJongF3Objective,
            DeJongF4Objective,
            DeJongF5Objective,
            RastriginFuncObjective,
            SchwefelFuncObjective,
            GriewangkFuncObjective,
            StretchedVSineWaveFuncObjective,
            AckleyFuncObjective,
            EggHolderFuncObjective,
            RanaFuncObjective,
            PathologicalFuncObjective
    };

    assert(sizeof(functionNames) / sizeof(char*)
           == sizeof(objectiveFunctions) / sizeof(Objective));

    size_t functionsCount = sizeof(functionNames) / sizeof(char*);

    for (size_t i = 0; i < functionsCount; ++i) {
        printf("%s\n", functionNames[i]);
        RunSimulation(iterations_count,
                      individuals_count,
                      chromosome_size,
                      mutation_probability,
                      k_neighbour,
                      cluster_height,
                      objectiveFunctions[i],  // Objective function
                      NULL,
                      0);
        printf("\n");
    }
}

double getAvgIterCount(size_t tests_count,
                       size_t iterations_count,
                       size_t individuals_count,
                       size_t chromosome_size,
                       double mutation_probability,
                       size_t k_neighbour,
                       double cluster_height,
                       Objective objective) {
    size_t success_iterations_count = 0;
    size_t all_iterations = 0;
    size_t iterations_made;
    int status;
    for (size_t i = 0; i < tests_count; ++i) {
        iterations_made = 0;
        status = RunSimulation(iterations_count,
                               individuals_count,
                               chromosome_size,
                               mutation_probability,
                               k_neighbour,
                               cluster_height,
                               objective,
                               &iterations_made,
                              0);
        if (GetLastError()) {
            return -1.0;
        }
        if (status) {
            ++success_iterations_count;
            all_iterations += iterations_made;
        }
    }
    if (success_iterations_count > 0) {
        return (double)all_iterations / success_iterations_count;
    }
    return -1;
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));


    // runAllListTests();
    // four_points_test();
    // eight_points_test();
    // twelve_points_test();
    // fisher_iris_test();

    size_t iterations_count = 2000;
    size_t individuals_count = 100;
    size_t chromosome_size = 2;
    double mutation_probability = 0.5;
    size_t k_neighbour = 5;
    double cluster_height = 0.4;

//    runForAllFucntions(iterations_count,
//                       individuals_count,
//                       chromosome_size,
//                       mutation_probability,
//                       k_neighbour,
//                       cluster_height);

    double avg = getAvgIterCount(5,
                                 iterations_count,
                                 individuals_count,
                                 chromosome_size,
                                 mutation_probability,
                                 k_neighbour,
                                 cluster_height,
                                 SchwefelFuncObjective);
    printf("Average iterations count: %.3f\n", avg);

    return 0;
}