#pragma once

#include <cstdint>
extern "C" {
#include "./SFMT/SFMT.h"
}

void sfmt_skip_rands(sfmt_t * sfmt, uint32_t skipped);