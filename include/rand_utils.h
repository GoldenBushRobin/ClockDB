#pragma once

#define SFMT_MEXP 19937
extern "C" {
#include "SFMT/SFMT.h"
}

#include <cstdint>

void sfmt_init_clocks(sfmt_t* sfmt, uint32_t seed, size_t skipped);
uint32_t calc_clocks(sfmt_t* sfmt);
uint32_t sfmt_calc_once(uint32_t seed, size_t skipped);