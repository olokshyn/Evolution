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

void Normalize(List* numbers) {
    if (numbers->length == 0) {
        Log(WARNING, "Normalize: cannot normalize empty list");
        return;
    }
    if (numbers->length == 1) {
        *(double*)numbers->head->value = 1.0;
        Log(WARNING, "Normalize: normalizing list of one element");
        return;
    }

    Scale(numbers, 1.0, 2.0);
    Normalize2(numbers);
    return;

    double sum = 0.0;
    ListIterator it = begin(numbers);
    double min = *((double*)it.current->value);

    for ( ; !isIteratorExhausted(it); next(&it)) {
        double value = *((double*)it.current->value);
        sum += value;
        if (value < min) {
            min = value;
        }
    }
    sum += fabs(min) * numbers->length;
    if (fabs(sum) < DOUBLE_EPS) {
        Log(WARNING, "Normalize: elements sum is close to 0");
        return;
    }

    double test = 0.0;
    for (it = begin(numbers); !isIteratorExhausted(it); next(&it)) {
        *(double*)it.current->value =
                ( *(double*)it.current->value + fabs(min) ) / sum;
        test += *(double*)it.current->value;
    }
    LOG_RELEASE_ASSERT(1.0 - test < DOUBLE_EPS);
}

// TODO: Remove this
void Normalize2(List* numbers) {
    if (numbers->length == 0) {
        Log(WARNING, "Normalize2: cannot normalize empty list");
        return;
    }
    if (numbers->length == 1) {
        *(double*)numbers->head->value = 1.0;
        Log(WARNING, "Normalize2: normalizing list of one element");
        return;
    }

    double sum = 0.0;
    ListIterator it = begin(numbers);
    for ( ; !isIteratorExhausted(it); next(&it)) {
        double value = *((double*)it.current->value);
        sum += value;
    }
    if (fabs(sum) < DOUBLE_EPS) {
        Log(WARNING, "Normalize2: elements sum is close to 0");
        return;
    }

    double test = 0.0;
    for (it = begin(numbers); !isIteratorExhausted(it); next(&it)) {
        *(double*)it.current->value /= sum;
        test += *(double*)it.current->value;
    }
    LOG_RELEASE_ASSERT(fabs(1.0 - test) < DOUBLE_EPS);
}

void Scale(List* numbers, double a, double b) {
    if (numbers->length == 0) {
        Log(WARNING, "Scale: cannot scale empty list");
        return;
    }
    if (numbers->length == 1) {
        Log(WARNING, "Scale: scaling list of one element");
        *(double*)numbers->head->value = b;
        return;
    }
    ListIterator it = begin(numbers);
    double min = *((double*)it.current->value);
    double max = *((double*)it.current->value);

    for ( ; !isIteratorExhausted(it); next(&it)) {
        double current = *(double*)it.current->value;
        if (current < min) {
            min = current;
        }
        if (current > max) {
            max = current;
        }
    }

    if (max - min < DOUBLE_EPS) {
        Log(WARNING, "Scale: max - min is close to 0");
        return;
    }

    for (it = begin(numbers); !isIteratorExhausted(it); next(&it)) {
        *(double*)it.current->value =
                a + (b - a) * (*(double*)it.current->value - min) / (max - min);
        LOG_RELEASE_ASSERT((*(double*)it.current->value >= a
                            || fabs(*(double*)it.current->value - a) < DOUBLE_EPS)
                           && (*(double*)it.current->value <= b ||
                               fabs(*(double*)it.current->value - b) < DOUBLE_EPS));
    }
}

double EuclidMeasure(double* x, double* y, size_t size) {
    double sum = 0.0;
    for (size_t i = 0; i < size; ++i) {
        sum += pow(x[i] - y[i], 2);
    }
    return sqrt(sum);
}

//double rnd() {
//    return rand() / (double)RAND_MAX;
//}

//double gauss_rnd()
//{
//    static double r;
//    static char flag = 0;
//    double v1, v2, s;
//    if (flag) {
//        flag = 0;
//        return r;
//    }
//    while (1) {
//        v1 = 2.0 * rnd() - 1.0;
//        v2 = 2.0 * rnd() - 1.0;
//        s = v1 * v1 + v2 * v2;
//        if ((s <= 1.0) && (s > 0.0)) {
//            r = v1 * sqrt((-2.0) * log(s) / s);
//            flag = 1;
//            return v2 * sqrt((-2.0) * log(s) / s);
//        }
//    }
//}

//double fp_uniform(double a, double b)
//{
//    double t;
//    unsigned char *p = (unsigned char *)&t;
//    size_t i, n = 0;
//    do
//    {
//        for (i = 0; i < sizeof (double); ++i)
//        {
//            p[i] = rand() / (RAND_MAX + 1.0) * (UCHAR_MAX + 1.0);
//        }
//        n++;
//    } while (t == 0 || isnan(t) || t < a || t > b);
//    return t;
//}
