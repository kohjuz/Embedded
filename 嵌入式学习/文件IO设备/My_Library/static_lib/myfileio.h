#ifndef __MYFILEIO_H
#define __MYFILEIO_H

int my_open(const char *path,int flag);
ssize_t my_read(int fd,void *buf,size_t count);
ssize_t my_write(int fd,void *buf,size_t count);
int my_close(int fd);

#endif
