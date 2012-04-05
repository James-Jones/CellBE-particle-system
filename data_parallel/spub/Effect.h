#ifndef EFFECT_H
#define EFFECT_H

#include "common.h"
#include <spu_mfcio.h>
#include <load_vec_float4.h>
#include <libmisc.h>
#include <spu/sys/mman.h>//SPU C Lib
#include <sync/spu/libsync.h>
#include <libgmath.h>
#include <libvector.h>
#include <cmath>
#include <length_vec3.h>

// Base class for particle effects.

class Effect
{
protected:

  int buffer_size;
  int per_buffer;

unsigned int updates;
unsigned int creates;

unsigned int eahi;
unsigned int ealow;

unsigned int ea_array_size;

unsigned int eal;

static vector float timesTwo;

static vector float delta_time_sq_vec;

public:
virtual ~Effect(){}

static void set_dtime2( vector float v ) {delta_time_sq_vec = v;}
void set_target_size(unsigned int s){ea_array_size = s;}
void set_target(unsigned int lowword, unsigned int hiword){ eahi = hiword; ealow = lowword;}

unsigned int get_updates(){ return updates; }

void add_task(int);

};


#endif
