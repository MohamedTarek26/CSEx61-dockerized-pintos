#include "userprog/syscall.h"
#include "userprog/process.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/synch.h"
#include <string.h>
#include "../filesys/filesys.h"
#include "../filesys/file.h"
#include "../devices/block.h"
#include "threads/vaddr.h"
static void syscall_handler (struct intr_frame *);

/* lock for files synch*/
struct semaphore file_lock;

void syscall_init(void)
{
  sema_init(&file_lock, 1);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
  printf ("system call!\n");
  // halting a process
  if (*(int*)f->eip == SYS_HALT){
    shutdown_power_off();
  }

  // exiting a process
  else if (*(int*)f->eip == SYS_EXIT){
    check_void_ptr(f->esp + 4);

    int status = *((int*)f->esp + 1);
    sys_exit(status);
  }

  else if (*(int*)f->eip == SYS_EXEC){
    // get filename for process_execute
    char *process_name = (char *)(f->esp + 1);
    printf("File to execute is : %s", process_name);
    tid_t thread_id = process_execute(process_name);
  }
  else if (*(int*)f->eip == SYS_WAIT){
    printf("ALOOOOOOOO wait");
    // get filename for process_execute
    int id = (int *)(f->esp + 1);

    process_wait(id);
    printf("ALOOOOOOOO khlst wait");

  }

  // creating a file
  else if (*(int*)f->eip == SYS_CREATE){
    check_void_ptr(f->esp + 4);
    check_void_ptr(f->esp + 8);
    char* name = (char*)(*((int*)f->esp + 1));
    if (name == NULL) sys_exit(-1); 

    size_t initial_size = *((int*)f->esp + 2);
    sema_down(&file_lock);

    f->eax = filesys_create(name, initial_size);

    sema_up(&file_lock);
  }
  // removing a file
  else if (*(int*)f->eip == SYS_REMOVE){
    check_void_ptr(f->esp + 4);

    char* name = (char*)(*((int*)f->esp + 1));
    if (name == NULL) sys_exit(-1);

    sema_down(&file_lock);

    f->eax = filesys_remove(name);

    sema_up(&file_lock);
  }

  // opening a file
  else if (*(int*)f->eip == SYS_OPEN){
    check_void_ptr(f->esp + 4);

    char* name = (char*)(*((int*)f->esp + 1));

    if (name == NULL) sys_exit(-1);

    struct open_file* open = palloc_get_page(0);

    if (open == NULL) 
    {
      palloc_free_page(open);
      f->eax = -1;
      return;
    }

    sema_down(&file_lock);
    open->ptr = filesys_open(name);
    sema_up(&file_lock);

    if (open->ptr == NULL)
    {
      f->eax = -1;
      return;
    }
    open->fd = ++thread_current()->fd_last;
    list_push_back(&thread_current()->open_file_list,&open->elem);
    f->eax = open->fd;
  }

  // getting file size
  else if (*(int*)f->eip == SYS_FILESIZE){
    check_void_ptr(f->esp + 4);
    struct file* my_file = get_file(*((int*)f->esp + 1))->ptr;

    // checking if the file exists
    if (my_file == NULL){
      f->eax = -1;
      return;
    }

    sema_down(&file_lock);
    f->eax = file_length(my_file);
    sema_up(&file_lock);
  }

  // reading from a file
  else if (*(int*)f->eip == SYS_READ){
    check_void_ptr(f->esp + 4);
    check_void_ptr(f->esp + 8);
    check_void_ptr(f->esp + 12);

    int fd, size;
    void* buffer;
    fd = *((int*)f->esp + 1);
    buffer = (void*)(*((int*)f->esp + 2));
    size = *((int*)f->esp + 3);

    check_void_ptr(buffer + size);
    if (fd == 0)
    {
      for (size_t i = 0; i < size; i++)
      {
        sema_down(&file_lock);
        ((char*)buffer)[i] = input_getc();
        sema_up(&file_lock);
      }
      f->eax = size;
      return;
    }else{
      struct file* my_file = get_file(fd)->ptr;

      if (my_file == NULL)
      {
        f->eax = -1;
        return;
      }
      sema_down(&file_lock);
      f->eax = file_read(my_file,buffer,size);
      sema_up(&file_lock);
    }
    
  }

  // writing in a file
  else if (*(int*)f->eip == SYS_WRITE){
    check_void_ptr(f->esp+4);
    check_void_ptr(f->esp+8);
    check_void_ptr(f->esp+12);

    int fd, size;
    void* buffer;
    fd = *((int*)f->esp + 1);
    buffer = (void*)(*((int*)f->esp + 2));
    size = *((int*)f->esp + 3);

    if (buffer == NULL) sys_exit(-1);

    if (fd == 1)
    { 
      sema_down(&file_lock);
      putbuf(buffer,size);
      sema_up(&file_lock);
      f->eax = size;
      return;
    }else{
      struct file* my_file = get_file(fd)->ptr;

      if (my_file == NULL)
      {
        f->eax = -1;
        return;
      }
      sema_down(&file_lock);
      f->eax = file_write(my_file,buffer,size);
      sema_up(&file_lock);
    }
  }

  // seeking the next byte in a file
  else if (*(int*)f->eip == SYS_SEEK){
    check_void_ptr(f->esp + 4);
    check_void_ptr(f->esp + 8);

    int fd;
    unsigned pos;
    fd = *((int*)f->esp + 1);
    pos = *((unsigned*)f->esp + 2);

    struct file* my_file = get_file(fd)->ptr;

    if (my_file == NULL)
    {
      return;
    }

    sema_down(&file_lock);
    file_seek(my_file,pos);
    sema_up(&file_lock);
  }

  // returning the next byte in a file
  else if (*(int*)f->eip == SYS_TELL){
    check_void_ptr(f->esp + 4);
    int fd = *((int*)f->esp + 1);

    struct file* my_file = get_file(fd)->ptr;
    if (my_file == NULL)
    {
      f->eax = -1;
      return;
    }

    sema_down(&file_lock);
    f->eax = file_tell(my_file);
    sema_up(&file_lock);
  }

  // closing file
  else if (*(int*)f->eip == SYS_CLOSE){
    check_void_ptr(f->esp + 4);

    int fd = *((int*) f->esp + 1);
    struct open_file* my_file = get_file(fd);

    if (my_file == NULL)
    {
      return;
    }

    sema_down(&file_lock);
    file_close(my_file->ptr);
    sema_up(&file_lock);
    list_remove(&my_file->elem);
    palloc_free_page(my_file);
  }

  thread_exit ();
}

//checking if void pointer
void check_void_ptr(const void* pt)
{
  if (pt == NULL || !is_user_vaddr(pt) || pagedir_get_page(thread_current()->pagedir, pt) == NULL) 
  {
    sys_exit(-1);

  }
}

// gets the file using its file descriptor 
struct open_file* get_file(int fd){
    struct thread* t = thread_current();
    struct file* my_file = NULL;
    for (struct list_elem* e = list_begin (&t->open_file_list); e != list_end (&t->open_file_list);
    e = list_next (e))
    {
      struct open_file* opened_file = list_entry (e, struct open_file, elem);
      if (opened_file->fd == fd)
      {
        return opened_file;
      }
    }
    return NULL;
}

// exiting process
void sys_exit (int status){
    printf("ALOOOOOOOO");

    struct thread* parent = thread_current()->parent_thread;
    printf("%s: exit(%d)\n", thread_current()->name, status);
    if(parent) parent->child_status = status;
    
    // closing all opened files
    while (!list_empty(&thread_current()->open_file_list)){
      struct open_file *my_file = list_entry(list_pop_back(&thread_current()->open_file_list), struct open_file, elem);
      sema_down(&file_lock);
      file_close(my_file->ptr);
      sema_up(&file_lock);
      list_remove(&my_file->elem);
      palloc_free_page(my_file);
    }
  exit_process(status);
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