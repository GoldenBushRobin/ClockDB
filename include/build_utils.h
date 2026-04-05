#pragma once

#include "parallel-radix-sort/parallel_radix_sort.h"
#include <cstdint>
typedef parallel_radix_sort::PairSort<uint32_t,uint32_t> psort32;

void buildClocks(size_t skipped, uint32_t offset, size_t size, psort32 &pair_sort, uint32_t * clocks, uint32_t * seeds);
void countClocks(const char * path, uint32_t * counts, size_t size);
