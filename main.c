//
// Created by Oleg on 12/5/15.
//

#include <stdio.h>

#include "List/ListTests.h"

#include "Wishart/WishartTest.h"
#include "GeneticAlgorithm/Simulation.h"


int main(int argc, char* argv[]) {
    // runAllListTests();
    // four_points_test();
    // eight_points_test();
    // twelve_points_test();
    // fisher_iris_test();


    RunSimulation(1000,  // iterations_count
                  100,  // individuals_count
                  5,  // chromosome_size
                  0.5,  // mutation_probability
                  1,  // k_neighbour
                  0.01334, // cluster_height
                  DeJongF1Objective  // Objective function
                 );
}