#ifndef MDSC_SYSTEM_H
#define MDSC_SYSTEM_H

#include "mdsc/config.h"
#include "mdsc/vec.h"

typedef struct {
  vec3 r, v, f;
} particle_t;

typedef struct {
  int n;
  double box[3];
  double rc, rc2;
  particle_t* p;
  // neighbor list (cell linked list)
  int* head; // size: ncells
  int* next; // size: n
  int nc[3];
  double cell[3];
} system_t;

void sys_alloc(system_t* s, int n);
void sys_free(system_t* s);
void sys_random_init(system_t* s, int n, double rho);
void sys_zero_forces(system_t* s);
void pbc_wrap(system_t* s);
void min_image(system_t* s, vec3* dr);
double kinetic_energy(system_t* s);

void sys_build_cells(system_t* s);
void build_verlet(system_t* s);
void for_each_pair(system_t* s, void (*pair_fn)(system_t*, int, int, void*), void* userdata);

#endif
