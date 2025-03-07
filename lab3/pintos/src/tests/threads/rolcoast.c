/*
  File for 'rolcoast' task implementation.
*/

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "devices/timer.h"

struct semaphore board; /* for carriage waiting */
struct semaphore ride;  /* for passengers waiting */

int passengers;
int needed_passengers;

static void init(unsigned int carriage_size UNUSED)
{
  sema_init(&board, 0);
  sema_init(&ride, 0);

  needed_passengers = carriage_size;
}

static void carriage(void* arg UNUSED)
{
  msg("carriage created.");

  while (1)
    {
      for (int i = 0; i < needed_passengers; i++)         /* waiting for needed quantity of passengers */
        {
          sema_down(&board);
        }

      msg("carriage filled.");
      msg("carriage started.");

      timer_sleep(10);                                    /* ~ time of ride */

      msg("carriage finished.");
      msg("passengers leaving.");

      for (int i = 0; i < needed_passengers; i++)         /* unblock passengers */
        {
          sema_up(&ride);
        }

      for (int i = 0; i < needed_passengers; i++)         /* waiting of leaving each passengers */
        {
          sema_down(&board);
        }

      msg("carriage empty.");
    }
}

static void passenger(void* arg)
{
  msg("passenger %d created.", (int) arg);

  sema_up(&board);                                    /* one from needed quantity boarded */
  msg("passenger %d boarded and wait.", (int) arg);

  sema_down(&ride);                            
  
  
  
  msg("passenger %d left.", (int) arg);

  sema_up(&board);
}

void test_rolcoast(unsigned int num_clients, unsigned int carriage_size, unsigned int interval)
{
  unsigned int i;
  init(carriage_size);

  thread_create("carriage", PRI_DEFAULT, &carriage, NULL);

  for(i = 0; i < num_clients; i++)
  {
    char name[32];
    timer_sleep(interval);
    snprintf(name, sizeof(name), "passenger_%d", i + 1);
    thread_create(name, PRI_DEFAULT, &passenger, (void*) (i+1) );
  }

  timer_msleep(5000);
  pass();    
}
