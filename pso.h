#ifndef PSO_H_
#define PSO_H_


#include <stdlib.h>


#define PSO_GET_NAXIS_FROM_ARRAY(array) (sizeof(array) / sizeof(array[0]))


typedef struct {
    double *position;
    double *velocity;
    double *best_position;
    double value;
    double best_value;
} particle_t;


typedef struct {
    size_t n_particles;
    size_t n_iterations;
    size_t n_axis;
    double max_error_allowed;
    struct {
        double w, c1, c2;
    };
    particle_t *particles;
    double *limits;
    double *global_best_position;
    double global_best_value;
    double (*cost_function)(const void *const);
    bool valid;     // Indicates whether struct was initialized correctly
} pso_t;


[[nodiscard]]
int pso_init(pso_t *const pso, const size_t n_axis,
             const double limits[static n_axis][2],
             const double params[static 3]);
void pso_free(pso_t *const pso);
[[nodiscard]]
int pso_init_particles(pso_t *const pso,
                       const double limits[static pso->n_axis][2]);
int pso_print_particles(const pso_t *const pso);
int pso_find_best_global_particle(pso_t *const pso);
int pso_run(pso_t *const pso);
int pso_update_particles(pso_t *const pso);
int pso_print_best_particle(const pso_t *const pso);
int pso_print_best_value(const pso_t *const pso);

#endif // !PSO_H_
