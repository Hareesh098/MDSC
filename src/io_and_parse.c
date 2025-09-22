#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "mdsc/io.h"
#include "mdsc/thermostat.h"

void io_open_xyz(io_t* io, const char* path)
{
  io->xyz_path = path;
  io->xyz = path ? fopen(path, "w") : NULL;
  if (path && !io->xyz) { perror("xyz open"); }
}

void io_open_log(io_t* io, const char* path)
{
  io->log_path = path;
  io->log = path ? fopen(path, "w") : NULL;
  if (path && !io->log) { perror("log open"); }
}

void io_logf(io_t* io, const char* fmt, ...)
{
  if (!io->log) return;
  va_list ap;
  va_start(ap, fmt);
  vfprintf(io->log, fmt, ap);
  va_end(ap);
  fflush(io->log);
}

void io_open_thermo(io_t* io, const char* path, int every)
{
  io->thermo_path = path;
  io->thermo_every = every > 0 ? every : 100;
  io->thermo = path ? fopen(path, "w") : NULL;
  if (path && !io->thermo) { perror("thermo open"); }
  if (io->thermo) {
    fprintf(io->thermo, "# time  KE  PE  T  Etot\n");
    fflush(io->thermo);
  }
}

void io_write_thermo_header(io_t* io)
{
  if (!io->thermo) return;
  fprintf(io->thermo, "# time  KE  PE  T  Etot\n");
  fflush(io->thermo);
}

void io_write_thermo_row(io_t* io, double time, double ke, double pe, double T, double Etot)
{
  if (!io->thermo) return;
  fprintf(io->thermo, "%.8f  %.10g  %.10g  %.10g  %.10g\n", time, ke, pe, T, Etot);
}

void io_write_xyz(io_t* io, const system_t* s, int step, double time)
{
  if (!io->xyz) return;
  fprintf(io->xyz, "%d\n# step %d time %g\n", s->n, step, time);
  for (int i=0;i<s->n;++i){
    fprintf(io->xyz, "Ar %g %g %g\n", s->p[i].r.x, s->p[i].r.y, s->p[i].r.z);
  }
}

void io_close_all(io_t* io)
{
  if (io->xyz) fclose(io->xyz);
  io->xyz = NULL;
  if (io->log) fclose(io->log);
  io->log = NULL;
  if (io->thermo) fclose(io->thermo);
  io->thermo = NULL;
}

static int eq(const char* a, const char* b){ return strcmp(a,b)==0; }

int parse_file(mdsc_params_t* p, const char* filename, io_t* io)
{
  FILE* f = fopen(filename, "r");
  if (!f) { perror("input file"); return 1; }

  // defaults
  p->n = 256;
  p->rho = 0.7;
  p->nsteps = 1000;
  p->dt = 0.005;
  p->rc = 2.5;
  p->use_cuda = 0;
  p->thermo_kind = 0; p->T0=1.0; p->tau=1.0; p->nu=0.1;

  io->xyz_path = NULL; io->log_path=NULL; io->thermo_path="thermo.dat"; io->thermo_every=100;

  char line[1024];
  while (fgets(line, sizeof(line), f)){
    char* s = line;
    while (*s==' '||*s=='\t') ++s;
    if (*s=='#' || *s=='\n' || *s=='\0') continue;
    char *key = strtok(s, " \t\r\n");
    char *val = strtok(NULL, "\r\n");
    if (!key || !val) continue;
    while (*val==' '||*val=='\t') ++val;

    if      (eq(key,"n")) p->n = atoi(val);
    else if (eq(key,"rho")) p->rho = atof(val);
    else if (eq(key,"steps")) p->nsteps = atoi(val);
    else if (eq(key,"dt")) p->dt = atof(val);
    else if (eq(key,"rc")) p->rc = atof(val);
    else if (eq(key,"cuda")) p->use_cuda = atoi(val);
    else if (eq(key,"thermo")){
      if      (eq(val,"none")) p->thermo_kind = 0;
      else if (eq(val,"berendsen")) p->thermo_kind = 1;
      else if (eq(val,"andersen")) p->thermo_kind = 2;
    }
    else if (eq(key,"T0")) p->T0 = atof(val);
    else if (eq(key,"tau")) p->tau = atof(val);
    else if (eq(key,"nu"))  p->nu  = atof(val);
    else if (eq(key,"xyz")) io->xyz_path = strdup(val);
    else if (eq(key,"log")) io->log_path = strdup(val);
    else if (eq(key,"thermo_out")) io->thermo_path = strdup(val);
    else if (eq(key,"thermo_every")) io->thermo_every = atoi(val);
  }
  fclose(f);
  return 0;
}
