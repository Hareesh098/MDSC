#ifndef MDSC_VEC_H
#define MDSC_VEC_H

#include <math.h>

typedef struct { double x, y, z; } vec3;

static inline vec3 v3(double x,double y,double z){ return (vec3){x,y,z}; }
static inline vec3 v_add(vec3 a, vec3 b){ return v3(a.x+b.x, a.y+b.y, a.z+b.z); }
static inline vec3 v_sub(vec3 a, vec3 b){ return v3(a.x-b.x, a.y-b.y, a.z-b.z); }
static inline vec3 v_scale(vec3 a, double s){ return v3(a.x*s, a.y*s, a.z*s); }
static inline double v_dot(vec3 a, vec3 b){ return a.x*b.x + a.y*b.y + a.z*b.z; }
static inline double v_norm2(vec3 a){ return v_dot(a,a); }
static inline double v_norm(vec3 a){ return sqrt(v_norm2(a)); }

#endif
