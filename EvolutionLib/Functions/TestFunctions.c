#include "TestFunctions.h"

Objective DeJongF1Objective = {
    .func = DeJongF1,
    .min = -5.12,
    .max = 5.12,
    .optimum = 0,
};
Objective DeJongF2Objective = {
    .func = DeJongF2,
    .min = -2.048,
    .max = 2.048,
    .optimum = 0,
};
Objective DeJongF3Objective = {
    .func = DeJongF3,
    .min = -5.12,
    .max = 5.12,
    .optimum = 25,
    .max_args_count = 5
};
Objective DeJongF4Objective = {
    .func = DeJongF4,
    .min = -1.28,
    .max = 1.28,
    .optimum = 0,
};
Objective DeJongF5Objective = {
    .func = DeJongF5,
    .min = -65.536,
    .max = 65.536,
    .optimum = 500,
    .max_args_count = 2,
};

Objective RastriginFuncObjective = {
    .func = RastriginFunc,
    .min = -5.12,
    .max = 5.12,
    .optimum = 0,
};

Objective SchwefelFuncObjective = {
    .func = SchwefelFunc,
    .min = -500,
    .max = 500,
    .optimum = 0,
};

Objective GriewangkFuncObjective = {
    .func = GriewangkFunc,
    .min = -600,
    .max = 600,
    .optimum = 0,
};

Objective StretchedVSineWaveFuncObjective = {
    .func = StretchedVSineWaveFunc,
    .min = -100,
    .max = 100,
    .optimum = 0,
};

Objective AckleyFuncObjective = {
    .func = AckleyFunc,
    .min = -32.768,
    .max = 32.768,
    .optimum = 0,
};

Objective EggHolderFuncObjective = {
    .func = EggHolderFunc,
    .min = -512,
    .max = 512,
    .optimum = 959.6407,
    .max_args_count = 2
};

Objective RanaFuncObjective = {
    .func = RanaFunc,
    .min = -500,
    .max = 500,
    .optimum = 0,
};

Objective PathologicalFuncObjective = {
    .func = PathologicalFunc,
    .min = -100,
    .max = 100,
    .optimum = 0,
};


double DeJongF1(double* args, int args_size) {
    double res = 0;
    for (int i = 0; i < args_size; ++i) {
        res += args[i] * args[i];
    }
    return -res;
}

double DeJongF2(double* args, int args_size) {
    double res = 0;
    for (int i = 0; i < args_size - 1; ++i) {
        res += 100 * pow(args[i + 1] - args[i] * args[i], 2.0) 
               + pow(1 - args[i], 2.0);
    }
    return -res;
}

double DeJongF3(double* args, int args_size) {
    double res = 0;
    for (int i = 0; i < args_size; ++i) {
        res += (int)args[i];  // get the integer part of Number
    }
    return res;
}

double DeJongF4(double* args, int args_size) {
    double res = getRand(0, 1);  // TODO: Gauss distribution must be used
    for (int i = 0; i < args_size; ++i) {
        res += (i + 1) * pow(args[i], 4.0);
    }
    return -res;
}

double DeJongF5(double* args, int args_size) {
    double res = 0;
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; j++) {
            res += 1 / (5*(i + 2) + j + 3 + 
                pow(args[0] - 16*j, 6.0) + pow(args[1] - 16*i, 6.0));
        }
    }
    res += 0.002;
    return 1 / res;
}

double RastriginFunc(double* args, int args_size) {
    double res = 10 * args_size;
    for (int i = 0; i < args_size; ++i) {
        res += args[i] * args[i] - 10 * cos(2 * M_PI * args[i]);
    }
    return -res;
}

double SchwefelFunc(double* args, int args_size) {
    double res = 418.9829 * args_size;
    for (int i = 0; i < args_size; ++i) {
        res += -args[i] * sin(sqrt(fabs(args[i])));
    }
    return -res;
}

double GriewangkFunc(double* args, int args_size) {
    double res = 1;
    for (int i = 0; i < args_size; ++i) {
        res += pow(args[i], 2) / 4000;
    }
    double temp = 1;
    for (int i = 0; i < args_size; ++i) {
        temp *= cos(args[i] / sqrt((double)(i + 1)));
    }
    res -= temp;
    return -res;
}

double StretchedVSineWaveFunc(double* args, int args_size) {
    double res = 0, t = 0;
    for (int i = 0; i < args_size - 1; ++i) {
        t = pow(args[i], 2) + pow(args[i + 1], 2);
        res += pow(t, 0.25)
            * pow(sin(50 * pow(t, 0.1)), 2) + 1;
    }
    return -res;
}

double AckleyFunc(double* args, int args_size) {
    double res = 0, t1 = 0, t2 = 0;
    for (int i = 0; i < args_size; ++i) {
        t1 += args[i] * args[i];
        t2 += cos(2 * M_PI * args[i]);
    }
    res = -20 * exp(-0.2 * sqrt(1.0 / args_size * t1))
          - exp(1.0 / args_size * t2) + 20 + M_E;
//    for (int i = 0; i < args_size - 1; ++i) {
//        res += 20 + M_E
//            - 20 * pow(M_E, -0.2 * sqrt((pow(args[i], 2) + pow(args[i + 1], 2)) / 2))
//            - pow(M_E, 0.5 * (cos(2 * M_PI * args[i]) + cos(2 * M_PI + args[i + 1])));
//    }
    return -res;
}

double EggHolderFunc(double* args, int args_size) {
    double res = 0;
    for (int i = 0; i < args_size - 1; ++i) {
        res -= args[i] * sin(sqrt(fabs(args[i] - args[i + 1] - 47)))
            + (args[i + 1] + 47) 
            * sin(sqrt(fabs(args[i + 1] + 47 + args[i] / 2.0)));
    }
    return -res;
}

double RanaFunc(double* args, int args_size) {
    double res = 0, t1 = 0, t2 = 0;
    for (int i = 0; i < args_size - 1; ++i) {
        t1 = sqrt(fabs(args[i + 1] + 1 - args[i]));
        t2 = sqrt(fabs(args[i + 1] + 1 + args[i]));
        res += args[i] 
            * sin(t1)
            * cos(t2)
            + (args[i + 1] + 1)
            * cos(t1)
            * sin(t2);
    }
    return res;
}

double PathologicalFunc(double* args, int args_size) {
    double res = 0;
    for (int i = 0; i < args_size - 1; ++i) {
        // TODO: check this function
        // double temp = 1 + 0.001 * pow(args[i] - args[i + 1], 4);
        res += 0.5 
            + (pow(sin(sqrt(100 * pow(args[i], 2) + pow(args[i + 1], 2))), 2) - 0.5)
            / (1 + 0.001 * pow(args[i] - args[i + 1], 4));
    }
    return -res;
}