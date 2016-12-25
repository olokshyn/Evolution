//
// Created by Oleg on 7/27/16.
//

#ifndef EVOLUTION_COMMON_H
#define EVOLUTION_COMMON_H

#include "List/List.h"

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

#define SIGN(A) (((A) >= 0) - ((A) < 0))

#define DOUBLE_EPS 1e-7

typedef double(*ObjectiveFunc)(double*, int);

typedef struct objective {
    ObjectiveFunc func;
    double min;
    double max;
    size_t max_args_count;  // 0 if no limit on args count
    double optimum;  // not used
    size_t best_k;  // not used
    double best_h;  // not used
} Objective;

double getRand(double min, double max);
int doWithProbability(double prob);
int selectRandom(int rangeLow, int rangeHigh);
void Normalize(List* numbers);
void Scale(List* numbers, double a, double b);
double EuclidMeasure(double* x, double* y, size_t size);

#endif //EVOLUTION_COMMON_H
