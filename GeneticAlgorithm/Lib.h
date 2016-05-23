#ifndef LIBHEADER
#define LIBHEADER

#include "../List/List.h"

#define MIN(A, B) (A < B ? A : B)
#define MAX(A, B) (A > B ? A : B)

double getRand(double min, double max);
int doWithProbability(double prob);
int selectRandom(int rangeLow, int rangeHigh);
void Normalize(List* numbers);

#endif