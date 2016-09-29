//
// Created by Oleg on 9/13/16.
//

#ifndef EVOLUTION_CROSSOVERLIB_H
#define EVOLUTION_CROSSOVERLIB_H

#include <stdlib.h>
#include <math.h>

#include "Common.h"
#include "Logging/Logging.h"

typedef double (*norm_func_t)(double q, double x, double y);

typedef double (*scale_func_t)(size_t gen_numb, size_t max_generations_count);

static inline double Tnorm(double q, double x, double y) {
    LOG_ASSERT(q >= 0 && q <= 1);
    LOG_ASSERT(x >= 0 && x <= 1);
    LOG_ASSERT(y >= 0 && y <= 1);
    return (x * y) / MAX(x, MAX(y, q));
}

static inline double Tconorm(double q, double x, double y) {
    LOG_ASSERT(q >= 0 && q <= 1);
    LOG_ASSERT(x >= 0 && x <= 1);
    LOG_ASSERT(y >= 0 && y <= 1);
    return 1 - (1 - x) * (1 - y) / MAX(1 - x, MAX(1 - y, q));
}

static inline double ParAvgSqrt(double q, double x, double y) {
    LOG_ASSERT(x >= 0 && x <= 1);
    LOG_ASSERT(y >= 0 && y <= 1);
    return pow((pow(x, q) + pow(y, q)) / 2, 1 / q);
}

static inline double ParAvgLinear(double q, double x, double y) {
    LOG_ASSERT(q >= 0 && q <= 1);
    LOG_ASSERT(x >= 0 && x <= 1);
    LOG_ASSERT(y >= 0 && y <= 1);
    return q * x + (1 - q) * y;
}

static inline double scale_exploration(size_t gen_numb,
                                       size_t max_generations_count) {
    return 1.0 / gen_numb;
}

static inline double scale_exploitation_plus(size_t gen_numb,
                                             size_t max_generations_count) {
    return 1 + log(max_generations_count / (double)gen_numb);
}

static inline double scale_exploitation_minus(size_t gen_numb,
                                              size_t max_generations_count) {
    return 1 + log((double)gen_numb / max_generations_count);
}

static inline double DynamicBase(size_t gen_numb, size_t max_generations_count,
                                 double c1, double c2, double a, double b,
                                 norm_func_t norm, scale_func_t scale) {
    LOG_ASSERT(gen_numb >= 1 && gen_numb <= max_generations_count);
    LOG_ASSERT(b >= a);
    LOG_ASSERT(c1 >= a && c1 <= b);
    LOG_ASSERT(c2 >= a && c1 <= b);
    double m = b - a;
    return a + m * norm(scale(gen_numb, max_generations_count),
                        (c1 - a) / m, (c2 - a) / m);
}

static inline double dcF(size_t gen_numb, size_t max_generations_count,
                         double c1, double c2, double a, double b) {
    return DynamicBase(gen_numb, max_generations_count,
                       c1, c2, a, b,
                       Tnorm, scale_exploration);
}

static inline double dcS(size_t gen_numb, size_t max_generations_count,
                         double c1, double c2, double a, double b) {
    return DynamicBase(gen_numb, max_generations_count,
                       c1, c2, a, b,
                       Tconorm, scale_exploration);
}

static inline double dcMplus(size_t gen_numb, size_t max_generations_count,
                             double c1, double c2, double a, double b) {
    return DynamicBase(gen_numb, max_generations_count,
                       c1, c2, a, b,
                       ParAvgSqrt, scale_exploitation_plus);
}

static inline double dcMminus(size_t gen_numb, size_t max_generations_count,
                              double c1, double c2, double a, double b) {
    return DynamicBase(gen_numb, max_generations_count,
                       c1, c2, a, b,
                       ParAvgSqrt, scale_exploitation_minus);
}

static inline double dbcM(size_t gen_numb, size_t max_generations_count,
                          double c1, double c2,
                          double fitness1, double fitness2) {
    LOG_ASSERT(gen_numb >= 1 && gen_numb <= max_generations_count);
    LOG_ASSERT(SIGN(fitness1) == SIGN(fitness2));
    double lambda = fitness1 / (fitness1 + fitness2);
    double q = 0.5 + (lambda - 0.5)
                     * (gen_numb - 1) / (max_generations_count - 1);
    return q * c1 + (1 - q) * c2;
}

#endif //EVOLUTION_CROSSOVERLIB_H
