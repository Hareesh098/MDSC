#ifndef MDSC_FORCE_H
#define MDSC_FORCE_H

#include "mdsc/system.h"

typedef struct {
  double pe;
} force_accum_t;

void lj_forces(system_t* s, force_accum_t* acc);

#ifdef __cplusplus
extern "C" {
#endif
void lj_forces_cuda(system_t* s, force_accum_t* acc);
#ifdef __cplusplus
}
#endif

#endif
