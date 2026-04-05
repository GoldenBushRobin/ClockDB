#pragma once

#include "constants.h"

#include <vector>
#include <string>
#include <cstdint>
#include <set>

int parseLine(std::string input, std::vector<uint32_t> &clocks);
uint32_t clockConvert(std::vector<uint32_t> &clocks);
int clockCandidates(uint32_t clock, size_t num, std::set<uint32_t> &clkgroups, bool fuzzy = true);
size_t countCandidates(std::set<uint32_t> &clkgroups, size_t num, uint32_t *indices);
int getSeeds(std::set<uint32_t> &clkgroups, size_t num, uint32_t * seedfiles[SEG_COUNT], uint32_t *indices, std::set<uint32_t> &seeds);
int filter(std::vector<uint32_t> &clocks, uint32_t skipped, std::set<uint32_t> &seeds, size_t start = 0);
