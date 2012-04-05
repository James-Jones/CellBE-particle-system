#ifndef COMMON_H
#define COMMON_H

#ifdef __SPU__
#include <spu_intrinsics.h>

#else
#include <altivec.h>
#include <ppu_intrinsics.h>

#endif

#include <libvector.h>
#include <libsync.h>

#define NEXP 50
#define NDROPLETS 700



typedef union
{
	unsigned long long ull;
	unsigned int ui[2];
	void* p;
} addr64_t;

class Packet
{
 public:
  vector float pos;
  vector float rgba;
};

class Particle
{
 public:
  float active;
  Packet packet;
  vector float last_pos;
  vector float acc;
  
  /* Total: 68 bytes */
  char pad[12];//80 bytes
};

class Exp
{
public:
  Particle debris[40];
  vector float origin;// zero w means inactive
  unsigned int radius;
  //Total: 3220 bytes
char pad[12];//3232
};


//size is a multiple of 16 bytes, for a valid DMA transfer size.

class SPUArgsExp
{
 public:
  addr64_t exp_1_ea __attribute__ (( aligned(128) ));//8
  addr64_t exp_2_ea;//8
  unsigned int array_length;//4
  
  atomic_ea_t switch_buffer_a;
	atomic_ea_t elast_index_a;

	unsigned int timebase;
	
  /* Total: 40 bytes */
	char pad[8];//48 bytes

};

class SPUArgsRain
{
 public:
  addr64_t rain_1_ea __attribute__ (( aligned(128) ));//8 bytes
  addr64_t rain_2_ea;//8
  unsigned int array_length;//4

  vector float upper_left_near;//16
  vector float lower_right_far;//16
  unsigned int x_percent;//4
  unsigned int z_percent;//4

  atomic_ea_t switch_buffer_a;
	atomic_ea_t rlast_index_a;

	unsigned int timebase;

  /* Total: 80 bytes */
};

const unsigned int SPU_GO = 1;
const unsigned int SPU_READY = 2;
const unsigned int STOP_SIGNAL = 3;
//Task Intruction Type
const unsigned int NEW_TASK = 4;
const unsigned int EXPLOSION = 5;
const unsigned int RAIN = 6;

typedef union
{
	unsigned int ui;
	unsigned short us[2];//0 = message, 1 = data
} msg_t;

#endif
