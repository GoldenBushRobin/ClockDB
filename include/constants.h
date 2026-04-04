#pragma once

#define MAX_CLOCK 410338673

#define BLOCK_COUNT 256ULL // must be a power of 2
#define BLOCK_SIZE 0x100000000ULL / BLOCK_COUNT

#define SEG_COUNT 17
#define SEG_SIZE (MAX_CLOCK / SEG_COUNT)

#ifndef PAGESIZE
#define PAGESIZE sysconf(_SC_PAGESIZE)
#endif
#define PAGEENTS (PAGESIZE / sizeof(uint32_t))

inline int CLOCKOFFS[7] = {24137569, 1419857, 83521, 4913, 289, 17, 1};