//
// Created by Oleg on 12/4/15.
//

#ifndef WISHART_WISHART_H
#define WISHART_WISHART_H

#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "../List/List.h"

typedef struct vector {
    const double* x;
    double kDistance;
    double density;
    size_t w;  // Cluster number
} Vector;

typedef struct vectors {
    Vector* v;
    size_t count;
    size_t vector_length;
    double** distances;
} Vectors;

typedef struct cluster {
    size_t number;
    short formed;
} Cluster;

size_t* Wishart(const double* const* vectors,
                size_t vectors_count,
                size_t vector_length,
                size_t k,
                double h);

List* WishartWrapped(const double* const* vectors,
                     size_t vectors_count,
                     size_t vector_length,
                     size_t k,
                     double h);

#endif //WISHART_WISHART_H
