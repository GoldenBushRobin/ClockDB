#include "build_utils.h"
#include "log_utils.h"
#include "rand_utils.h"
#include "io_utils.h"

void buildClocks(size_t skipped, uint32_t offset, size_t size, psort32 &pair_sort, uint32_t * clocks, uint32_t * seeds) {    
    LOG(INFO, "Calculating clocks for seeds from %08X to %08lX", offset, offset + size - 1);
    
    #pragma omp parallel for
    for(uint32_t idx = 0; idx < size; ++idx) {
        uint32_t seed = offset + idx;
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

void countClocks(const char * path, uint32_t * counts, size_t size) {
    uint32_t * clockdata;
    size_t fsize = size * sizeof(uint32_t);
    void* tmp;
    int fd = make_map(path, tmp, fsize);
    clockdata = static_cast<uint32_t*>(tmp);
    #pragma omp parallel for
    for(uint32_t seed = 0; seed < size; ++seed) {
        ++counts[clockdata[seed]];
    }
    close_map(fd, tmp, fsize);
}
