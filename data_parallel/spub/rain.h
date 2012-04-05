#ifndef RAIN_H
#define RAIN_H

#include "Effect.h"
#include <cassert>

class RainManager : public Effect
{
protected:

  volatile Particle buffer[4][ 100 ] __attribute__ (( aligned(128) ));

  float x_step;
  float z_step;

  float xpos;
  float zpos;

	float left;
	float near;
	float right;
	float far;

  float min_height;
  float max_height;

  void update(int buf, int i);

public:
  RainManager():Effect(){}

  void initialize( volatile SPUArgs* );

  void process_tasks();

  void validate_buffer_size()
  {
	 assert( buffer_size <= 16 * 1024 );
	 assert( buffer_size % 16 == 0 );
  }
};

#endif
