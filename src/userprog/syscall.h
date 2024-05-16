#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);

void check_void_ptr(const void* pt);
void sys_exit (int status);
struct open_file* get_file(int fd);
void exit_process(int status);

#endif /* userprog/syscall.h */
