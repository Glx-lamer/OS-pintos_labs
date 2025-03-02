#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

#include "devices/shutdown.h"
#include "threads/vaddr.h"
#include "pagedir.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

int
exit_code (int code)
{
  thread_current()->exit = code;
  thread_exit();
  return code;
}

int
check_ptr (void *vaddr)
{
  if (is_user_vaddr(vaddr))
    {
      if (pagedir_get_page(thread_current()->pagedir, vaddr))
        {
          return 0;
        }
    }
  return exit_code(-1);
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  check_ptr(f->esp);

  int SYS_CALL = *(int*)f->esp;
  int* args = (int*)f->esp + 1;

  switch (SYS_CALL) {
    case SYS_HALT:
      shutdown_power_off();
      break;

    case SYS_EXIT:
      exit_code(args[0]);
      break;

    case SYS_EXEC:
      break;

    case SYS_WAIT:
      break;

    case SYS_CREATE:
      break;

    case SYS_REMOVE:
      break;

    case SYS_OPEN:
      break;

    case SYS_FILESIZE:
      break;

    case SYS_READ:
      break;

    case SYS_WRITE:
      putbuf(args[1], args[2]);
      break;

    case SYS_CLOSE:
      break;

    default:
      break;
  }
}
