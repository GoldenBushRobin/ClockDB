#pragma once

#include <cstdint>
#define SFMT_MEXP 19937
extern "C" {
#include "SFMT.h"
}

void sfmt_skip_rands(sfmt_t * sfmt, uint32_t skipped);
uint32_t calc_clocks(sfmt_t * sfmt);