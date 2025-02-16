#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/malloc.h"
#include "threads/thread.h"
#include "devices/timer.h"

/* Calculating Fibonacci numbers as computing load */
long int fib (int n) {
    if (n <= 1) return n;
    return fib(n - 1) + fib(n - 2);
}

/* Function for computing load for threads */
void 
comp_thread (void *aux)
{
    int n = (int) aux;
    fib(n);
}

/* Function of busy-wait for threads */
void
busy_wait (void *aux)
{
    int n = (int) aux;
    timer_mdelay(n);
}

/* Main test function */
void
test_threads_audit (void) 
{
  for(int i = 0; i < 5; i++)
    {
        char name[16];
        snprintf(name, sizeof(name), "Thread %d comps", i);
        thread_create(name, PRI_DEFAULT, comp_thread, 17 + i);
    }

  for(int i = 0; i < 5; i++)
    {
        char name[16];
        snprintf(name, sizeof(name), "Thread %d waits", 5 + i);
        thread_create(name, PRI_DEFAULT, busy_wait, 3000 - i * 500);
    }

  timer_msleep(5000);

  out_audit_list();
}