#ifndef MDSC_INTEGRATOR_H
#define MDSC_INTEGRATOR_H

#include "mdsc/system.h"
#include "mdsc/force.h"

typedef struct {
  double dt;
} vv_t;

void vv_step(system_t* s, vv_t* vv, force_accum_t* acc, void (*force_fn)(system_t*, force_accum_t*));

#endif
