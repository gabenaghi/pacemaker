#include "common.h"
#include "generator.h"
#include "responder.h"

Thread threads[NUM_THREADS]; //default osPriority Normal

int main()
{

   threads[T_GENERATOR].start(generator_thread);
   threads[T_RESPONDER].start(responder_thread);
   return 0; 
}