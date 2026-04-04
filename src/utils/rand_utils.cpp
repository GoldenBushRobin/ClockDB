#include "rand_utils.h"

void sfmt_skip_rands(sfmt_t* sfmt, size_t skipped) {
    uint32_t new_idx = sfmt->idx + skipped * 2;
    uint32_t passes = new_idx / SFMT_N32;
    for (uint32_t i = 0; i < passes; ++i) sfmt_gen_rand_all(sfmt);
    sfmt->idx = new_idx % SFMT_N32;
}

void sfmt_init_clocks(sfmt_t* sfmt, uint32_t seed, size_t skipped) {
    sfmt_init_gen_rand(&sfmt, seed);
    sfmt_skip_rands(&sfmt,skipped);    
}

uint32_t calc_clocks(sfmt_t* sfmt) {
    uint32_t clocks = 0;
    for(int i = 0; i < 7; ++i) {
        clocks += clocks << 4;
        clocks += sfmt_genrand_uint64(sfmt) % 17;
    }
    return clocks;
}
