
#include "../timer.h"
#include <stdio.h>
#include "libsim.h"
#include <unistd.h>
#include <stdlib.h>
#include "rain.h"

volatile SPUArgsRain control_block __attribute__((aligned(128)));

extern int _etext;
extern int _edata;
extern int _end;

spu_32bit_timer timer;

int current_buffer;

unsigned int eah;
unsigned int eal;

RainManager* rain;

//--------FUNCTION DEFINITIONS----------
int main (unsigned long long speid, unsigned long long eaAddr, unsigned long long env __attribute__ ((unused)) )
{

	printf("spu: code size is %p", &_etext);
	printf("spu: address after stack is %p", &_edata);
	printf("spu: address after heap is %p", &_end);
	printf("spu: sbrk(0)= %p", sbrk(0));

	if( sizeof(Exp) % 16 != 0)
	{
		printf("BAD EFFECT SIZE");
		return -1;
	}

	unsigned int tag_id = 31;//0-31
	unsigned int tag_mask = (1<<( tag_id & 0x1F));//1FH = 31 = 2^5-1

	//check that the command queue is not full
	while( spu_readchcnt(MFC_Cmd) == 0);

	spu_mfcdma64(&control_block, mfc_ea2h(eaAddr), mfc_ea2l(eaAddr), sizeof(SPUArgsRain), tag_id, MFC_GET_CMD);
	
	spu_writech(MFC_WrTagMask, tag_mask);
	spu_mfcstat(2);

  atomic_set(control_block.rlast_index_a, 0);

	current_buffer = 0;

	bool end_program = false;

  addr64_t a;
  a.ull = control_block.rain_1_ea.ull;
  eah = mfc_ea2h(a.ui[0]);
  eal = mfc_ea2l(a.ui[0]);

  rain = new RainManager;
  rain->set_target_size(control_block.array_length);
  rain->set_target(eal, eah);
  rain->initialize( &control_block );
  
  rain->validate_buffer_size();

  spu_writech(SPU_WrOutMbox, SPU_READY);

  unsigned int response = spu_readch(SPU_RdInMbox);
  
  if( response != SPU_GO )
  {
    printf( "spu: response != SPU_GO\n" );
  }
  
  
    timer.start();
    float delta_time = timer.get_seconds();
    Effect::set_dtime2( spu_splats( (float) (delta_time * delta_time) ) );
	  rain->process_tasks();
	  atomic_set(control_block.rlast_index_a, rain->get_updates());
    
  
  spu_writech(SPU_WrOutMbox, SPU_READY);

 response = spu_readch(SPU_RdInMbox);

  if( response != SPU_GO )
  {
    printf( "spu: response != SPU_GO\n" );
  }
  
  delta_time = timer.get_seconds();


	while( !end_program )
	{
    timer.start();
    
    
    Effect::set_dtime2( spu_splats( (float) (delta_time * delta_time) ) );

    //DMA engine can process up to 16 transfers in parallel.

	  rain->process_tasks();

	  atomic_set(control_block.rlast_index_a, rain->get_updates());

    if( atomic_read(control_block.switch_buffer_a) == 1)
    {
      spu_writech(MFC_WrTagMask, tag_mask);
      spu_mfcstat(2);

      atomic_set(control_block.switch_buffer_a, 0);
      
      if (current_buffer == 0)
      {
        current_buffer = 1;

        a.ull = control_block.rain_2_ea.ull;
        eah = mfc_ea2h(a.ui[0]);
        eal = mfc_ea2l(a.ui[0]);
        rain->set_target(eal, eah);
      }
      else
      {
        current_buffer = 0;

        a.ull = control_block.rain_1_ea.ull;
        eah = mfc_ea2h(a.ui[0]);
        eal = mfc_ea2l(a.ui[0]);
        rain->set_target(eal, eah);
      }
    }

	if( spu_readchcnt( SPU_RdInMbox) )
	{
		msg_t val;
		val.ui = spu_readch(SPU_RdInMbox);
		if(val.us[0] == NEW_TASK )
		{
			rain->add_task(val.us[1]);
    }
	}
	
		//avoid branch while checking for STOP signal
		end_program = spu_readchcnt(SPU_RdSigNotify1);

    delta_time = timer.get_seconds();
  
	}
	
	timer.stop();

  return 0;
}
