#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "mdsc/neigh.h"

void sys_build_cells(system_t* s)
{
  // cell size ≈ rc
  for (int k=0;k<3;++k){
    s->nc[k] = (int)floor(s->box[k] / s->rc);
    if (s->nc[k] < 1) s->nc[k] = 1;
    s->cell[k] = s->box[k] / s->nc[k];
  }
  int ncells = s->nc[0]*s->nc[1]*s->nc[2];
  free(s->head); free(s->next);
  s->head = (int*)malloc(sizeof(int)*ncells);
  s->next = (int*)malloc(sizeof(int)*s->n);
  for (int i=0;i<ncells;++i) s->head[i] = -1;
  for (int i=0;i<s->n;++i) s->next[i] = -1;

  // fill
  for (int i=0;i<s->n;++i){
    int ix = (int)floor(s->p[i].r.x / s->cell[0]); if (ix<0) ix=0; if (ix>=s->nc[0]) ix=s->nc[0]-1;
    int iy = (int)floor(s->p[i].r.y / s->cell[1]); if (iy<0) iy=0; if (iy>=s->nc[1]) iy=s->nc[1]-1;
    int iz = (int)floor(s->p[i].r.z / s->cell[2]); if (iz<0) iz=0; if (iz>=s->nc[2]) iz=s->nc[2]-1;
    int cid = ix + s->nc[0]*(iy + s->nc[1]*iz);
    s->next[i] = s->head[cid];
    s->head[cid] = i;
  }
}

static inline int cell_id(system_t* s, int ix,int iy,int iz) {
  return ix + s->nc[0]*(iy + s->nc[1]*iz);
}

static inline int pmod(int a, int m){ int r=a%m; return r<0?r+m:r; }

void for_each_pair(system_t* s, void (*pair_fn)(system_t*, int, int, void*), void* userdata)
{
  int nx=s->nc[0], ny=s->nc[1], nz=s->nc[2];
  for (int ix=0; ix<nx; ++ix)
  for (int iy=0; iy<ny; ++iy)
  for (int iz=0; iz<nz; ++iz){
    int cid = cell_id(s, ix,iy,iz);
    for (int dx=-1; dx<=1; ++dx)
    for (int dy=-1; dy<=1; ++dy)
    for (int dz=-1; dz<=1; ++dz){
      int jx=pmod(ix+dx,nx), jy=pmod(iy+dy,ny), jz=pmod(iz+dz,nz);
      int nid = cell_id(s, jx,jy,jz);
      if (nid < cid) continue; // enforce ordering to avoid double counting across cells
      for (int i = s->head[cid]; i!=-1; i = s->next[i]){
        for (int j = s->head[nid]; j!=-1; j = s->next[j]){
          if (nid==cid && j<=i) continue; // unique pairs within same cell
          pair_fn(s, i, j, userdata);
        }
      }
    }
  }
}

void build_verlet(system_t* s){ sys_build_cells(s); }
