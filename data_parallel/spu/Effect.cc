#include "Effect.h"

vector float Effect::timesTwo = (vector float) {2.0f,2.0f,2.0f,2.0f};

vector float Effect::delta_time_sq_vec = (vector float) {1.0f, 1.0f, 1.0f, 1.0f};

void Effect::add_task(int n)
{
  if(n == -1)
  {
    updates = 0;
    creates = 0;
  }
  else
  creates += n;
}
