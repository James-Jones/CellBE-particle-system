
#ifndef TIMER_H
#define TIMER_H

#ifdef __SPU__
#include <spu_intrinsics.h>
#include <spu_mfcio.h>

class spu_32bit_timer
{
  protected:
  static unsigned int timebase;// = 79800000;//proc/cpuinfo, Ticks Per Second

  public:

  static void set_timebase(unsigned int tb){timebase=tb;}
  void start()
  {
    spu_writech(SPU_WrDec, 0x0); //using 0 will cause dec event after 1 tick
  }
  float get_seconds()
  {
    return  ( (-spu_readch(SPU_RdDec)) / timebase );
  }
  void stop()
  {
    //To stop a decrementer, ack the decrementer event when the event mask is not set
    unsigned int current_mask = spu_readch(SPU_RdEventMask);//save the mask
    spu_writech(SPU_WrEventMask, 0);
    spu_writech(SPU_WrEventAck, MFC_DECREMENTER_EVENT);
    spu_writech(SPU_WrEventMask, current_mask);//restore the mask
  }
};

unsigned int spu_32bit_timer::timebase = 0;

#else

#include <cstdio>
#include <altivec.h>
#include <ppu_intrinsics.h>
#include <fstream>
/*
  In order for ppu_64bit_timer to work, the hypervisor software must have set the tb_enable bit of the HID6 register
*/

//not stop function since the TB register can only be written to by hypervisor software
class ppu_64bit_timer
{
  protected:
  static unsigned int timebase;// = 79800000;//proc/cpuinfo, Ticks Per Second
  unsigned long long start_value;
  
  /*
  correctly reads 64-bit TB register when ELF compiled as 32-bit
  this function is from a post on the IBM Cell Architecture Forum
  
  mftb = (m)ove (f)rom (t)ime (b)ase register
  */
  unsigned long long mftb32()
  {
    unsigned long long result = 0;
    unsigned long upper, lower, temp;

    __asm__ volatile
    (
      "loop  \n"
      "\tmftbu %0 \n"
      "\tmftb %1 \n"
      "\tmftbu %2 \n"
      "\tcmpw %2,%0 \n"
      "\tbne loop \n"
      :"=r"(upper),"=r"(lower),"=r"(temp)
    );

    result = upper;
    result = result << 32;
    result = result | lower;
    return result;
  }
  
  public:

  static void read_timebase_value()
  {
    char buffer[100];
    std::ifstream file;
    file.open("/proc/cpuinfo");
    while(!file.eof())
    {
      file.getline(buffer, 100);
      if(sscanf(buffer, "timebase : %d", &timebase)==1)
        break;
    }
    file.close();
  }

  static unsigned int get_timebase(){return timebase;}

  void reset()
  {
    #ifdef __powerpc64__
      start_value = __mftb();
    #else
      start_value = mftb32();
    #endif
  }
  float get_seconds()
  {
    #ifdef __powerpc64__
      return ((__mftb() - start_value)/timebase);
    #else
      return ((mftb32() - start_value)/timbase);
    #endif
  }
  
  unsigned long long get_start_value(){ return start_value; }
};

unsigned int ppu_64bit_timer::timebase = 0;

#endif
#endif
