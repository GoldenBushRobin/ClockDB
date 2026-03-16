#include "utils.h"

void sfmt_skip_rands(sfmt_t * sfmt, uint32_t target) {
    uint32_t new_idx = sfmt->idx + target;
    uint32_t passes = new_idx / SFMT_N32;
    for (uint32_t i = 0; i < passes; ++i) sfmt_gen_rand_all(sfmt);
    sfmt->idx = new_idx % SFMT_N32;
}

uint32_t calc_clocks(sfmt_t * sfmt) {
    uint32_t arr[7];
    for(int i = 0; i < 7; ++i) {
        arr[i] = sfmt_genrand_uint64(sfmt) % 17;
    }
    
    uint32_t clocks = 24137569*arr[0] +\
                     1419857*arr[1] +\
                     83521*arr[2] +\
                     4913*arr[3] +\
                     289*arr[4] +\
                     17*arr[5] +\
                     arr[6];
     return clocks;
}