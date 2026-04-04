#pragma once

#define SFMT_MEXP 19937
extern "C" {
#include "SFMT/SFMT.h"
}
void sfmt_skip_rands(sfmt_t * sfmt, uint32_t skipped);

#include "parallel-radix-sort/parallel_radix_sort.h"
#include "constants.h"

#include <cstdint>
#include <vector>
#include <string>
#include <chrono>
#include <format>

#define psort32 parallel_radix_sort::PairSort<uint32_t,uint32_t>

#define LOG_INFO(fmt, ...) fprintf(stderr, "[%s] " fmt "\n", std::format("{:%F %T}", std::chrono::system_clock::now()).c_str(), ##__VA_ARGS__)

void writeVector(std::vector<uint32_t>& vec, size_t size, std::string fname);
void readVector(std::vector<uint32_t>& vec, size_t size, std::string fname);

uint32_t calc_clocks(sfmt_t * sfmt);
void buildClocks(psort32& pair_sort, uint32_t block, uint32_t target, uint32_t * clocks, uint32_t * seeds, size_t blocksize = BLOCK_SIZE);
void countClocks(std::string clockName, std::vector<uint32_t> &counts, size_t size = BLOCK_SIZE);
size_t binarySearch(uint32_t* arr, size_t size, uint32_t val);