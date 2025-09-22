#ifndef MDSC_THERMOSTAT_H
#define MDSC_THERMOSTAT_H

#include "mdsc/system.h"

typedef enum { THERMO_NONE=0, THERMO_BERENDSEN=1, THERMO_ANDERSEN=2 } thermostat_kind;

typedef struct {
  thermostat_kind kind;
  double T0;
  double tau;
  double nu;
} thermostat_t;

double temperature(const system_t* s);
void thermostat_apply(system_t* s, const thermostat_t* th, double dt);

double mdsc_rand_uniform();
double mdsc_rand_normal();

#endif
