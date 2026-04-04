#include "io_utils.h"
#include <fcntl.h>
#include <unistd.h>

void read(char * path, void * buf, size_t size) {
    int fd = open(path, O_RDONLY);
    read(fd, buf, size);
    close(fd);
}

void write(char * path, void * buf, size_t size) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644)
    write(fd, buf, size);      
    close(fd);
}

void append(char * path, void * buf, size_t size, off_t off) {
    int fd = open(path, O_WRONLY | O_APPEND)
    pwrite(fd, buf, size, off);
    close(fd);
}

int read_map(char * path, void * &data, size_t size) {
    int fd = open(path, O_RDONLY);
    data = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    return fd;
}

void close_map(int fd, void * &data, size_t size) {
    munmap(data, size);
    data = NULL;
    close(fd);
}