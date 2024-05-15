#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init (void);
void check_void_ptr(const void* pt);
struct open_file* get_file(int fd);
#endif /* userprog/syscall.h */
