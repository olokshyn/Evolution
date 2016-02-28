#ifndef TESTFUNCTIONSHEADER
#define TESTFUNCTIONSHEADER

#define _USE_MATH_DEFINES
#include <math.h>

#include "Lib.h"


double DeJongF1(double* args, int args_size);  // De Jong`s F1 function, [-5.12, 5.12], max - 0, inversed
double DeJongF2(double* args, int args_size);  // De Jong`s F2 function, [-2.048, 2.048], max - 0, inversed
double DeJongF3(double* args, int args_size);  // De Jong`s F3 function, [-5.12, 5.12], max - 25, min - -30
double DeJongF4(double* args, int args_size);  // De Jong`s F4 function, [-1.28, 1.28], max - 0, inversed
double DeJongF5(double* args, int args_size);  // De Jong`s F5 function, [-65.536, 65.536], max - 500, min - 1, two-vars function

double RastriginFunc(double* args, int args_size);  // Rastrigin`s function, [-5.12, 5.12], max - 0, inversed

double SchwefelFunc(double* args, int args_size);  // Schwefel`s function, [-500, 500], max - 0, inversed, trouble

double GriewangkFunc(double* args, int args_size);  // Griewangk’s function, [-500, 500], max - 0, inversed

double StretchedVSineWaveFunc(double* args, int args_size);  // Stretched V sine wave function, [-10, 10], max - 0, inversed

double AckleyFunc(double* args, int args_size);  // Ackley`s function, [-32.768, 32.768], max - 0, inversed

double EggHolderFunc(double* args, int args_size);  // Egg Holder function, [-512, 512], max - 959.6407, inversed, trouble - solved

double RanaFunc(double* args, int args_size);  // Rana`s function, [-500, 500], max - 0, inversed, trouble

double PathologicalFunc(double* args, int args_size);  // Pathological test function, [-100, 100], max - 0, inversed

#endif