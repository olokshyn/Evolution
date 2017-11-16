//
// Created by Oleg on 7/27/16.
//

#ifndef EVOLUTION_COMMON_H
#define EVOLUTION_COMMON_H

#include "3rdParty/CList/list.h"

#ifndef LIST_DEFINED_DOUBLE
DEFINE_LIST(double)
#define LIST_DEFINED_DOUBLE
#endif

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#define MAX(A, B) ((A) > (B) ? (A) : (B))

#define SIGN(A) (((A) >= 0) - ((A) < 0))

#define DOUBLE_EPS 1e-10

#define DOUBLE_EQ(A, B) (fabs((A) - (B)) < DOUBLE_EPS)
#define DOUBLE_LE(A, B) (DOUBLE_EQ(A, B) || (A) < (B))
#define DOUBLE_LT(A, B) (!DOUBLE_EQ(A, B) && (A) < (B))
#define DOUBLE_GE(A, B) (DOUBLE_EQ(A, B) || (A) > (B))
#define DOUBLE_GT(A, B) (!DOUBLE_EQ(A, B) && (A) > (B))

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
void Normalize(LIST_TYPE(double) numbers);
void Scale(LIST_TYPE(double) numbers, double a, double b);
void ScaleSumToOne(LIST_TYPE(double) numbers);
double EuclidMeasure(double* x, double* y, size_t size);

#endif //EVOLUTION_COMMON_H
