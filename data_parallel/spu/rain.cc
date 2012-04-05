
#include "rain.h"

void RainManager::initialize( volatile SPUArgs* args )
{

  buffer_size= (sizeof(Particle)*100);
  per_buffer = 100;

  x_step = z_step = xpos = zpos = 0.0f;

  vector float deltavec = spu_sub(args->lower_right_far, args->upper_left_near);

  float depth = spu_extract(deltavec, 2);
  float width = spu_extract(deltavec, 0);


  x_step = width / NDROPLETS * (100 - args->x_percent);
  z_step = depth / NDROPLETS * (100 - args->z_percent);

  int nx = (int) width / x_step;
  int nz = (int) depth / z_step;

  int nparticles = nx * nz;

  if(nparticles > NDROPLETS)
  {
    nparticles = NDROPLETS;

    x_step = width / sqrt(nparticles);

    z_step = depth / sqrt(nparticles);
  }


  int delta = nparticles % 10;


  nparticles -= delta;

  x_step = width / sqrt(nparticles);

//the renderer uses a left-handed coord system, so z_step must be -ve
  z_step = - (depth / sqrt(nparticles));

	left = spu_extract(args->upper_left_near, 0);
	near = spu_extract(args->upper_left_near, 2);
	right = spu_extract(args->lower_right_far, 0);
	far = spu_extract(args->lower_right_far, 2);
	
  min_height = spu_extract(args->lower_right_far, 1);
  max_height = spu_extract(args->upper_left_near, 1);

  xpos = left;
  zpos = near;
}

void RainManager::process_tasks()
{
  //UPDATE ALL EXPLOSIONS


  int nLoops = updates / buffer_size;

  int remainingBytes = updates % buffer_size;

  int eal = ealow;

  while( nLoops-4 >= 0)
  {
    //ENQUEUE TRANSFERS

    spu_mfcdma64(buffer[0], eahi, eal, buffer_size, 0, MFC_GET_CMD);
    spu_mfcdma64(buffer[1], eahi, eal + buffer_size, buffer_size, 1, MFC_GET_CMD);
    spu_mfcdma64(buffer[2], eahi, eal + (buffer_size * 2), buffer_size, 2, MFC_GET_CMD);
    spu_mfcdma64(buffer[3], eahi, eal + (buffer_size * 2), buffer_size, 3, MFC_GET_CMD);

    //USE DATA

    spu_writech(MFC_WrTagMask, 1<<0);
    spu_mfcstat(2);
    update(0, buffer_size);
    //Return data
    spu_mfcdma64(buffer[0], eahi, eal, buffer_size, 0, MFC_PUT_CMD);

    spu_writech(MFC_WrTagMask, 1<<1);
    spu_mfcstat(2);
    update(1, buffer_size);
    //Return data
    spu_mfcdma64(buffer[1], eahi, eal + buffer_size, buffer_size, 1, MFC_PUT_CMD);

    spu_writech(MFC_WrTagMask, 1<<2);
    spu_mfcstat(2);
    update(2, buffer_size);
    //Return data
    spu_mfcdma64(buffer[2], eahi, eal + (buffer_size * 2), buffer_size, 2, MFC_PUT_CMD);

    spu_writech(MFC_WrTagMask, 1<<3);
    spu_mfcstat(2);
    update(3, buffer_size);
    //Return data
    spu_mfcdma64(buffer[3], eahi, eal + (buffer_size * 3), buffer_size, 3, MFC_PUT_CMD);

    //wait for all puts
    spu_writech(MFC_WrTagMask, 1<<0 | 1<<1 | 1<<2 | 1<<3);
    spu_mfcstat(2);

    nLoops-=4;
    eal += buffer_size * 4;
  }

  if( nLoops-3 >= 0)
  {
    //ENQUEUE TRANSFERS

    spu_mfcdma64(buffer[0], eahi, eal, buffer_size, 0, MFC_GET_CMD);
    spu_mfcdma64(buffer[1], eahi, eal + buffer_size, buffer_size, 1, MFC_GET_CMD);
    spu_mfcdma64(buffer[2], eahi, eal + (buffer_size * 2), buffer_size, 2, MFC_GET_CMD);

    //USE DATA

    spu_writech(MFC_WrTagMask, 1<<0);
    spu_mfcstat(2);
    update(0, buffer_size);
    //Return data
    spu_mfcdma64(buffer[0], eahi, eal, buffer_size, 0, MFC_PUT_CMD);

    spu_writech(MFC_WrTagMask, 1<<1);
    spu_mfcstat(2);
    update(1, buffer_size);
    //Return data
    spu_mfcdma64(buffer[1], eahi, eal + buffer_size, buffer_size, 1, MFC_PUT_CMD);

    spu_writech(MFC_WrTagMask, 1<<2);
    spu_mfcstat(2);
    update(2, buffer_size);
    //Return data
    spu_mfcdma64(buffer[2], eahi, eal + (buffer_size * 2), buffer_size, 2, MFC_PUT_CMD);

    //wait for all puts
    spu_writech(MFC_WrTagMask, 1<<0 | 1<<1 | 1<<2);
    spu_mfcstat(2);

    nLoops-=3;
    eal += buffer_size * 3;
  }

  if( nLoops-2 >= 0 )
  {

    //ENQUEUE TRANSFERS
    spu_mfcdma64(buffer[0], eahi, eal, buffer_size, 0, MFC_GET_CMD);
    spu_mfcdma64(buffer[1], eahi, eal + buffer_size, buffer_size, 1, MFC_GET_CMD);
 
    //USE DATA

    spu_writech(MFC_WrTagMask, 1<<0);
    spu_mfcstat(2);
    update(0, buffer_size);
    //Return data
    spu_mfcdma64(buffer[0], eahi, eal, buffer_size, 0, MFC_PUT_CMD);

    spu_writech(MFC_WrTagMask, 1<<1);
    spu_mfcstat(2);
    update(1, buffer_size);
    //Return data
    spu_mfcdma64(buffer[1], eahi, eal + buffer_size, buffer_size, 1, MFC_PUT_CMD);

    //wait for all puts
    spu_writech(MFC_WrTagMask, 1<<0 | 1<<1);
    spu_mfcstat(2);

    nLoops-=2;
    eal += buffer_size * 2;
  }
  if( nLoops-1 >=0 )
  {
    spu_mfcdma64( buffer[0], eahi, eal, buffer_size, 1<<12, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1<<12);
    spu_mfcstat(2);

    update(0, remainingBytes / sizeof(Particle) );

    spu_mfcdma64( buffer[0], eahi, eal, buffer_size, 1<<12, MFC_PUT_CMD);
    spu_writech(MFC_WrTagMask, 1<<12);
    spu_mfcstat(2);
    
    eal += buffer_size;
  }

  if( remainingBytes >0 )//<100 particles
  {
    spu_mfcdma64( buffer[0], eahi, eal, remainingBytes, 1<<12, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1<<12);
    spu_mfcstat(2);

    update( 0, remainingBytes / sizeof(Particle) );

    spu_mfcdma64( buffer[0], eahi, eal, remainingBytes, 1<<12, MFC_PUT_CMD);
    spu_writech(MFC_WrTagMask, 1<<12);
    spu_mfcstat(2);
  }

  //CREATES
  if( creates )
  {
    int index = 0;
    volatile Particle rain[creates] __attribute__ ((aligned(16)));
    while( creates )
    {
      --creates;
      if( xpos > right )
	    {
        xpos = left;
		    zpos += z_step;
      }
      else
        xpos += x_step;

      rain[index].packet.pos = _load_vec_float4(xpos, max_height, xpos, 1.0f);
      rain[index].acc = _load_vec_float4(0.0f, -0.1f, 0.0f, 0.0f);//should be a single global since all have the same acceleration
      rain[index].packet.rgba = _load_vec_float4(1.0f, 1.0f, 0.1f, 1.0f);

      ++updates;
      spu_mfcdma64(&rain[index], eahi, ealow + (sizeof(Particle) * updates), buffer_size, 0, MFC_PUTF_CMD);//send to ea array at the last used index + 1
      ++index;
    }
  
    spu_writech(MFC_WrTagMask, 1<<0);
    spu_mfcstat(2);
  }
}

void RainManager::update(int buf, int i)
{
  vector float new_pos;
	while( i )//i is quantity of particles
  {
    --i;

    new_pos = spu_msub( buffer[buf][i].packet.pos, timesTwo, buffer[buf][i].last_pos );// temp_pos = pos*2 - last_pos +
    new_pos = spu_madd( buffer[buf][i].acc, delta_time_sq_vec, new_pos );// acc * delta_time * delta_time

    if(spu_extract(new_pos, 1) <= min_height)
    {
      //reset
      spu_insert(max_height, buffer[buf][i].packet.pos ,1);
      spu_insert(max_height, buffer[buf][i].last_pos ,1);
      buffer[buf][i].acc = _load_vec_float4(0.0f, -0.1f, 0.0f, 0.0f);//should be s single global since all have the same acceleration
    }
    else
    {
      buffer[buf][i].last_pos = buffer[buf][i].packet.pos;
      buffer[buf][i].packet.pos = new_pos;
    }
  }
}
