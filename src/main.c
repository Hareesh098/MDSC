#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mdsc/config.h"
#include "mdsc/system.h"
#include "mdsc/force.h"
#include "mdsc/integrator.h"
#include "mdsc/io.h"
#include "mdsc/thermostat.h"

int main(int argc, char** argv)
{
  mdsc_params_t prm;
  io_t io = {0};
  const char* infile = "in.mdsc";
  if (argc>=2) infile = argv[1];
  if (parse_file(&prm, infile, &io)) return 1;

  system_t s;
  sys_random_init(&s, prm.n, prm.rho);
  s.rc  = prm.rc;
  s.rc2 = prm.rc*prm.rc;

  io_open_xyz(&io, io.xyz_path);
  io_open_log(&io, io.log_path);
  io_open_thermo(&io, io.thermo_path, io.thermo_every);

  vv_t vv = { .dt = prm.dt };
  force_accum_t acc;
  sys_zero_forces(&s);

  void (*force_fn)(system_t*, force_accum_t*) = lj_forces;
#ifdef USE_CUDA
  if (prm.use_cuda) force_fn = lj_forces_cuda;
#endif

  io_logf(&io, "MDSC start: n=%d rho=%.6f dt=%.6f steps=%d rc=%.3f\n", prm.n, prm.rho, prm.dt, prm.nsteps, prm.rc);
  force_fn(&s, &acc);

  thermostat_t th = { .kind = (thermostat_kind)prm.thermo_kind, .T0 = prm.T0, .tau = prm.tau, .nu = prm.nu };

  for (int step=0; step<prm.nsteps; ++step){
    vv_step(&s, &vv, &acc, force_fn);
    thermostat_apply(&s, &th, prm.dt);

    if (io.xyz && (step % 10 == 0)) io_write_xyz(&io, &s, step, step*prm.dt);

    if (io.thermo && (step % io.thermo_every == 0)){
      double t = step*prm.dt;
      double ke = kinetic_energy(&s);
      double T = temperature(&s);
      double pe = acc.pe;
      double Etot = ke + pe;
      io_write_thermo_row(&io, t, ke, pe, T, Etot);
    }

    if (step % 100 == 0){
      double ke = kinetic_energy(&s);
      io_logf(&io, "step %8d  KE %12.6f  PE %12.6f\n", step, ke, acc.pe);
      printf("step %8d  KE %12.6f\n", step, ke);
    }
  }

  io_close_all(&io);
  sys_free(&s);
  return 0;
}
