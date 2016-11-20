#include "common.h"
#include "generator.h"

Thread threads[NUM_THREADS]; //default osPriority Normal

int main()
{

   threads[T_GENERATOR].start(generator_thread);
   return 0; 
}