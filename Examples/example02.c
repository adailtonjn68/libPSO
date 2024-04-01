/*
 * File: example02.c
 * Author: Adailton Braga Junior
 * E-mail: adailtonjn68@gmail.com
 * Description: Find the first 3 terms of the Taylor series expansion of sin(x).
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "../pso.h"


double cost(const void *const params)
{

    const double *const temp = (double *)params;
    double error = 0.;
    double sin_estim;
    double a1 = temp[0];
    double a3 = temp[1];
    double a5 = temp[2];


    const int n_points = 1000;
    const double PI = 3.14159265359;
    const double dx = 2. * PI / (double) n_points;
    double x = -PI;
    double mean_abs_error = 0.;
    for (int i = 0; i < n_points; i++) {
        double x1, x3, x5;
        x1 = x;
        x3 = x1 * x * x;
        x5 = x3 * x * x;
        sin_estim = a1 * x1 + a3 * x3 + a5 * x5;
        error = sin(x) - sin_estim;
        mean_abs_error += error * error;
        x += dx;
    }
    return mean_abs_error / (double) n_points;
}


int main(void)
{
    printf("***** PSO test 2 *****\n");
    printf("*** Find Taylor coefficients of sin(x) ***\n");

    srand(time(0));
    int status = 0;

    const size_t N_PARTICLES = 500;
    const size_t N_ITERATIONS = 500;
    const double MAX_ERROR = .0001;
    const double w = .95; 
    const double c1 = .1;
    const double c2 = .1;

    const double limits[][2] = {{0, 2}, {-1, 0}, {0., 1}};
    const size_t n_axis = PSO_GET_NAXIS_FROM_ARRAY(limits);

    pso_t pso = {.n_particles = N_PARTICLES, .n_iterations = N_ITERATIONS,
                 .max_error_allowed = MAX_ERROR, .n_axis = n_axis,
                 .cost_function = cost};

    status = pso_init(&pso, n_axis, limits, (double [3]) {w, c1, c2});
    if (status) {
        fprintf(stderr, "ERROR: Not possible to initialize PSO\n");
        return status;
    }


    pso_run(&pso);

    printf("**********************\nResult\nParticle: {");
    pso_print_best_particle(&pso);
    printf("}\nError: ");
    pso_print_best_value(&pso);
    printf("\n**********************\n");

    pso_free(&pso);

    return 0;
}
