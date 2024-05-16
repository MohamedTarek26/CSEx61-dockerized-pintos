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

void
exit_process(int status){
   
  struct thread* cur = thread_current(); // child
  struct thread* parent = cur->parent_thread; // parent

  // struct child_thread* c = has_child(cur->parent_thread, cur->tid);
  if(parent != NULL){
  if(parent->wait_on == cur->tid){
    parent->child_status = status;
    parent->wait_on = -1;
    sema_up(&parent->wait_child);
  }
  else{
    struct child_thread * ct = has_child(parent,cur->tid);
    if(ct != NULL)
    {
      list_remove(&ct->child_elem);
    }
  }
  }

  cur->status = status;
  sema_up(&cur->parent_child);
  
  thread_exit();
}