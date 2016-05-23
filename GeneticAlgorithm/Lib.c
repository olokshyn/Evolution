#include <stdlib.h>
#include <math.h>

#include "Lib.h"
#include <assert.H>

#define DOUBLE_EPS 1e-7

double getRand(double min, double max) {
    // return gauss_rnd() * (max - min) + min;
    return rand() / (RAND_MAX + 1.0) * (max - min) + min;
}

int doWithProbability(double prob) {
    if (prob < 0 || prob > 1) {
        return 0;
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
    if (!numbers->length) {
        return;
    }
    double sum = 0.0;
    ListIterator it = begin(numbers);
    double min = *((double*)it.current->value);

    for ( ; !isIteratorAtEnd(it); next(&it)) {
        double value = *((double*)it.current->value);
        sum += value;
        if (value < min) {
            min = value;
        }
    }
    sum += fabs(min) * numbers->length;
    if (fabs(sum) < DOUBLE_EPS) {
        return;
    }

    double test = 0.0;
    for (it = begin(numbers); !isIteratorAtEnd(it); next(&it)) {
        *((double*)it.current->value) =
                ( *((double*)it.current->value) + fabs(min) ) / sum;
        test += *((double*)it.current->value);
    }
    assert(1.0 - test < DOUBLE_EPS);
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
