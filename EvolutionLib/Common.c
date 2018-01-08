//
// Created by Oleg on 7/27/16.
//

#include "Common.h"

#include <math.h>

#include "Logging/Logging.h"

double getRand(double min, double max) {
    // return gauss_rnd() * (max - min) + min;
    // LinearRankingSelection relies on [min, max) interval
    double r = rand() / (RAND_MAX + 1.0) * (max - min) + min;
    LOG_RELEASE_ASSERT(r >= min && r <= max);
    return r;
}

int doWithProbability(double prob) {
    if (prob <= 0.0) {
        return 0;
    }
    if (prob >= 1.0) {
        return 1;
    }
    if (getRand(0, 1) <= prob) {
        return 1;
    }
    return 0;
}

int selectRandom(int rangeLow, int rangeHigh) {
    return (int)round(getRand(0, 1) * (rangeHigh - rangeLow)) + rangeLow;
}

void Normalize(LIST_TYPE(double) numbers) {
    // TODO: maybe something different should be done here?
    Scale(numbers, 0.01, 0.99);
}

void Scale(LIST_TYPE(double) numbers, double a, double b) {
    LOG_FUNC_START;
    LOG_RELEASE_ASSERT(numbers);

    if (list_len(numbers) == 0) {
        Log(WARNING, "%s: cannot scale empty list", __func__);
        goto error;
    }
    if (list_len(numbers) == 1) {
        Log(WARNING, "%s: scaling list of one element", __func__);
        list_first(numbers) = b;
        goto exit;
    }

    LIST_ITER_TYPE(double) iter = list_begin(double, numbers);
    double min = list_first(numbers);
    double max = list_first(numbers);
    for (; list_iter_valid(iter); list_next(double, iter)) {
        if (list_iter_value(iter) < min) {
            min = list_iter_value(iter);
        }
        if (list_iter_value(iter) > max) {
            max = list_iter_value(iter);
        }
    }

    if (!DOUBLE_EQ(min, max)) {
        double factor = (b - a) / (max - min);
        Log(DEBUG, "%s: min value: %0.3f, max value: %0.3f", __func__, min, max);

        list_for_each(double, numbers, var) {
            list_var_value(var) = (list_var_value(var) - min) * factor + a;

            LOG_RELEASE_ASSERT(DOUBLE_GE(list_var_value(var), a)
                               && DOUBLE_LE(list_var_value(var), b));
        }
    }
    else {
        Log(WARNING, "%s: scaling list of equal elements", __func__);
        list_for_each(double, numbers, var) {
            list_var_value(var) = b;
        }
    }

exit:
    LOG_FUNC_SUCCESS;
    return;

error:
    LOG_FUNC_ERROR;
}

void ScaleSumToOne(LIST_TYPE(double) numbers) {
    double sum = 0.0;
    list_for_each(double, numbers, var) {
        sum += list_var_value(var);
    }
    if (DOUBLE_EQ(sum, 1.0)) {
        return;
    }
    LOG_RELEASE_ASSERT(!DOUBLE_EQ(sum, 0.0));
    list_for_each(double, numbers, var) {
        list_var_value(var) /= sum;
    }
#ifndef NDEBUG
    sum = 0.0;
    list_for_each(double, numbers, var) {
        sum += list_var_value(var);
    }
    LOG_ASSERT(DOUBLE_EQ(sum, 1.0));
#endif
}

double EuclidMeasure(const double* x, const double* y, size_t size) {
    double sum = 0.0;
    for (size_t i = 0; i < size; ++i) {
        sum += pow(x[i] - y[i], 2);
    }
    return sqrt(sum);
}

bool GaussSLE(double** matrix, size_t rows, size_t cols, double* solution) {
    if (rows + 1 != cols) {
        return false;
    }

    // forward
    for (size_t i = 0; i != rows; ++i) {
        // find the row with the biggest key
        size_t max_key_row = i;
        for (size_t j = i + 1; j != rows; ++j) {
            if (fabs(matrix[j][i]) > fabs(matrix[max_key_row][i])) {
                max_key_row = j;
            }
        }
        if (max_key_row != i) {
            double* temp = matrix[i];
            matrix[i] = matrix[max_key_row];
            matrix[max_key_row] = temp;
        }

        // Check for the incompatible matrix
        if (DOUBLE_EQ(matrix[i][i], 0.0)) {
            return false;
        }

        for (size_t j = i + 1; j != rows; ++j) {
            double delimiter = - matrix[j][i] / matrix[i][i];
            for (size_t c = i; c != cols; ++c) {
                matrix[j][c] += matrix[i][c] * delimiter;
            }
        }
    }

    // backward
    for (size_t i = 0; i != rows; ++i) {
        size_t index = rows - i - 1;
        solution[index] = matrix[index][rows];
        for (size_t j = index + 1; j != rows; ++j) {
            solution[index] -= matrix[index][j] * solution[j];
        }
        solution[index] /= matrix[index][index];
    }

    return true;
}
