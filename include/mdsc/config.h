#ifndef MDSC_CONFIG_H
#define MDSC_CONFIG_H

#define MDSC_DIM 3

typedef struct {
  int use_cuda;
  int nsteps;
  double dt;
  double rho;
  double rc;
  int n;
  // thermostat
  int thermo_kind; // 0 none, 1 Berendsen, 2 Andersen
  double T0;
  double tau;
  double nu;
} mdsc_params_t;

#endif
