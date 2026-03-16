#include "utils.h"

void sfmt_skip_rands(sfmt_t * sfmt, uint32_t skipped) {
    uint32_t new_idx = sfmt->idx + skipped * 2;
    uint32_t passes = new_idx / SFMT_N32;
    for (uint32_t i = 0; i < passes; ++i) sfmt_gen_rand_all(sfmt);
    sfmt->idx = new_idx % SFMT_N32;
}