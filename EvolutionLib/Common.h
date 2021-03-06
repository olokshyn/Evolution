//
// Created by Oleg on 7/27/16.
//

#ifndef EVOLUTION_COMMON_H
#define EVOLUTION_COMMON_H

#include <math.h>

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

double getRand(double min, double max);
int doWithProbability(double prob);
int selectRandom(int rangeLow, int rangeHigh);
void Normalize(LIST_TYPE(double) numbers);
void Normalize_array(double* numbers, size_t size);
void Scale(LIST_TYPE(double) numbers, double a, double b);
void Scale_array(double* numbers, size_t size, double a, double b);
void ScaleSumToOne(LIST_TYPE(double) numbers);
double EuclidMeasure(const double* x, const double* y, size_t size);
bool GaussSLE(double** matrix, size_t rows, size_t cols, double* solution);
double** DistanceMatrix(double** vectors, size_t vectors_count, size_t vector_size);
void DestroyDistanceMatrix(double** matrix, size_t matrix_size);
void NormalizeDistanceMatrix(double** matrix, size_t matrix_size);
size_t MinDistance(double** matrix, size_t matrix_size, size_t vector_index);

#endif //EVOLUTION_COMMON_H
