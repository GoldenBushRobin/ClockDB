#pragma once

#include "constants.h"
#include "path_utils.h"

#include <vector>
#include <string>
#include <cstdint>
#include <set>
#include <cstdbool>

int parseLine(std::string input, std::vector<uint32_t> &clocks);
void rotateClocks(std::vector<uint32_t> &clocks, size_t add);
uint32_t clockConvert(std::vector<uint32_t> &clocks);
int clockCandidates(uint32_t clock, size_t num, std::set<uint32_t> &clkgroups, bool fuzzy = true);
size_t countCandidates(std::set<uint32_t> &clkgroups, size_t num, seeddata data);
int getSeeds(std::set<uint32_t> &clkgroups, size_t num, seeddata data, std::set<uint32_t> &seeds);
int filter(std::vector<uint32_t> &clocks, uint32_t skipped, std::set<uint32_t> &seeds, size_t start = 0);

size_t search_seeds(std::vector<uint32_t> &clocks, uint32_t skipped, seeddata data, std::set<uint32_t> &seeds, bool fuzzy = true);
