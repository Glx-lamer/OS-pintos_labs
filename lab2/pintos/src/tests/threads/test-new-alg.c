#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/init.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "devices/timer.h"
#include "threads/interrupt.h"

int priority_array[6] = {63, 43, 33, 25, 1, 63};
int64_t burst_array[6] = {8, 5, 1, 2, 5, 2};


void
new_alg_func (void *aux UNUSED)
{
    int64_t burst = thread_current()->cpu_burst;
    msg(">>> %s created with priority: %d, burst %d\n", thread_current()->name, thread_current()->priority, burst);
    int64_t ticks = timer_ticks();
    int64_t now = timer_ticks();
    while(now < ticks + burst)
        {
            intr_disable();
            if (timer_ticks() != now) 
                {
                    msg(">>> %s: tick %d\n", thread_current()->name, now - ticks + 1);
                    now = timer_ticks();
                }
            intr_enable();
        }
    msg(">>> %s: completed\n", thread_current()->name);
}

void 
test_new_alg ()
{
    msg(">>> Test main thread priority set to MAX.");
    thread_set_priority(PRI_MAX);
    for(int i = 0; i < 6; i++)
        {
            char name[16];
            snprintf(name, sizeof(name), "Thread %d", i);
            thread_create_burst(name, priority_array[i], burst_array[i], new_alg_func, NULL);
        }
    msg(">>> Test main thread priority set to MIN.");
    thread_set_priority(PRI_MIN);
    thread_yield();
}