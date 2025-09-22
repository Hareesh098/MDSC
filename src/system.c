#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mdsc/system.h"

static double frand()
{
  return rand() / (double)RAND_MAX;
}

void sys_alloc(system_t* s, int n)
{
  memset(s, 0, sizeof(*s));
  s->n = n;
  s->p = (particle_t*)calloc(n, sizeof(particle_t));
  if (!s->p) { fprintf(stderr, "alloc failed\n"); exit(1); }
}

void sys_free(system_t* s)
{
  free(s->p);
  free(s->head);
  free(s->next);
  memset(s, 0, sizeof(*s));
}

void sys_random_init(system_t* s, int n, double rho)
{
  sys_alloc(s, n);
  double vol = n / rho;
  double L = cbrt(vol);
  s->box[0] = s->box[1] = s->box[2] = L;
  for (int i = 0; i < n; ++i) {
    s->p[i].r = (vec3){ frand()*L, frand()*L, frand()*L };
    s->p[i].v = (vec3){ 0.0, 0.0, 0.0 };
    s->p[i].f = (vec3){ 0.0, 0.0, 0.0 };
  }
}

void sys_zero_forces(system_t* s)
{
  for (int i = 0; i < s->n; ++i) s->p[i].f = (vec3){0,0,0};
}

void min_image(system_t* s, vec3* dr)
{
  for (int k=0;k<3;++k) {
    double* comp = k==0? &dr->x : k==1? &dr->y : &dr->z;
    double L = s->box[k];
    if (*comp >  0.5*L) *comp -= L;
    if (*comp < -0.5*L) *comp += L;
  }
}

void pbc_wrap(system_t* s)
{
  for (int i=0;i<s->n;++i){
    double* r[3] = { &s->p[i].r.x, &s->p[i].r.y, &s->p[i].r.z };
    for (int k=0;k<3;++k){
      double L = s->box[k];
      if      (*r[k] >= L) *r[k] -= L*floor(*r[k]/L);
      else if (*r[k] < 0 ) *r[k] += L*ceil(-*r[k]/L);
    }
  }
}

double kinetic_energy(system_t* s)
{
  double ke=0.0;
  for (int i=0;i<s->n;++i){
    double v2 = s->p[i].v.x*s->p[i].v.x + s->p[i].v.y*s->p[i].v.y + s->p[i].v.z*s->p[i].v.z;
    ke += 0.5 * v2;
  }
  return ke;
}
