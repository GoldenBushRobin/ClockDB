#include "build_utils.h"
#include "log_utils.h"
#include "rand_utils.h"
#include "io_utils.h"

void buildClocks(size_t skipped, uint32_t offset, size_t size, psort32 &pair_sort, uint32_t * clocks, uint32_t * seeds) {    
    LOG(INFO, "Calculating clocks for seeds from %08lX to %08lX", offset, offset + size - 1);
    
    #pragma omp parallel for
    for(uint32_t idx = 0; idx < size; ++idx) {
        uint32_t seed = offset + idx
        sfmt_t sfmt;
        sfmt_init_clocks(&sfmt, seed, skipped);
        clocks[idx] = calc_clocks(&sfmt);
        seeds[idx] = seed;
    }
    
    LOG(INFO, "Sorting clocks and seeds");
    
    auto sorted = pair_sort.Sort(clocks, seeds, size);
    memcpy(clocks, sorted.first, size * sizeof(uint32_t));
    memcpy(seeds, sorted.second, size * sizeof(uint32_t));
    
    LOG(INFO, "Copied into vector");
}

void countClocks(char * path, uint32_t * counts, size_t size) {
    uint32_t * clockdata;
    size_t fsize = size * sizeof(uint32_t);
    int fd = read_map(path, clockdata, fsize);
    
    #pragma omp parallel for
    for(uint32_t seed = 0; seed < size; ++seed) {
        ++counts[clockdata[seed]];
    }
    close_map(fd, clockdata, fsize);
}

size_t binarySearch(uint32_t* arr, size_t size, uint32_t val) {
    size_t pos = 0;
    size_t start = 1 << 63;
    while(start * 2 < size) start <<= 1;
    for(size_t jump = start; jump != 0;jump >>= 1) {
        if(pos + jump < size && arr[pos + jump] < val) {
            pos += jump;
        }
    }
    return pos + 1;
}