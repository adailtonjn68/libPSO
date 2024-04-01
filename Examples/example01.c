/*
 * File: example01.c
 * Author: Adailton Braga Junior
 * E-mail: adailtonjn68@gmail.com
 * Description: Minimize a two dimensional second order equation
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../pso.h"


double cost(const void *const params)
{

    const double *const temp = (double *)params;
    double x = temp[0];
    double y = temp[1];

    return 5. * x * x  - 52. * x + 200. + 5. * y * y - 30 * y + 100;
}


int main(void)
{
    printf("***** PSO test 1 *****\n");

    srand(time(0));
    int status = 0;

    const size_t N_PARTICLES = 50;
    const size_t N_ITERATIONS = 20;
    const double MAX_ERROR = .3;
    const double w = .1; 
    const double c1 = .5;
    const double c2 = .8;

    const double limits[][2] = {{-10, 10}, {-20, 20}};
    const size_t n_axis = PSO_GET_NAXIS_FROM_ARRAY(limits);

    pso_t pso = {.n_particles = N_PARTICLES, .n_iterations = N_ITERATIONS,
                 .max_error_allowed = MAX_ERROR, .n_axis = n_axis,
                 .cost_function = cost};

    status = pso_init(&pso, n_axis, limits, (double [3]) {w, c1, c2});
    if (status) {
        fprintf(stderr, "ERROR: Not possible to initialize PSO\n");
        return status;
    }

    pso_print_particles(&pso);

    pso_run(&pso);

    pso_print_particles(&pso);

    printf("**********************\nResult\nParticle: {");
    pso_print_best_particle(&pso);
    printf("}\nError: ");
    pso_print_best_value(&pso);
    printf("\n**********************\n");

    pso_free(&pso);

    return 0;
}
