//
// Created by oleg on 10.01.18.
//

#include "math_vector.h"

#include <math.h>
#include <stdlib.h>
#include <assert.h>

double* math_vector_copy(const double* vector, size_t size) {
    double* copy = (double*)malloc(sizeof(double) * size);
    if (!copy) {
        return NULL;
    }
    for (size_t i = 0; i != size; ++i) {
        copy[i] = vector[i];
    }
    return copy;
}

void math_vector_add(const double* vec_i, const double* vec_j, size_t size, double* result) {
    assert(vec_i && vec_j && result);
    for (size_t i = 0; i != size; ++i) {
        result[i] = vec_i[i] + vec_j[i];
    }
}

void math_vector_subtract(const double* vec_i, const double* vec_j, size_t size, double* result) {
    assert(vec_i && vec_j && result);
    for (size_t i = 0; i != size; ++i) {
        result[i] = vec_i[i] - vec_j[i];
    }
}

void math_vector_multiply(const double* vector, double operand, size_t size, double* result) {
    assert(vector && result);
    for (size_t i = 0; i != size; ++i) {
        result[i] = vector[i] * operand;
    }
}

void math_vector_divide(const double* vector, double operand, size_t size, double* result) {
    assert(vector && result);
    for (size_t i = 0; i != size; ++i) {
        result[i] = vector[i] / operand;
    }
}

double math_vector_norm(const double* vector, size_t size) {
    assert(vector);
    double norm = 0.0;
    for (size_t i = 0; i != size; ++i) {
        norm += vector[i] * vector[i];
    }
    return sqrt(norm);
}

double math_vector_distance(const double* vec_i, const double* vec_j, size_t size) {
    assert(vec_i && vec_j);
    double norm = 0.0;
    for (size_t i = 0; i != size; ++i) {
        norm += (vec_i[i] - vec_j[i]) * (vec_i[i] - vec_j[i]);
    }
    return sqrt(norm);
}

void math_vector_normalize(double* vector, size_t size) {
    assert(vector);
    double norm = math_vector_norm(vector, size);
    if (norm == 0.0) {
        return;
    }
    math_vector_divide(vector, norm, size, vector);
}

double math_vector_dot_product(const double* vec_i, const double* vec_j, size_t size) {
    assert(vec_i && vec_j);
    double product = 0.0;
    for (size_t i = 0; i != size; ++i) {
        product += vec_i[i] * vec_j[i];
    }
    return product;
}
