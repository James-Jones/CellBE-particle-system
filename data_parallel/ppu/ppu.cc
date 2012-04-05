
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

extern spe_program_handle_t SPU;
extern spe_program_handle_t SPUb;

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
struct SPEData
{
	spe_program_handle_t* image;
  spe_spu_control_area_t* ctrl;//memory mapped access to registers
  spe_sig_notify_1_area_t* sig1;
  spe_context_ptr_t context;
  spe_stop_info_t stop_info;
  pthread_t posix_thread;
  //buffer switch
  unsigned int switch_buffer __attribute__ ((aligned(4)));
  
  unsigned int exp_last_index __attribute__ ((aligned(4)));

  unsigned int rain_last_index __attribute__ ((aligned(4)));

	//---------- PARTICLE DATA - TWO BUFFERS EACH LIKE FRONT AND BACK FRAME BUFFERS -----------------
	
  Exp exp1 [NEXP / MAX_SPUS_USED] __attribute__ ((aligned(128)));
  Exp exp2 [NEXP / MAX_SPUS_USED] __attribute__ ((aligned(128)));

  Particle droplets1[NDROPLETS / MAX_SPUS_USED] __attribute__ ((aligned(128)));
  Particle droplets2[NDROPLETS / MAX_SPUS_USED] __attribute__ ((aligned(128)));
  
} SPE[MAX_SPUS_USED];

SPUArgs thread_args[2] __attribute__ ((aligned(128)));
int current_buffer;
int nspus;
ppu_64bit_timer timer;
int thread_count;
int end_program;

//------- Close program on ctrl-c command ---------
void sig_handle(int signal)
{
  end_program = 1;
}

/*--------- Begin Function declarations ---------------------- */

void start_thread(int idx);
void* kernel_thread(void* args);
void set_args(int);
void schedule_tasks(unsigned int t, int n);
void start_thread(int idx);
void sync_mbox( int idx );
void handle_stop( int idx );//Callback Function
bool is_exp_active( int spu, int idx );

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
  
	SPE[0].image =&SPU;
  SPE[1].image = &SPUb;
  
  ppu_64bit_timer::read_timebase_value();
  
  printf("timebase: %d", ppu_64bit_timer::get_timebase());

  start_thread(0);
  start_thread(1);

  while(thread_count != nspus);

//------------ Enter Main Loop ---------------------------

  sync_mbox( 0 );
  sync_mbox( 1 );

  schedule_tasks(RAIN, 500);
  schedule_tasks(EXPLOSION, 0);
  
  sync_mbox( 0 );
  sync_mbox( 1 );

  while(end_program == 0)
  {

    server->write_float(timer.get_seconds());
    if(nspus != thread_count)
    {
      handle_stop( thread_count );
      nspus = thread_count;
	  }

	  int count1 = 0;;
    for(int i=0; i<(SPE[0].exp_last_index); ++i)
		  if(is_exp_active(0, i))
			 ++count1;

    int count2 = 0;
    for(int i=0; i<(SPE[1].exp_last_index); ++i)
		  if(is_exp_active(1, i))
			 ++count2;
			 
	  server->write_int(count1 + count2);
	 
	  while (count1)
	  {
      if(current_buffer == 0)
      {
        if(is_exp_active(0, count1))
	      {
          server->write_packet_batch(&SPE[0].exp1[count1].debris[0]);
          server->write_packet_batch(&SPE[0].exp1[count1].debris[10]);
          server->write_packet_batch(&SPE[0].exp1[count1].debris[20]);
          server->write_packet_batch(&SPE[0].exp1[count1].debris[30]);
		      --count1;
        }
      }
      else
      {
	      if(is_exp_active(0, count1))
	      {
           server->write_packet_batch(&SPE[0].exp2[count1].debris[0]);
           server->write_packet_batch(&SPE[0].exp2[count1].debris[10]);
           server->write_packet_batch(&SPE[0].exp2[count1].debris[20]);
           server->write_packet_batch(&SPE[0].exp2[count1].debris[30]);
		       --count1;
       	}
      }
    }
    
	  while (count2)
	  {
      if(current_buffer == 0)
      {
        if(is_exp_active(1, count2))
	      {
          server->write_packet_batch(&SPE[1].exp1[count2].debris[0]);
          server->write_packet_batch(&SPE[1].exp1[count2].debris[10]);
          server->write_packet_batch(&SPE[1].exp1[count2].debris[20]);
          server->write_packet_batch(&SPE[1].exp1[count2].debris[30]);
		      --count2;
        }
      }
      else
      {
	      if(is_exp_active(1, count2))
	      {
           server->write_packet_batch(&SPE[1].exp2[count2].debris[0]);
           server->write_packet_batch(&SPE[1].exp2[count2].debris[10]);
           server->write_packet_batch(&SPE[1].exp2[count2].debris[20]);
           server->write_packet_batch(&SPE[1].exp2[count2].debris[30]);
		       --count2;
       	}
      }
    }

    server->write_int( (SPE[0].rain_last_index + SPE[1].rain_last_index) / 10);

    count1 = SPE[0].rain_last_index;
    count2 = SPE[1].rain_last_index;

    while (count1)
    {

      if(current_buffer == 0)
      {
		    server->write_packet_batch(&SPE[0].droplets1[count1]);
		    count1 -= 10;
      }
      else
      {
		    server->write_packet_batch(&SPE[0].droplets2[count1]);
		    count1 -= 10;
      }
    }
    while (count2)
    {

      if(current_buffer == 0)
      {
		    server->write_packet_batch(&SPE[1].droplets1[count2]);
		    count2 -= 10;
      }
      else
      {
		    server->write_packet_batch(&SPE[1].droplets2[count2]);
		    count2 -= 10;
      }
    }
    
    (current_buffer == 0) ? current_buffer = 1 : current_buffer = 0;
    //tell spu to swap buffers.
    SPE[0].switch_buffer = 1;
    SPE[1].switch_buffer = 1;
    //spu finishes current update then swaps and acks ppu
    while(SPE[0].switch_buffer == 1);
    while(SPE[1].switch_buffer == 1);
  
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

  _spe_sig_notify_1_write(SPE[0].sig1, STOP_SIGNAL);
  _spe_sig_notify_1_write(SPE[1].sig1, STOP_SIGNAL);

  while( thread_count != 0 );

  pthread_join(SPE[0].posix_thread, NULL);
  spe_context_destroy( SPE[0].context );
  spe_image_close(SPE[0].image);
  
  pthread_join(SPE[1].posix_thread, NULL);
  spe_context_destroy( SPE[1].context );
  spe_image_close(SPE[1].image);
  
  server->shutdown();
  
  return 0;
}

void start_thread(int idx)
{
  init_control_block(idx);

  printf("starting thread\n");

  SPE[idx].context = spe_context_create(SPE_MAP_PS, NULL);

	if(SPE[idx].context == NULL)
	printf("failed context errno = %d", errno);

	SPE[idx].ctrl = (spe_spu_control_area_t*) spe_ps_area_get( SPE[idx].context, SPE_CONTROL_AREA );
	SPE[idx].sig1 = (spe_sig_notify_1_area_t*) spe_ps_area_get( SPE[idx].context, SPE_SIG_NOTIFY_1_AREA );

  if( spe_program_load(SPE[0].context, SPE[0].image) == -1)
    printf("failed prog load, errno = %d\n", errno);

  int id[1] = {idx};
  pthread_create( &SPE[idx].posix_thread, NULL, kernel_thread, &id );
}

void sync_mbox( int idx )
{
  printf("sync mbox\n");
  unsigned int message;
  message = _spe_out_mbox_read(SPE[idx].ctrl);//will block
  if( message != SPU_READY)
    printf("SPU_READY expected, %d found instead.", message);

  _spe_in_mbox_write(SPE[idx].ctrl, SPU_GO);
}

//Callback Function
void handle_stop( int idx )
{
  printf("SPU %d has stopped/n", idx);
}

inline bool is_exp_active( int spu, int idx )
{
	net_vecf v;
	if(current_buffer == 0)
	 v.vec = SPE[spu].exp1[idx].origin;
  else
    v.vec = SPE[spu].exp2[idx].origin;

  if(v.scalar[4] >= 0.0f)
	 return true;
  return false;
}

void* kernel_thread(void* args)
{
	int* speid = (int*) args;

	unsigned int entry = SPE_DEFAULT_ENTRY;

	SPUArgs* init = &thread_args[speid[0]];
	++thread_count;
  if( spe_context_run(SPE[speid[0]].context, &entry, 0, init, NULL, &SPE[speid[0]].stop_info) == -1)
  	printf("failed to run context, errno = %d\n", errno);

  pthread_exit(NULL);
}


void set_args(int spu)
{

  thread_args[spu].rain_1_ea.p = &SPE[spu].droplets1;//send output to this address
  thread_args[spu].rain_2_ea.p = &SPE[spu].droplets2;
  
  thread_args[spu].rain_array_length = NDROPLETS / MAX_SPUS_USED;
  
  thread_args[spu].exp_1_ea.p = &SPE[spu].exp1;//send output to this address
  thread_args[spu].exp_2_ea.p = &SPE[spu].exp2;
  
  thread_args[spu].exp_array_length = NEXP / MAX_SPUS_USED;
  

  thread_args[spu].switch_buffer_a = (atomic_ea_t)(&SPE[spu].switch_buffer);

  thread_args[spu].elast_index_a = (atomic_ea_t)(&SPE[spu].exp_last_index);
  
  thread_args[spu].rlast_index_a = (atomic_ea_t)(&SPE[spu].exp_last_index);

  thread_args[spu].timebase = ppu_64bit_timer::get_timebase();
  
  thread_args->x_percent = 10;
  thread_args->z_percent = 10;

  if( spu == 0)
  {
    //first half of space. divded by depth
   thread_args->upper_left_near = _load_vec_float4(-WORLD_WIDTH/2, +WORLD_HEIGHT/2, WORLD_ORIGIN, 1.0f);
   thread_args->lower_right_far = _load_vec_float4(+WORLD_WIDTH/2, -WORLD_HEIGHT/2, WORLD_ORIGIN + (WORLD_DEPTH * 0.5), 1.0f);
  }
  else
  //spu = 1
  {
    //second half of space
   thread_args->upper_left_near = _load_vec_float4(-WORLD_WIDTH/2, +WORLD_HEIGHT/2, WORLD_ORIGIN + (WORLD_DEPTH * 0.5), 1.0f);
   thread_args->lower_right_far = _load_vec_float4(+WORLD_WIDTH/2, -WORLD_HEIGHT/2, WORLD_ORIGIN + WORLD_DEPTH, 1.0f);
  }
}

void schedule_tasks(unsigned int t, int n)
{
  int spu = 0;

  if( t == RAIN)
  {
    if( _atomic_read(SPE[0].rain_last_index) > _atomic_read(SPE[1].rain_last_index) );//last index used as workload idicator
      spu = 1;
  }
  else
  if( t == EXPLOSION)
  {
    if( _atomic_read(SPE[0].exp_last_index) > _atomic_read(SPE[1].exp_last_index) );//last index used as workload idicator
      spu = 1;
  }

  msg_t message;
  message.us[0] = t;
  message.us[1] = n;

  __eieio();//si_sync()
  _spe_in_mbox_write(SPE[spu].ctrl, message.ui);
}
