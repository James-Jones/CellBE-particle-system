
#include "exp.h"

void ExplosionManager::initialize()
{
  buffer_size = (sizeof(Exp)*5);
  per_buffer = 5;

	mean_colour = _load_vec_float4(1.0f, 0.0f, 0.0f, 0.0f);

	variance_colour = _load_vec_float4(1.0f,1.0f,1.0f,1.0f);
	
	speed = _load_vec_float4(0.1f, 0.1f, 0.1f, 0.1f);
	
}

void ExplosionManager::process_tasks()
{

  //UPDATE ALL EXPLOSIONS
  
  int nLoops = updates / buffer_size;

  int remainingBytes = updates % buffer_size;

  eal = ealow;
  
  while( nLoops-4 >= 0)
  {
    //ENQUEUE TRANSFERS

    spu_mfcdma64(buffer[0], eahi, eal, buffer_size, 0, MFC_GET_CMD);
    spu_mfcdma64(buffer[1], eahi, eal + buffer_size, buffer_size, 1, MFC_GET_CMD);
    spu_mfcdma64(buffer[2], eahi, eal + (buffer_size * 2), buffer_size, 2, MFC_GET_CMD);
    spu_mfcdma64(buffer[3], eahi, eal + (buffer_size * 3), buffer_size, 3, MFC_GET_CMD);

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
    spu_mfcdma64(buffer[1], eahi, eal, buffer_size, 1, MFC_PUT_CMD);
  
    spu_writech(MFC_WrTagMask, 1<<2);
    spu_mfcstat(2);
    update(2, buffer_size);
    //Return data
    spu_mfcdma64(buffer[2], eahi, eal, buffer_size, 2, MFC_PUT_CMD);
  
    spu_writech(MFC_WrTagMask, 1<<3);
    spu_mfcstat(2);
    update(3, buffer_size);
    //Return data
    spu_mfcdma64(buffer[3], eahi, eal, buffer_size, 3, MFC_PUT_CMD);
  
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

    update(0, remainingBytes);

    spu_mfcdma64( buffer[0], eahi, eal, buffer_size, 1<<12, MFC_PUT_CMD);
    spu_writech(MFC_WrTagMask, 1<<12);
    spu_mfcstat(2);
    
    eal += buffer_size;
  }

  if( remainingBytes >0 )//<40 particles
  {

    spu_mfcdma64( buffer[0], eahi, eal, remainingBytes, 1<<12, MFC_GET_CMD);
    spu_writech(MFC_WrTagMask, 1<<12);
    spu_mfcstat(2);

    update( 0, remainingBytes);

    spu_mfcdma64( buffer[0], eahi, ealow, remainingBytes, 1<<12, MFC_PUT_CMD);
    spu_writech(MFC_WrTagMask, 1<<12);
    spu_mfcstat(2);
  }




  //PROCESS ALL CREATE TASKS after updates otherwise the newly created data would be updated unnecessarily
  if( creates )
  {
    int index = 0;
    volatile Exp exp[creates] __attribute__ ((aligned(16)));
    while( creates )
    {
      if( updates + 1 > ea_array_size)
        break;

      --creates;

      exp[index].origin = spu_mul( rand_0_to_1_v(), spu_splats(5.0f) );

      unsigned int i = 40 ;
      while (i)
      {
        vector float randomv = rand_minus1_to_1_v();
        float length = exp->radius * rand_0_to_1();
        float rtheta = 6.283185 * rand_minus1_to_1();
        float rphi = 6.283185 * rand_minus1_to_1();

        exp[index].debris[i].packet.pos = _load_vec_float4( length * sin8(rphi) * cos8(rtheta), length * sin8(rphi) * sin8(rtheta), length * cos8(rphi), 1.0f);
        exp[index].debris[i].acc = spu_mul(spu_sub(exp[index].debris[i].packet.pos, exp[index].origin), speed);
        exp[index].debris[i].packet.rgba = spu_madd(mean_colour, variance_colour, randomv );
        exp[index].debris[i].active = 10.0f;

        rtheta = 6.283185 * rand_minus1_to_1();
        rphi = 6.283185 * rand_minus1_to_1();

        exp[index].debris[i].packet.pos = _load_vec_float4( length * sin8(rphi) * cos8(rtheta), length * sin8(rphi) * sin8(rtheta), length * cos8(rphi), 1.0f);
        exp[index].debris[i].acc = spu_mul(spu_sub(exp[index].debris[i].packet.pos, exp[index].origin), speed);
        exp[index].debris[i].packet.rgba = spu_madd(mean_colour, variance_colour, randomv );
        exp[index].debris[i].active = 10.0f;

        i -= 2;
      }
      ++updates;
    
      if( recycle.get_size() > 0 )
        spu_mfcdma64(&exp[index], eahi, ealow + (sizeof(Exp) * recycle.pop() ), buffer_size, 0, MFC_PUTF_CMD);//send to ea array at unused element
      else
        spu_mfcdma64(&exp[index], eahi, ealow + (sizeof(Exp) * updates), buffer_size, 0, MFC_PUTF_CMD);//send to ea array at the last used index + 1

      ++index;
    }

    //wait for transfers to finish before the function scoped exp array is released
    spu_writech(MFC_WrTagMask, 1<<0);
    spu_mfcstat(2);
  }

}


void ExplosionManager::update(unsigned int buf, unsigned int nExp)
{
  unsigned int n = 0;
  while( (n+1) < nExp )
  {
    
    if(spu_extract(buffer[buf][n].origin, 4) >= 0.0f)
    {
      vector float new_pos;
      int inactive_count = 0;
      volatile Particle* debris = buffer[buf][n].debris;

      int i =0;
      while( i < 40 )
      {

        new_pos = spu_msub( debris[i].packet.pos, timesTwo, debris[i].last_pos );// temp_pos = pos*2 - last_pos +
        new_pos = spu_madd( debris[i].acc, delta_time_sq_vec, new_pos );// acc * delta_time * delta_time
        debris[i].last_pos = debris[i].packet.pos;
        debris[i].packet.pos = new_pos;
        debris[i].active -= fabs( _length_vec3( spu_sub( debris[i].last_pos, debris[i].packet.pos ) ) );
        if( debris[i].active <= 0.0f )
        {
          ++inactive_count;
    	    spu_insert( -5.0f, debris[i].packet.pos, 2);//behind camera
        }

	      ++i;

        new_pos = spu_msub( debris[i].packet.pos, timesTwo, debris[i].last_pos );// temp_pos = pos*2 - last_pos +
        new_pos = spu_madd( debris[i].acc, delta_time_sq_vec, new_pos );// acc * delta_time * delta_time
        debris[i].last_pos = debris[i].packet.pos;
        debris[i].packet.pos = new_pos;
        debris[i].active -= fabs( _length_vec3( spu_sub( debris[i].last_pos, debris[i].packet.pos ) ) );
        if( debris[i].active <= 0.0f )
        {
          ++inactive_count;
	        spu_insert( -5.0f, debris[i].packet.pos, 2);//behind camera
        }

	      ++i;

        new_pos = spu_msub( debris[i].packet.pos, timesTwo, debris[i].last_pos );// temp_pos = pos*2 - last_pos +
        new_pos = spu_madd( debris[i].acc, delta_time_sq_vec, new_pos );// acc * delta_time * delta_time
        debris[i].last_pos = debris[i].packet.pos;
        debris[i].packet.pos = new_pos;
        debris[i].active -= fabs( _length_vec3( spu_sub( debris[i].last_pos, debris[i].packet.pos ) ) );
        if( debris[i].active <= 0.0f )
        {
          ++inactive_count;
	        spu_insert( -5.0f, debris[i].packet.pos, 2);//behind camera
        }

	      ++i;

        new_pos = spu_msub( debris[i].packet.pos, timesTwo, debris[i].last_pos );// temp_pos = pos*2 - last_pos +
        new_pos = spu_madd( debris[i].acc, delta_time_sq_vec, new_pos );// acc * delta_time * delta_time
        debris[i].last_pos = debris[i].packet.pos;
        debris[i].packet.pos = new_pos;
        debris[i].active -= fabs( _length_vec3( spu_sub( debris[i].last_pos, debris[i].packet.pos ) ) );
        if( debris[i].active <= 0.0f )
        {
          ++inactive_count;
	        spu_insert( -5.0f, debris[i].packet.pos, 2);//behind camera
        }

	      ++i;

        new_pos = spu_msub( debris[i].packet.pos, timesTwo, debris[i].last_pos );// temp_pos = pos*2 - last_pos +
        new_pos = spu_madd( debris[i].acc, delta_time_sq_vec, new_pos );// acc * delta_time * delta_time
        debris[i].last_pos = debris[i].packet.pos;
        debris[i].packet.pos = new_pos;
        debris[i].active -= fabs( _length_vec3( spu_sub( debris[i].last_pos, debris[i].packet.pos ) ) );
        if( debris[i].active <= 0.0f )
        {
          ++inactive_count;
	        spu_insert( -5.0f, debris[i].packet.pos, 2);//behind camera
        }

	      ++i;

        new_pos = spu_msub( debris[i].packet.pos, timesTwo, debris[i].last_pos );// temp_pos = pos*2 - last_pos +
        new_pos = spu_madd( debris[i].acc, delta_time_sq_vec, new_pos );// acc * delta_time * delta_time
        debris[i].last_pos = debris[i].packet.pos;
        debris[i].packet.pos = new_pos;
        debris[i].active -= fabs( _length_vec3( spu_sub( debris[i].last_pos, debris[i].packet.pos ) ) );
        if( debris[i].active <= 0.0f )
        {
          ++inactive_count;
	        spu_insert( -5.0f, debris[i].packet.pos, 2);//behind camera
        }

	      ++i;

        new_pos = spu_msub( debris[i].packet.pos, timesTwo, debris[i].last_pos );// temp_pos = pos*2 - last_pos +
        new_pos = spu_madd( debris[i].acc, delta_time_sq_vec, new_pos );// acc * delta_time * delta_time
        debris[i].last_pos = debris[i].packet.pos;
        debris[i].packet.pos = new_pos;
        debris[i].active -= fabs( _length_vec3( spu_sub( debris[i].last_pos, debris[i].packet.pos ) ) );
        if( debris[i].active <= 0.0f )
        {
          ++inactive_count;
	        spu_insert( -5.0f, debris[i].packet.pos, 2);//behind camera
        }

	      ++i;

        new_pos = spu_msub( debris[i].packet.pos, timesTwo, debris[i].last_pos );// temp_pos = pos*2 - last_pos +
        new_pos = spu_madd( debris[i].acc, delta_time_sq_vec, new_pos );// acc * delta_time * delta_time
        debris[i].last_pos = debris[i].packet.pos;
        debris[i].packet.pos = new_pos;
        debris[i].active -= fabs( _length_vec3( spu_sub( debris[i].last_pos, debris[i].packet.pos ) ) );
        if( debris[i].active <= 0.0f )
        {
          ++inactive_count;
	        spu_insert( -5.0f, debris[i].packet.pos, 2);//behind camera
        }

    	  ++i;

        new_pos = spu_msub( debris[i].packet.pos, timesTwo, debris[i].last_pos );// temp_pos = pos*2 - last_pos +
        new_pos = spu_madd( debris[i].acc, delta_time_sq_vec, new_pos );// acc * delta_time * delta_time
        debris[i].last_pos = debris[i].packet.pos;
        debris[i].packet.pos = new_pos;
        debris[i].active -= fabs( _length_vec3( spu_sub( debris[i].last_pos, debris[i].packet.pos ) ) );
        if( debris[i].active <= 0.0f )
        {
          ++inactive_count;
	        spu_insert( -5.0f, debris[i].packet.pos, 2);//behind camera
        }

	      ++i;

        new_pos = spu_msub( debris[i].packet.pos, timesTwo, debris[i].last_pos );// temp_pos = pos*2 - last_pos +
        new_pos = spu_madd( debris[i].acc, delta_time_sq_vec, new_pos );// acc * delta_time * delta_time
        debris[i].last_pos = debris[i].packet.pos;
        debris[i].packet.pos = new_pos;
        debris[i].active -= fabs( _length_vec3( spu_sub( debris[i].last_pos, debris[i].packet.pos ) ) );
        if( debris[i].active <= 0.0f )
        {
          ++inactive_count;
        	spu_insert( -5.0f, debris[i].packet.pos, 2);//behind camera
        }

	      ++i;
      }

      if(inactive_count > 35 )
      {
        spu_insert( 0.0f, buffer[buf][n].origin, 3);
		    recycle.push( ((eal - ealow) / sizeof(Exp)) +  n);
      }
    }
  ++n;
  }
}
