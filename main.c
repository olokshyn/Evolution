//
// Created by Oleg on 12/5/15.
//

#include <stdio.h>
#include <assert.h>

#include "List/ListTests.h"

#include "Wishart/WishartTest.h"
#include "GeneticAlgorithm/Simulation.h"


int main(int argc, char* argv[]) {
    // runAllListTests();
    // four_points_test();
    // eight_points_test();
    // twelve_points_test();
    // fisher_iris_test();

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
        RunSimulation(1000,  // iterations_count
                      100,  // individuals_count
                      5,  // chromosome_size
                      0.5,  // mutation_probability
                      2,  // k_neighbour
                      0.2, // cluster_height
                      objectiveFunctions[i]  // Objective function
                     );
        printf("\n");
    }
}