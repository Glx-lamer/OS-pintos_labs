
/* File for 'narrow_bridge' task implementation.  
   SPbSTU, IBKS, 2017 */

#include <stdio.h>
#include "tests/threads/tests.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include "narrow-bridge.h"

//! New variables 

struct semaphore left_emergency_c;      /* c - in my mind controller */
struct semaphore left_usual_c;
struct semaphore right_emergency_c;
struct semaphore right_usual_c; 

/* First idea - use only sems, but we cant get value of sem,
	so needed create new value-variables for each car type,
	bridge direction quantity */

int64_t left_emergency_q;
int64_t left_usual_q;
int64_t right_emergency_q;
int64_t right_usual_q;	

int64_t bridge_direction_right_q;
int64_t bridge_direction_left_q;


// Called before test. Can initialize some synchronization objects.
void 
narrow_bridge_init(void)
{

	sema_init(&left_emergency_c, 0);
	sema_init(&right_emergency_c, 0);
	sema_init(&left_usual_c, 0);
	sema_init(&right_usual_c, 0);

	left_emergency_q = 0;
	left_usual_q = 0;
	right_emergency_q = 0;
	right_usual_q = 0;	

	bridge_direction_right_q = 0;
	bridge_direction_left_q = 0;

}

bool
car_permition(enum car_direction dir)
{
	if (bridge_direction_left_q + bridge_direction_right_q == 0)      /* if bridge empty*/
		{
			bridge_direction_left_q = (dir == dir_left) ? 1 : 0;
			bridge_direction_right_q = (dir == dir_left) ? 0 : 1;

			return true;
		}
	else if (bridge_direction_left_q == 1 && dir == dir_left)          /* if bridge dir matched with car dir */
		{
			bridge_direction_left_q++;

			return true;
		}
	else if (bridge_direction_right_q == 1 && dir == dir_right)
		{
			bridge_direction_right_q++;

			return true;
		}
	else
		{
			return false;
		}
}

void
sems_control (enum car_priority prio, enum car_direction dir)
{
	if (prio == car_emergency)
		{
			if (dir == dir_left)
				{
					left_emergency_q++;
					sema_down(&left_emergency_c);
				}
			else if (dir == dir_right)
				{
					right_emergency_q++;
					sema_down(&right_emergency_c);
				}
		}

	else if (prio == car_normal)
		{
			if (dir == dir_left)
				{
					left_usual_q++;
					sema_down(&left_usual_c);
				}
			else if (dir == dir_right)
				{
					right_usual_q++;
					sema_down(&right_usual_c);
				}
		}
}

/* Pass logic */

void
emergency_pass (enum car_priority dir)
{
	if (dir == dir_left)
		{
			while (left_emergency_q > 0 && bridge_direction_right_q + bridge_direction_left_q < 2)
				{
                    bridge_direction_left_q++;
					left_emergency_q--;
					sema_up(&left_emergency_c);
				}
		}
	else if (dir == dir_right)
		{
			while (right_emergency_q > 0 && bridge_direction_right_q + bridge_direction_left_q < 2)
				{
                    bridge_direction_right_q++;
					right_emergency_q--;
					sema_up(&right_emergency_c);
				}
		}
}

void
usual_pass (enum car_priority dir)
{
	if (dir == dir_left)
		{
			while (left_usual_q > 0 && bridge_direction_right_q + bridge_direction_left_q < 2)
				{
                    bridge_direction_left_q++;
					left_usual_q--;
					sema_up(&left_usual_c);
				}
		}
	else if (dir == dir_right)
		{
			while (right_usual_q > 0 && bridge_direction_right_q + bridge_direction_left_q < 2)
				{
                    bridge_direction_right_q++;
					right_usual_q--;
					sema_up(&right_usual_c);
				}
		}
}

void
car_pass (enum car_priority dir)
{
	/* emergency goes first from each side, priority for opposit side */
	if (dir == dir_left)
		{
			if (right_emergency_q > 0)
				{
					emergency_pass(dir_right);
					usual_pass(dir_right);
				}
			else if (left_emergency_q > 0)
				{
					emergency_pass(dir_left);
					usual_pass(dir_left);
				}
			else if (right_usual_q > 0)
				{
					usual_pass(dir_right);
				}
			else if (left_usual_q > 0)
				{
					usual_pass(dir_left);
				}
		}
	if (dir == dir_right)
		{
			if (left_emergency_q > 0)
				{
					emergency_pass(dir_left);
					usual_pass(dir_left);
				}
			else if (right_emergency_q > 0)
				{
					emergency_pass(dir_right);
					usual_pass(dir_right);
				}
			else if (left_usual_q > 0)
				{
					usual_pass(dir_left);
				}
			else if (right_usual_q > 0)
				{
					usual_pass(dir_right);
				}
		}
}

void 
arrive_bridge(enum car_priority prio UNUSED, enum car_direction dir UNUSED)
{
	/* if cars on bridge < 3 -> if bridge direction matched with
	   current car direction, else use sems for waiting*/

	bool permit = car_permition(dir);

	if (!permit) sems_control(prio, dir);
}

void exit_bridge(enum car_priority prio UNUSED, enum car_direction dir UNUSED)
{

	enum car_direction prev_direction = (bridge_direction_left_q > 0) ? dir_left : dir_right;
	bridge_direction_left_q = (prev_direction == dir_left) ? bridge_direction_left_q - 1 : bridge_direction_left_q;
	bridge_direction_right_q = (prev_direction == dir_right) ? bridge_direction_right_q - 1 : bridge_direction_right_q;

	if (bridge_direction_left_q + bridge_direction_right_q == 0) car_pass(prev_direction);

}
