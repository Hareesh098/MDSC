#include <cuda_runtime.h>
#include "mdsc/force.h"
#include "mdsc/vec.h"

extern "C" {

__global__ void lj_kernel(const double* __restrict__ rx,
                          const double* __restrict__ ry,
                          const double* __restrict__ rz,
                          double* __restrict__ fx,
                          double* __restrict__ fy,
                          double* __restrict__ fz,
                          double* __restrict__ pe_arr,
                          int n, double Lx, double Ly, double Lz, double rc2)
{
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  if (i>=n) return;
  double rxi = rx[i], ryi = ry[i], rzi = rz[i];
  double fxi=0.0, fyi=0.0, fzi=0.0;
  double pei=0.0;
  for (int j=0;j<n;++j){
    if (j==i) continue;
    double dx = rxi - rx[j];
    double dy = ryi - ry[j];
    double dz = rzi - rz[j];
    // minimum image
    if (dx >  0.5*Lx) dx -= Lx; if (dx < -0.5*Lx) dx += Lx;
    if (dy >  0.5*Ly) dy -= Ly; if (dy < -0.5*Ly) dy += Ly;
    if (dz >  0.5*Lz) dz -= Lz; if (dz < -0.5*Lz) dz += Lz;
    double r2 = dx*dx + dy*dy + dz*dz;
    if (r2 >= rc2 || r2==0.0) continue;
    double inv2 = 1.0/r2;
    double inv6 = inv2*inv2*inv2;
    double inv12 = inv6*inv6;
    double fscal = (48.0*inv12 - 24.0*inv6) * inv2;
    fxi += fscal*dx;
    fyi += fscal*dy;
    fzi += fscal*dz;
    // Potential (counted twice across i and j; we'll halve later on host)
    pei += 4.0*(inv6*inv6 - inv6);
  }
  fx[i] = fxi; fy[i] = fyi; fz[i] = fzi;
  pe_arr[i] = pei;
}

void lj_forces_cuda(system_t* s, force_accum_t* acc)
{
  int n = s->n;
  double *rx, *ry, *rz, *fx, *fy, *fz, *pe_arr;
  cudaMalloc(&rx, n*sizeof(double));
  cudaMalloc(&ry, n*sizeof(double));
  cudaMalloc(&rz, n*sizeof(double));
  cudaMalloc(&fx, n*sizeof(double));
  cudaMalloc(&fy, n*sizeof(double));
  cudaMalloc(&fz, n*sizeof(double));
  cudaMalloc(&pe_arr, n*sizeof(double));

  double *hrx=(double*)malloc(n*sizeof(double));
  double *hry=(double*)malloc(n*sizeof(double));
  double *hrz=(double*)malloc(n*sizeof(double));
  for (int i=0;i<n;++i){ hrx[i]=s->p[i].r.x; hry[i]=s->p[i].r.y; hrz[i]=s->p[i].r.z; }

  cudaMemcpy(rx, hrx, n*sizeof(double), cudaMemcpyHostToDevice);
  cudaMemcpy(ry, hry, n*sizeof(double), cudaMemcpyHostToDevice);
  cudaMemcpy(rz, hrz, n*sizeof(double), cudaMemcpyHostToDevice);

  int block=128, grid=(n+block-1)/block;
  lj_kernel<<<grid,block>>>(rx,ry,rz,fx,fy,fz,pe_arr,n,s->box[0],s->box[1],s->box[2],s->rc2);
  cudaDeviceSynchronize();

  double *hfx=(double*)malloc(n*sizeof(double));
  double *hfy=(double*)malloc(n*sizeof(double));
  double *hfz=(double*)malloc(n*sizeof(double));
  double *hpe=(double*)malloc(n*sizeof(double));
  cudaMemcpy(hfx, fx, n*sizeof(double), cudaMemcpyDeviceToHost);
  cudaMemcpy(hfy, fy, n*sizeof(double), cudaMemcpyDeviceToHost);
  cudaMemcpy(hfz, fz, n*sizeof(double), cudaMemcpyDeviceToHost);
  cudaMemcpy(hpe, pe_arr, n*sizeof(double), cudaMemcpyDeviceToHost);

  for (int i=0;i<n;++i){
    s->p[i].f.x = hfx[i];
    s->p[i].f.y = hfy[i];
    s->p[i].f.z = hfz[i];
  }
  // Sum potential (each pair counted twice in the simple O(N^2) kernel)
  double pe_sum = 0.0;
  for (int i=0;i<n;++i) pe_sum += hpe[i];
  acc->pe = 0.5 * pe_sum;

  free(hrx); free(hry); free(hrz);
  free(hfx); free(hfy); free(hfz);
  free(hpe);
  cudaFree(rx); cudaFree(ry); cudaFree(rz);
  cudaFree(fx); cudaFree(fy); cudaFree(fz);
  cudaFree(pe_arr);
}

} // extern "C"
