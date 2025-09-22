#include <math.h>
#include <stdlib.h>
#include "mdsc/thermostat.h"

static int seeded = 0;
double mdsc_rand_uniform()
{
  if (!seeded){ srand(12345u); seeded=1; } // deterministic for tests; replace with time(NULL) if desired
  return rand()/(double)RAND_MAX;
}

double mdsc_rand_normal()
{
  // Box-Muller
  double u1 = mdsc_rand_uniform();
  double u2 = mdsc_rand_uniform();
  double r = sqrt(-2.0*log(fmax(u1,1e-12)));
  double th = 2.0*M_PI*u2;
  return r*cos(th);
}

double temperature(const system_t* s)
{
  // k_B = 1, m = 1
  double ke = 0.0;
  for (int i=0;i<s->n;++i){
    double vx=s->p[i].v.x, vy=s->p[i].v.y, vz=s->p[i].v.z;
    ke += 0.5*(vx*vx+vy*vy+vz*vz);
  }
  int dof = 3*s->n; // no constraints here
  return (2.0*ke)/dof;
}

static void berendsen(system_t* s, double T0, double tau, double dt)
{
  double T = temperature(s);
  if (T<=0.0) return;
  double lambda = sqrt(1.0 + (dt/tau) * ((T0/T) - 1.0));
  for (int i=0;i<s->n;++i){
    s->p[i].v.x *= lambda;
    s->p[i].v.y *= lambda;
    s->p[i].v.z *= lambda;
  }
}

static void andersen(system_t* s, double T0, double nu, double dt)
{
  double p = 1.0 - exp(-nu*dt); // collision probability
  double sigma = sqrt(T0); // m=1, kB=1 → variance = T
  for (int i=0;i<s->n;++i){
    if (mdsc_rand_uniform() < p){
      s->p[i].v.x = sigma*mdsc_rand_normal();
      s->p[i].v.y = sigma*mdsc_rand_normal();
      s->p[i].v.z = sigma*mdsc_rand_normal();
    }
  }
}

void thermostat_apply(system_t* s, const thermostat_t* th, double dt)
{
  if (!th) return;
  switch (th->kind){
    case THERMO_BERENDSEN: berendsen(s, th->T0, th->tau, dt); break;
    case THERMO_ANDERSEN:  andersen(s, th->T0, th->nu, dt);   break;
    default: break;
  }
}
