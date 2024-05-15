#include "userprog/syscall.h"
#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler(struct intr_frame *);

void syscall_init(void)
{
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler(struct intr_frame *f UNUSED)
{
  int sys_call_type = *((int *) f->esp);
  printf("system call!\n");
  printf("System call type is : %d", sys_call_type);
  switch (sys_call_type)
  {
  case SYS_HALT:
    /* code to handle halt system call */
    break;
  case SYS_EXIT:
    /* code to handle exit system call */
    break;
  case SYS_EXEC:
  {
    // get filename for process_execute
    char *process_name = (char *)(f->esp + 1);
    printf("File to execute is : %s", process_name);
    tid_t thread_id = process_execute(process_name);
    
    break;
  }
  case SYS_WAIT:
    process_wait();
    break;
  case SYS_CREATE:
    /* code to handle create system call */
    break;
  case SYS_REMOVE:
    /* code to handle remove system call */
    break;
  case SYS_OPEN:
    /* code to handle open system call */
    break;
  case SYS_FILESIZE:
    /* code to handle filesize system call */
    break;
  case SYS_READ:
    /* code to handle read system call */
    break;
  case SYS_WRITE:
    /* code to handle write system call */
    break;
  case SYS_SEEK:
    /* code to handle seek system call */
    break;
  case SYS_TELL:
    /* code to handle tell system call */
    break;
  case SYS_CLOSE:
    /* code to handle close system call */
    break;
  default:
    printf("Unknown system call is requested\n");
    break;
  }
  thread_exit();
}
