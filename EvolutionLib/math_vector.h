//
// Created by oleg on 10.01.18.
//

#ifndef EVOLUTION_MATH_VECTOR_H
#define EVOLUTION_MATH_VECTOR_H

#include <stddef.h>

double* math_vector_copy(const double* vector, size_t size);

void math_vector_add(const double* vec_i, const double* vec_j, size_t size, double* result);
void math_vector_subtract(const double* vec_i, const double* vec_j, size_t size, double* result);
void math_vector_multiply(const double* vector, double operand, size_t size, double* result);
void math_vector_divide(const double* vector, double operand, size_t size, double* result);

double math_vector_norm(const double* vector, size_t size);
double math_vector_distance(const double* vec_i, const double* vec_j, size_t size);

void math_vector_normalize(double* vector, size_t size);

double math_vector_dot_product(const double* vec_i, const double* vec_j, size_t size);

#endif //EVOLUTION_MATH_VECTOR_H
