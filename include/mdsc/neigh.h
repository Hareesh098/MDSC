#ifndef MDSC_NEIGH_H
#define MDSC_NEIGH_H

#include "mdsc/system.h"

void build_verlet(system_t* s);
void for_each_pair(system_t* s, void (*pair_fn)(system_t*, int, int, void*), void* userdata);

#endif
