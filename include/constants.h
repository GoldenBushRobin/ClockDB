#pragma once
#include <unistd.h>

#define SIZE32 sizeof(uint32_t)

#define MAX_CLOCK 410338673

#define BLOCK_COUNT 256ULL
#define BLOCK_SIZE 0x100000000ULL / BLOCK_COUNT

#define SEG_COUNT 17
#define SEG_SIZE (MAX_CLOCK / SEG_COUNT)

#ifndef PAGESIZE
#define PAGESIZE sysconf(_SC_PAGESIZE)
#endif
#define PAGEENTS (PAGESIZE / SIZE32)

inline int CLOCKOFFS[7] = {24137569, 1419857, 83521, 4913, 289, 17, 1};