#pragma once

void read(char * path, void * buf, size_t size);
void write(char * path, void * buf, size_t size);
void append(char * path, void * buf, size_t size, off_t off);

int read_map(char * path, void * &data, size_t size);
void close_map(int fd, void * &data, size_t size);