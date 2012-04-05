#ifndef EXP_H
#define EXP_H

#include "Effect.h"
#include <cassert>

/* Insertions and Deletions from the front */
class FIFO
{
  private:

  class Node
  {
    public:
    int data;
    Node* next;
  };

  Node* head;
  unsigned int size;
  
  public:

  Stack(){size = 0;}

  unsigned int get_size(){return size;}

  void push(int a)
  {

    if(size == 0)
    {
      head = new Node;
      head->data=a;
      head->next = NULL;
    }
    else
    {
      Node* n = new Node;
      n->data = a;
      n->next = head;
      head = n;
    }

    ++size;
  }
  int pop()
  {
    assert(size > 0);

    int returnVal;

    if(size == 1)
    {
      returnVal = head->data;
      delete head;
      head = NULL;
    }
    else
    {
      Node* n = head;
      head = head->next;
      returnVal = n->data;
      delete n;
      n=NULL;
    }

    --size;

    return returnVal;
  }
};

class ExplosionManager : public Effect
{
protected:

  int buffer_size;
  int per_buffer;
  FIFO recycle;

  volatile Exp buffer[4][ 5 ] __attribute__ (( aligned(128) ));

  vector float mean_colour;
  vector float variance_colour;
  vector float speed;

  void update(unsigned int buf, unsigned int nExp);

public:
  ExplosionManager():Effect(){}
  void initialize();
  void process_tasks();
  void validate_buffer_size()
  {
	 assert( buffer_size <= 16 * 1024 );
	 assert( buffer_size % 16 == 0 );
  }
};

#endif
