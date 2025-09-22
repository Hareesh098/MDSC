#ifndef MDSC_IO_H
#define MDSC_IO_H

#include <stdio.h>
#include "mdsc/system.h"
#include "mdsc/config.h"

typedef struct {
  const char* xyz_path;
  FILE* xyz;

  const char* log_path;
  FILE* log;

  const char* thermo_path;
  FILE* thermo;
  int thermo_every; // steps
} io_t;

void io_open_xyz(io_t* io, const char* path);
void io_close_all(io_t* io);
void io_write_xyz(io_t* io, const system_t* s, int step, double time);

// plain text logger
void io_open_log(io_t* io, const char* path);
void io_logf(io_t* io, const char* fmt, ...);

// thermo .dat
void io_open_thermo(io_t* io, const char* path, int every);
void io_write_thermo_header(io_t* io);
void io_write_thermo_row(io_t* io, double time, double ke, double pe, double T, double Etot);

// input file parser
int parse_file(mdsc_params_t* p, const char* filename, io_t* io);

#endif
