#include "pso.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>


/**
 * @brief Test whether valid arguments were passed to pso_init()
 * @params pso Pointer to which the pso will be set up.
 * @param n_axis Number of axis (coordinates, dimensions)
 * @param limits Array [n_axis][2] with maximum and minimum of each axis
 * @param params Parameters [3] for correction of PSO
 * return Succes:0 ; Failure: negative number
 */
static int __pso_init_check_args(const pso_t *const pso,
                                 const size_t n_axis,
                                 const double limits[static n_axis][2],
                                 const double params[static 3])
{
    if (pso == nullptr) {
        fprintf(stderr, "ERROR: Invalid argumend pso\n");
        return -EINVAL;
    }

    if (!pso->n_particles || !pso->n_iterations) {
        fprintf(stderr, "ERROR: n_particles or n_iterations invalid\n");
        return -EINVAL;
    }

    if (!n_axis || limits == nullptr) {
        fprintf(stderr, "ERROR: n_axis or limis[n_axis][2] invalid\n");
        return -EINVAL;
    }

    if (params == nullptr) {
        fprintf(stderr, "ERROR: Invalid argumend params\n");
        return -EINVAL;
    }
    return 0;
}

 
/**
 * @brief Pass content of limits (axis maximum e minimum values) to pso
 * @param pso Pointer to pso
 * @param limits Array [n_axis][2] with maximum and minimum of each axis
 * @return 0 if succeded; negative number otherwise
 */
static int __pso_init_limits_ptr(pso_t *const pso,
                                 const double limits[static pso->n_axis][2])
{
    double (*_temp)[pso->n_axis][2] = malloc(sizeof(*_temp));
    if (_temp == nullptr) {
        fprintf(stderr, "ERROR: Not possible to allocate memory for"
                        "pso->limits\n" );
        return -ENOMEM;
    }
    pso->limits = (double *)_temp;
    memcpy(pso->limits, limits, 2 * pso->n_axis * sizeof(double));

    return 0;
}


/**
 * @brief Initialize pso.
 * @params pso Pointer to which the pso will be set up.
 * @param n_axis Number of axis (coordinates, dimensions)
 * @param limits Array [n_axis][2] with maximum and minimum of each axis
 * @param params Parameters [3] for correction of PSO
 * @return status Integer indicating status (0: okay, else: failed)
 */
int pso_init(pso_t *const pso,
             const size_t n_axis, const double limits[static n_axis][2],
             const double params[static 3])
{
    int status = 0;

    /* Check for invalid arguments */
    status = __pso_init_check_args(pso, n_axis, limits, params);
    if (status) {
        return status;
    }

    /* Initialize pso as invalid until it is all set up */
    pso->valid = false;

    /* Pass input arguments to pso struct */
    pso->n_axis = n_axis;

    /* Allocate memory for particles structs */
    pso->particles = malloc(sizeof(particle_t [pso->n_particles]));
    if (pso->particles == nullptr) {
        fprintf(stderr, "ERROR: It was not possible to allocate "
                        "memory for particles\n");
        status = -ENOMEM;
        goto _error_particles;
    }

    /*
     * Initialize each particle by allocating memory to it, setting its position
     * randomly inside the limits and setting velocity to zero
     */
    if (pso_init_particles(pso, limits)) {
        status = -ENOMEM;
        goto _error_particles_contents;
    }

    /* Pass input correction parameters to pso */
    pso->w = params[0];
    pso->c1 = params[1];
    pso->c2 = params[2];

    /* Allocate memory for best particle */
    pso->global_best_position = malloc(sizeof(double[pso->n_axis]));
    if (pso->global_best_position == nullptr) {
        fprintf(stderr, "ERROR: It was not possible to allocate memory for global_best_position\n");
        status = -ENOMEM;
        goto _error_global_best;
    }
    pso->global_best_value = 1e10;
    status = __pso_init_limits_ptr(pso, limits);
    if (status) {
        goto _error_limits;
    }
    /* Set pso validity flag to true */
    pso->valid = true;

    return 0;

_error_limits:
    free(pso->global_best_position);
    pso->global_best_position = nullptr;
_error_global_best:
    free(pso->particles[0].position);
_error_particles_contents:
    free(pso->particles);
    pso->particles = nullptr;
_error_particles:
    free(pso);
    return status;
}


/**
 * @brief Free memory allocated for pso
 * @param pso Pointer to pso to be deallocated
 * @return void
 */
void pso_free(pso_t *const pso)
{
    free(pso->limits);
    pso->limits = nullptr;
    free(pso->global_best_position);
    pso->global_best_position = nullptr;
    free(pso->particles[0].position);
    free(pso->particles);
    pso->particles = nullptr;
}


/**
 * @brief Set velocity of all particles to 0
 * @param pso Pointer to pso
 * @return 0 if succeded; negative number otherwise
 */
static int pso_set_zero_velocity(pso_t *const pso)
{
    if (pso == nullptr) {
        return -EINVAL;
    }

    for (size_t i = 0; i < pso->n_particles; i++) {
        for (size_t j = 0; j < pso->n_axis; j++) {
            pso->particles[i].velocity[j] = 0.;
        }
    }
    return 0;
}



/**
 * @brief Initialize particles' position randomly inside pso domain
 * @param pso Pointer to pso
 * @param limits Array [n_axis][2] to pso coordinate domain
 * @return 0 if succeded; negative number otherwise
 */
static int pso_set_rand_position(pso_t *const pso,
                                 const double limits[static pso->n_axis][2])
{
    if (pso == nullptr || limits == nullptr) {
        return -EINVAL;
    }

    for (size_t i = 0; i < pso->n_particles; i++) {
        for (size_t j = 0; j < pso->n_axis; j++) {
            const double limit1 = limits[j][0];
            const double limit2 = limits[j][1];
            const double temp = (limit2 - limit1) / (double) RAND_MAX;
            const double temp_pos = ((double)rand()) * temp + limit1;
            pso->particles[i].position[j] = temp_pos;
            pso->particles[i].best_position[j] = temp_pos;
        }
    }
    return 0;
}


/**
 * @brief Initialize all particles' best position with its actual position
 * @param pso Pointer to pso
 * @return 0 if succeded; negative number otherwise
 */
static int pso_init_particle_best_position(pso_t *const pso)
{
    if (pso == nullptr) {
        return -EINVAL;
    }

    for (size_t i = 0; i < pso->n_particles; i++) {
        memcpy(pso->particles[i].best_position, pso->particles[i].position,
               pso->n_axis * sizeof(double));
    }
    return 0;
}


/**
 * @brief Initialize all particles' best value to a huge number
 * @param pso Pointer to pso
 * @return 0 if succeded; negative number otherwise
 */
static int pso_init_best_value(pso_t *const pso)
{
    if (pso == nullptr) {
        return -EINVAL;
    }
    for (size_t i = 0; i < pso->n_particles; i++) {
        pso->particles[i].best_value = 1e10;
    }

    return 0;
}


/**
 * @brief Initialize particles for PSO
 *
 * Memory is allocated for all particles position, velocity and best position.
 *
 * @param pso Pointer to pso to be initialized.
 * @param limits Array with limits to each axis.
 * @return error Code with error.
 */
int pso_init_particles(pso_t *const pso,
                       const double limits[static pso->n_axis][2])
{
    int status = 0;
    /* Check validity of inputs */
    if (pso == nullptr || limits == nullptr) {
        fprintf(stderr, "ERROR: passed invalid arguments to %s\n", __func__);
        return -EINVAL;
    }

    /* Allocate memory for particles */
    double (*_ptr)[pso->n_particles][3][pso->n_axis] = malloc(sizeof(*_ptr));
    if (_ptr == nullptr) {
        fprintf(stderr, "ERROR: It wasn't possible to allocate memory "
                        "for particle contents\n");
        return -ENOMEM;
    }

    /* Pass address of each particle's position, velocity and best position */
    for (size_t i = 0; i < pso->n_particles ; i++) {
        pso->particles[i].position = &(*_ptr)[i][0][0];
        pso->particles[i].velocity = &(*_ptr)[i][1][0];
        pso->particles[i].best_position = &(*_ptr)[i][2][0];
    }

    status |= pso_set_zero_velocity(pso);
    status |= pso_set_rand_position(pso, limits);
    status |= pso_init_particle_best_position(pso);
    status |= pso_init_best_value(pso);

    return status;
}


/**
 * @brief Print all positions.
 * @param pso Pointer to pso
 * @return 0 if succeded; negative number otherwise
 */
int pso_print_particles(const pso_t *const pso)
{
    if (pso == nullptr) {
        fprintf(stderr, "ERROR: Invalid argument passed to %s\n", __func__);
        return -EINVAL;
    }

    for (size_t i = 0; i < pso->n_particles; i++) {
        printf("particle[%ld]  ", i);
        for (size_t j = 0; j < pso->n_axis; j++) {
            printf("[%ld] %lf    ", j, pso->particles[i].position[j]);
        }
        printf("\n");
    }
    return 0;
}


/**
 * @brief Update particles bases on its best position the swarm's best one.
 * @param pso_t
 * @return 0 if succeded; negative number otherwise
 */
int pso_update_particles(pso_t *const pso)
{
    if (pso == nullptr) {
        fprintf(stderr, "ERROR: Invalid argument passed to %s\n", __func__);
        return -EINVAL;
    }
    double inertial, self_confidence, swarm_confidence;
    double _rand1, _rand2;
    double velocity;
    double _max, _min;
    double (*limits)[pso->n_axis][2] = (void *) pso->limits;

    /* Loop through particles */
    for (size_t i = 0; i < pso->n_particles; i++) {
        /* Two random numbers between 0 and 1 */
        _rand1 = ((double) rand()) / (double) RAND_MAX;
        _rand2 = ((double) rand()) / (double) RAND_MAX;

        /* Loop through axis */
        for (size_t j = 0; j < pso->n_axis; j++) {
            /* Displacement related to its best position */
            double _temp1 = pso->particles[i].best_position[j] -
                            pso->particles[i].position[j];
            /* Displacement related to swarm best postion */
            double _temp2 = pso->global_best_position[j] -
                            pso->particles[i].position[j];

            inertial = pso->w * pso->particles[i].velocity[j];
            self_confidence = pso->c1 * _rand1 * _temp1;
            swarm_confidence = pso->c2 * _rand2 * _temp2;

            velocity = inertial + self_confidence + swarm_confidence;

            pso->particles[i].velocity[j] = velocity;

            pso->particles[i].position[j] += velocity;

            /* TODO: define max e min in pso_init() */
            if ((*limits)[j][0] > (*limits)[j][1]) {
                _max = (*limits)[j][0];
                _min = (*limits)[j][1];
            }
            else {
                _max = (*limits)[j][1];
                _min = (*limits)[j][0];
            }

            /* Ensure paritcle's new position is inside the domain */
            if (pso->particles[i].position[j] > _max) {
                pso->particles[i].position[j] = _max;
            }
            else if (pso->particles[i].position[j] < _min) {
                pso->particles[i].position[j] = _min;
            }
        }
    }
    return 0;
}


/**
 * @brief Fined the best particle in the swarm
 * @parm pso Pointer of pso
 * @return 0 if succeded; negative number otherwise
 */
int pso_find_best_global_particle(pso_t *const pso)
{
    double cost;

    if (pso == nullptr) {
        fprintf(stderr, "ERROR: Invalid argument to %s\n", __func__);
        return -EINVAL;
    }

    /* Loop through particles */
    for (size_t i = 0; i < pso->n_particles; i++) {

        /* Compute cost function with position of particle */
        cost = pso->cost_function(pso->particles[i].position);

        /* Update particle's best position and value */
        if (cost < pso->particles[i].best_value) {
            pso->particles[i].best_value = cost;
            memcpy(pso->particles[i].best_position, pso->particles[i].position,
                   sizeof(double [pso->n_axis]));
        }

        /* Update global best value and postion */
        if (cost < pso->global_best_value) {
            pso->global_best_value = cost;
            memcpy(pso->global_best_position, pso->particles[i].position,
                   sizeof(double [pso->n_axis]));
        }
    }
    return 0;
}


/**
 * @brief Run PSO optimization
 * @params pso Pointer to pso
 * @return 0 if succeded; negative number otherwise
 */
int pso_run(pso_t *const pso)
{
    int status = 0;
    if (pso == nullptr) {
        fprintf(stderr, "ERROR: Invalid argument to %s\n", __func__);
        return -EINVAL;
    }

    /* Start PSO optimization */
    size_t i = 0;
    do {
        printf("Iteration %ld - \t", i);

        status |= pso_find_best_global_particle(pso);
        status |= pso_update_particles(pso);
        if (status) {
            break;
        }

        printf("cost: %.10lf\n", pso->global_best_value);

        /* Stop if error is acceptable */
        if (pso->global_best_value < pso->max_error_allowed) {
            break;
        }

        i++;
    } while (i <= pso->n_iterations);
    return status;
}


/**
 * @brief Print coordinate of best particle
 *        It is worth observing that it does not find the best particle.
 * @param pso_t 
 * @return void
 */
int pso_print_best_particle(const pso_t *const pso)
{
    if (pso == nullptr) {
        fprintf(stderr, "ERROR: Invalid argument to  %s\n", __func__);
        return -EINVAL;
    }

    for (size_t i = 0; i < pso->n_axis - 1; i++) {
        printf("%lf, ", pso->global_best_position[i]);
    }
    printf("%lf", pso->global_best_position[pso->n_axis - 1]);
    return 0;
}


/**
 * @brief Print value (cost) of best particle
 *        It is worth observing that it does not find the best particle.
 * @param pso_t
 * @return void
 */
int pso_print_best_value(const pso_t *const pso)
{
    if (pso == nullptr) {
        fprintf(stderr, "ERROR: Invalid argument to  %s\n", __func__);
        return -EINVAL;
    }
    printf("%lf", pso->global_best_value);
    return 0;
}
