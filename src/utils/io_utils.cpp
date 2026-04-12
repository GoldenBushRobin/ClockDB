#include "io_utils.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstring>

void creat(const char * path) {
    creat(path, 0644);
}

void read(const char * path, void * buf, size_t size) {
    int fd = open(path, O_RDONLY);
    read(fd, buf, size);
    close(fd);
}

void write(const char * path, void * buf, size_t size) {
    int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fd, buf, size);      
    close(fd);
}

void append(const char * path, void * buf, size_t size) {
    int fd = open(path, O_WRONLY | O_APPEND);
    write(fd, buf, size);
    close(fd);
}

int make_map(const char * path, void * &mapping, size_t size) {
    int fd = open(path, O_RDONLY);
    if(fd < 0) return -1;
    mapping = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
    return fd;
}

void close_map(int fd, void * &mapping, size_t size) {
    if(fd < 0) return;
    munmap(mapping, size);
    mapping = NULL;
    close(fd);
}

void read_map(const char * path, void * data, size_t size) {
    void * mapping;
    int fd = make_map(path, mapping, size);
    if(fd < 0) return;
    memcpy(data, mapping, size);
    close_map(fd, mapping, size);
}



