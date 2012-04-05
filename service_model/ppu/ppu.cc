/*

PROJECT: MULTIPROCESSING DEMONSTRATION USING THE IBM SIMULATOR FOR THE CBE
AUTHOR: JAMES JONES
PROGRAM: 1
VERSION: -

*/
//prevent including libspe1.4
#define _libspe_h_ 0

#include "server.h"
#include <libvector.h>
#include <libspe2.h>
#include "../timer.h"

#include <cbe_mfc.h> // for problem state functions

#include <pthread.h>
#include <sys/time.h>
#include <load_vec_float4.h>
#include <iostream>
#include <signal.h>
#include <errno.h>

#include <atomic_read.h>
#include <atomic_set.h>

extern spe_program_handle_t rSPU;
extern spe_program_handle_t eSPU;

const float WORLD_WIDTH =100.0f;
const float WORLD_HEIGHT =100.0f;
const float WORLD_DEPTH =100.0f;
const float WORLD_ORIGIN =0.0f;

#define MAX_SPUS_USED 2
#define NPARTICLES (NEXP + NDROPLETS)

/*
Holds data on the spe ids
local store address and some problems state addresses
*/
class SPEExp
{
  public:
	spe_program_handle_t* image;
  spe_spu_control_area_t* ctrl;//memory mapped access to registers
  spe_sig_notify_1_area_t* sig1;
  spe_context_ptr_t context;
  spe_stop_info_t stop_info;
  pthread_t posix_thread;

  unsigned int switch_buffer __attribute__ ((aligned(4)));
  
  unsigned int exp_last_index __attribute__ ((aligned(4)));

	//---------- PARTICLE DATA - TWO BUFFERS EACH LIKE FRONT AND BACK FRAME BUFFERS -----------------
	
  Exp exp1 [NEXP / MAX_SPUS_USED] __attribute__ ((aligned(128)));
  Exp exp2 [NEXP / MAX_SPUS_USED] __attribute__ ((aligned(128)));
  
  SPUArgsExp args __attribute__ ((aligned(16)));;

};
SPEExp gSPEExp;

class SPERain
{
  public:
	spe_program_handle_t* image;
  spe_spu_control_area_t* ctrl;//memory mapped access to registers
  spe_sig_notify_1_area_t* sig1;
  spe_context_ptr_t context;
  spe_stop_info_t stop_info;
  pthread_t posix_thread;

  unsigned int switch_buffer __attribute__ ((aligned(4)));

  unsigned int rain_last_index __attribute__ ((aligned(4)));

	//---------- PARTICLE DATA - TWO BUFFERS EACH LIKE FRONT AND BACK FRAME BUFFERS -----------------

  Particle droplets1[NDROPLETS / MAX_SPUS_USED] __attribute__ ((aligned(128)));
  Particle droplets2[NDROPLETS / MAX_SPUS_USED] __attribute__ ((aligned(128)));
  
  SPUArgsRain args __attribute__ ((aligned(16)));;

};
SPERain gSPERain;

int current_buffer;
int nspus;
ppu_64bit_timer timer;
int thread_count;
int end_program;

const unsigned int RAIN_THREAD = 0;
const unsigned int EXP_THREAD = 1;

//------- Close program on ctrl-c command ---------
void sig_handle(int signal)
{
  end_program = 1;
}

/*--------- Begin Function declarations ---------------------- */

void start_thread(int thread);
void* kernel_thread(void* args);
void set_args(int thread);
void schedule_tasks(unsigned int type, int amount);
void start_service(int thread);
void stop_service(int thread);
void sync_mbox( int thread );
void handle_stop( int thread );//Callback Function
bool is_exp_active( int idx );

/*--------- End Function declarations ---------------------- */

int main ()
{
//  Init Vars

  timer.reset();


  end_program = 0;
  thread_count=0;
  nspus = MAX_SPUS_USED;
  current_buffer = 0;

//----------- Connect T0 OpenGL Renderer On Host -----------------

  Server* server = new Server();//Server::get_instance();

  server->set_client_id(CLIENT_ID);
  server->set_port(PORT);
 
  printf("creating socket\n");
  server->create();
  
  printf("binding to port\n");
  server->bind_to_port();
  
  printf("listening for client\n");
  server->listen_for_client();

  printf("done listening\n");
  
	gSPERain.image =&rSPU;
  gSPEExp.image = &eSPU;
  
  ppu_64bit_timer::read_timebase_value();
  
  printf("timebase: %d", ppu_64bit_timer::get_timebase());

  start_thread(RAIN_THREAD);
  start_thread(EXP_THREAD);

  while(thread_count != nspus);

//------------ Enter Main Loop ---------------------------

  sync_mbox( RAIN_THREAD );
  sync_mbox( EXP_THREAD );

  schedule_tasks(RAIN, 500);
  schedule_tasks(EXPLOSION, 0);

  sync_mbox( RAIN_THREAD );
  sync_mbox( EXP_THREAD );

  while(end_program == 0)
  {

    server->write_float(timer.get_seconds());
    if(nspus != thread_count)
    {
      handle_stop( thread_count );
      nspus = thread_count;
	  }

	  int count1 = 0;;
    for(int i=0; i<(gSPEExp.exp_last_index); ++i)
		  if(is_exp_active(i))
			 ++count1;

	  server->write_int(count1*4);
	 
	  while (count1)
	  {
      if(current_buffer == 0)
      {
        if(is_exp_active(count1))
	      {
          server->write_packet_batch(&gSPEExp.exp1[count1].debris[0]);
          server->write_packet_batch(&gSPEExp.exp1[count1].debris[10]);
          server->write_packet_batch(&gSPEExp.exp1[count1].debris[20]);
          server->write_packet_batch(&gSPEExp.exp1[count1].debris[30]);
		      --count1;
        }
      }
      else
      {
	      if(is_exp_active(count1))
	      {
           server->write_packet_batch(&gSPEExp.exp2[count1].debris[0]);
           server->write_packet_batch(&gSPEExp.exp2[count1].debris[10]);
           server->write_packet_batch(&gSPEExp.exp2[count1].debris[20]);
           server->write_packet_batch(&gSPEExp.exp2[count1].debris[30]);
		       --count1;
       	}
      }
    }

    server->write_int( _atomic_read(gSPERain.args.rlast_index_a) / 10);

    count1 = gSPERain.rain_last_index;

    while (count1)
    {

      if(current_buffer == 0)
      {
		    server->write_packet_batch(&gSPERain.droplets1[count1]);
		    count1 -= 10;
      }
      else
      {
		    server->write_packet_batch(&gSPERain.droplets2[count1]);
		    count1 -= 10;
      }
    }

    (current_buffer == 0) ? current_buffer = 1 : current_buffer = 0;
    //tell spu to swap buffers.
    _atomic_set( gSPERain.args.switch_buffer_a, 1);
    _atomic_set( gSPEExp.args.switch_buffer_a, 1);
    //spu finishes current update then swaps and acks ppu
    while(_atomic_read(gSPERain.args.switch_buffer_a) == 1);
    while(_atomic_read(gSPEExp.args.switch_buffer_a) == 1);
  
    char cmd = server->read_char();
    if(cmd == 'e')
      schedule_tasks(EXPLOSION, 1);
    if(cmd == 'r')
      schedule_tasks(RAIN, 100);
    if(cmd == 's')
      schedule_tasks(RAIN, -1);
  
  }
  
  printf("Program Closing ...\n");
//----------- End Of Main Loop - Shutdown and free thread resources ----------

  stop_service( RAIN_THREAD );
  stop_service( EXP_THREAD );
  
  server->shutdown();
  
  return 0;
}

void start_thread(int thread)
{
  set_args(thread);

  printf("starting thread\n");
  
  switch( thread )
  {
    case EXP_THREAD:
      {
        gSPEExp.context = spe_context_create(SPE_MAP_PS, NULL);
	      if(gSPEExp.context == NULL)
          printf("failed context errno = %d", errno);
	      gSPEExp.ctrl = (spe_spu_control_area_t*) spe_ps_area_get( gSPEExp.context, SPE_CONTROL_AREA );
	      gSPEExp.sig1 = (spe_sig_notify_1_area_t*) spe_ps_area_get( gSPEExp.context, SPE_SIG_NOTIFY_1_AREA );
        if( spe_program_load(gSPEExp.context, gSPEExp.image) == -1)
          printf("failed prog load, errno = %d\n", errno);
        int id[1] = {thread};
        pthread_create( &gSPEExp.posix_thread, NULL, kernel_thread, &id );
    }
    break;
  case RAIN_THREAD:
    {
      gSPERain.context = spe_context_create(SPE_MAP_PS, NULL);
	    if(gSPERain.context == NULL)
        printf("failed context errno = %d", errno);
	    gSPERain.ctrl = (spe_spu_control_area_t*) spe_ps_area_get( gSPERain.context, SPE_CONTROL_AREA );
	    gSPERain.sig1 = (spe_sig_notify_1_area_t*) spe_ps_area_get( gSPERain.context, SPE_SIG_NOTIFY_1_AREA );
      if( spe_program_load(gSPERain.context, gSPERain.image) == -1)
        printf("failed prog load, errno = %d\n", errno);
      int id[1] = {thread};
      pthread_create( &gSPERain.posix_thread, NULL, kernel_thread, &id );
    }
    break;
  }
}

void sync_mbox( int thread )
{
  printf("sync mbox\n");
  unsigned int message;
  
  switch( thread )
{
  case EXP_THREAD:
    {
      message = _spe_out_mbox_read(gSPEExp.ctrl);//will block
      if( message != SPU_READY)
        printf("SPU_READY expected, %d found instead.", message);
      _spe_in_mbox_write(gSPEExp.ctrl, SPU_GO);
    }
  break;
  case RAIN_THREAD:
    {
      message = _spe_out_mbox_read(gSPERain.ctrl);//will block
      if( message != SPU_READY)
        printf("SPU_READY expected, %d found instead.", message);
      _spe_in_mbox_write(gSPERain.ctrl, SPU_GO);
    }
    break;
}
}

//Callback Function
void handle_stop( int thread )
{
  printf("SPU %d has stopped/n", thread);
}

bool is_exp_active( int idx )
{
	net_vecf v;
	if(current_buffer == 0)
	 v.vec = gSPEExp.exp1[idx].origin;
  else
    v.vec = gSPEExp.exp2[idx].origin;

  if(v.scalar[4] >= 0.0f)
	 return true;
  return false;
}

void* kernel_thread(void* args)
{
	int* speid = (int*) args;

	unsigned int entry = SPE_DEFAULT_ENTRY;
	
	++thread_count;
	
	if( speid[0] == EXP_THREAD)
	{
    if( spe_context_run(gSPEExp.context, &entry, 0, &gSPEExp.args, NULL, &gSPEExp.stop_info) == -1)
  	 printf("failed to run context, errno = %d\n", errno);
  }
  else
  {
    if( spe_context_run(gSPERain.context, &entry, 0, &gSPERain.args, NULL, &gSPERain.stop_info) == -1)
  	 printf("failed to run context, errno = %d\n", errno);
  }

  pthread_exit(NULL);
}


void set_args(int spu)
{
  switch( spu )
  {
    case 0:
      {
        gSPERain.args.rain_1_ea.p = &gSPERain.droplets1;//send output to this address
        gSPERain.args.rain_2_ea.p = &gSPERain.droplets2;
        gSPERain.args.array_length = NDROPLETS;
        gSPERain.args.switch_buffer_a = (atomic_ea_t)(&gSPERain.switch_buffer);
        gSPERain.args.rlast_index_a = (atomic_ea_t)(&gSPERain.rain_last_index);
        gSPERain.args.timebase = ppu_64bit_timer::get_timebase();
        gSPERain.args.x_percent = 10;
        gSPERain.args.z_percent = 10;
        gSPERain.args.upper_left_near = _load_vec_float4(-WORLD_WIDTH/2, +WORLD_HEIGHT/2, WORLD_ORIGIN, 1.0f);
        gSPERain.args.lower_right_far = _load_vec_float4(+WORLD_WIDTH/2, -WORLD_HEIGHT/2, WORLD_ORIGIN + WORLD_DEPTH, 1.0f);
      }
      break;
    case 1:
      {
        gSPEExp.args.exp_1_ea.p = gSPEExp.exp1;
        gSPEExp.args.exp_2_ea.p = gSPEExp.exp2;
        gSPEExp.args.array_length = NEXP;
        gSPEExp.args.switch_buffer_a = (atomic_ea_t)(&gSPEExp.switch_buffer);
	gSPEExp.args.elast_index_a = (atomic_ea_t)(&gSPEExp.exp_last_index);
        gSPEExp.args.timebase = ppu_64bit_timer::get_timebase();
      }
      break;
   }
}

void stop_service(int idx)
{
  int tc = thread_count;
  switch( idx )
  {
    case 0:
      {
        _spe_sig_notify_1_write(gSPEExp.sig1, STOP_SIGNAL);
        while( thread_count != tc-1 );
        pthread_join(gSPEExp.posix_thread, NULL);
        spe_context_destroy( gSPEExp.context );
        spe_image_close(gSPEExp.image);
      }
      break;
    case 1:
      {
        _spe_sig_notify_1_write(gSPERain.sig1, STOP_SIGNAL);
        while( thread_count != tc-1 );
        pthread_join(gSPERain.posix_thread, NULL);
        spe_context_destroy( gSPERain.context );
        spe_image_close(gSPERain.image);
      }
      break;
  }
}

void schedule_tasks(unsigned int type, int amount)
{
  msg_t message;
  message.us[0] = NEW_TASK;
  message.us[1] = amount;
  
  switch( type )
  {
  case EXPLOSION:
    {
      _spe_in_mbox_write(gSPEExp.ctrl, message.ui);
    }
    break;
  case RAIN:
    {
      _spe_in_mbox_write(gSPERain.ctrl, message.ui);
    }
    break;
  }
}
