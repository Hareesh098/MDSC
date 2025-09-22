#include "mdsc/integrator.h"

void vv_step(system_t* s, vv_t* vv, force_accum_t* acc, void (*force_fn)(system_t*, force_accum_t*))
{
  double dt = vv->dt;
  // half-kick, drift
  for (int i=0;i<s->n;++i){
    s->p[i].v.x += 0.5*dt*s->p[i].f.x;
    s->p[i].v.y += 0.5*dt*s->p[i].f.y;
    s->p[i].v.z += 0.5*dt*s->p[i].f.z;
    s->p[i].r.x += dt*s->p[i].v.x;
    s->p[i].r.y += dt*s->p[i].v.y;
    s->p[i].r.z += dt*s->p[i].v.z;
  }
  pbc_wrap(s);
  sys_zero_forces(s);
  force_fn(s, acc);
  // half-kick
  for (int i=0;i<s->n;++i){
    s->p[i].v.x += 0.5*dt*s->p[i].f.x;
    s->p[i].v.y += 0.5*dt*s->p[i].f.y;
    s->p[i].v.z += 0.5*dt*s->p[i].f.z;
  }
}
