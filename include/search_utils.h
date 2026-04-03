#pragma once

#include "constants.h"

#include <vector>
#include <string>
#include <cstdint>


int parseLine(std::string input, std::vector<uint32_t> &clocks);
int clockConvert(std::vector<uint32_t> &clocks);
int clockCandidates(uint32_t clock, uint32_t num, std::vector<uint32_t> &seeds);
size_t countCandidates(std::vector<uint32_t> &clocks, size_t numInputs, uint32_t *indices);
int getSeeds(std::vector<uint32_t> &clocks, uint32_t numInputs, uint32_t * seedfiles[SEG_COUNT], uint32_t *indices, std::vector<uint32_t> &seeds);
int filter(std::vector<uint32_t> &inputs, uint32_t skipped, std::vector<uint32_t> &seeds);
