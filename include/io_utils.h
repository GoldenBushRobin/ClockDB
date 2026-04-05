#pragma once
#include <cstddef>

void creat(const char * path);
void read(const char * path, void * buf, size_t size);
void write(const char * path, void * buf, size_t size);
void append(const char * path, void * buf, size_t size);

int make_map(const char * path, void * &mapping, size_t size);
void close_map(int fd, void * &mapping, size_t size);
void read_map(const char * path, void * data, size_t size);
