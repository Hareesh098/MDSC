#include <math.h>
#include "mdsc/force.h"
#include "mdsc/neigh.h"
#include "mdsc/vec.h"

typedef struct { double rc2; double pe; } lj_ctx;

static void pair_lj(system_t* s, int i, int j, void* userdata)
{
  lj_ctx* ctx = (lj_ctx*)userdata;
  vec3 dr = (vec3){
    s->p[i].r.x - s->p[j].r.x,
    s->p[i].r.y - s->p[j].r.y,
    s->p[i].r.z - s->p[j].r.z
  };
  min_image(s, &dr);
  double r2 = dr.x*dr.x + dr.y*dr.y + dr.z*dr.z;
  if (r2 >= ctx->rc2 || r2 == 0.0) return;

  double inv2 = 1.0/r2;
  double inv6 = inv2*inv2*inv2;
  double inv12 = inv6*inv6;
  double fscal = (48.0*inv12 - 24.0*inv6) * inv2;
  s->p[i].f.x += fscal*dr.x;
  s->p[i].f.y += fscal*dr.y;
  s->p[i].f.z += fscal*dr.z;
  s->p[j].f.x -= fscal*dr.x;
  s->p[j].f.y -= fscal*dr.y;
  s->p[j].f.z -= fscal*dr.z;

  // LJ potential 4*(r^-12 - r^-6)
  ctx->pe += 4.0*(inv6*inv6 - inv6);
}

void lj_forces(system_t* s, force_accum_t* acc)
{
  acc->pe = 0.0;
  lj_ctx ctx = { .rc2 = s->rc2, .pe = 0.0 };
  build_verlet(s);
  for_each_pair(s, pair_lj, &ctx);
  acc->pe = ctx.pe;
}
