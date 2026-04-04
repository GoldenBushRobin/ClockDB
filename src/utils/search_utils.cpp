#include "search_utils.h"
#include "utils.h"

#include<sstream>

using std::string, std::vector;

int parseLine(string input, vector<uint32_t> &clocks) {
    if(input.length() > 256) return 1;
    std::stringstream ss(input);
    uint32_t clock = 0;
    while(ss >> clock) {
        if(clock > 16) return 1;
        clocks.push_back(clock);
    }
    if (ss.fail() && !ss.eof()) {
        return 2;
    }
    return 0;
}

int clockConvert(vector<uint32_t> &clocks) {
    uint32_t clock = 0;
    for(size_t pos = 0; pos < 7 && pos < clocks.size(); ++pos) {
        if(clocks[pos] > 16) return -1;
        clock += CLOCKOFFS[pos] * clocks[pos];
    }
    return clock;
}

int addClockCands(uint32_t clockNum, vector<uint32_t> &clocks) {
    size_t currSize = clocks.size();
    for(size_t idx = 0; idx < currSize; ++idx) {
        clocks.push_back((clocks[idx] + CLOCKOFFS[clockNum]) % MAX_CLOCK);
        clocks.push_back((clocks[idx] + 16 * CLOCKOFFS[clockNum]) % MAX_CLOCK);
    }
    return 0;
}

int clockCandidates(uint32_t clock, uint32_t num, vector<uint32_t> &clocks) {
    if(clock >= MAX_CLOCK || clocks.size()) return 1;

    clocks.push_back(clock);
    for(uint32_t pos = 0; pos < 7 && pos < num; ++pos) {
        addClockCands(pos, clocks);
    }
    return 0;
}

size_t countCandidates(vector<uint32_t> &clocks, size_t numInputs, uint32_t *indices) {
    size_t offset = 1;
    if(numInputs < 7) {
        for(size_t cnt = 7; cnt > numInputs; --cnt) {
            offset += (offset << 4);
        }
    }
    size_t count = 0;
    for(auto clock : clocks) {
        if(clock % offset || clock >= MAX_CLOCK) return -1;
        count += indices[clock + offset] - indices[clock];
    }
    return count;
}

int getSeeds(vector<uint32_t> &clocks, uint32_t numInputs, uint32_t * seedfiles[SEG_COUNT], uint32_t *indices, vector<uint32_t> &seeds) {
    if(numInputs > 7) numInputs = 7;
    for(uint32_t start : clocks) {
        uint32_t end = start + CLOCKOFFS[numInputs - 1];
        uint32_t fileno = start / SEG_SIZE;
        if((end - 1) / SEG_SIZE != fileno) {
            uint32_t mid = (fileno + 1) * SEG_SIZE;
            for(size_t pos = 0; pos < indices[end] - indices[mid]; ++pos) {
                seeds.push_back(seedfiles[fileno + 1][pos]);
            }
            end = mid;
        }

        for(size_t pos = indices[start]; pos < indices[end]; ++pos) {
            seeds.push_back(seedfiles[fileno][pos - indices[fileno * SEG_SIZE]]);
        }
    }

    return 0;
}

int filter(vector<uint32_t> &inputs, uint32_t skipped, vector<uint32_t> &seeds) {
    size_t numInputs = inputs.size();
    if(seeds.size() >= 0x1000000) return 1;

    for(auto it = seeds.begin(); it < seeds.end();) {
        uint32_t seed = *it;
        sfmt_t sfmt;
        sfmt_init_gen_rand(&sfmt, seed);
        sfmt_skip_rands(&sfmt, skipped * 2);
        
        uint32_t clock = 0;
        bool valid = true;
        for(size_t pos = 0; pos < numInputs; ++pos) {
            if(pos % 7 == 0) clock = calc_clocks(&sfmt);
            uint32_t clk = clock / CLOCKOFFS[pos % 7];
            if(inputs[pos] != clk && inputs[pos] != (clk + 1) % 17 && inputs[pos] != (clk + 16) % 17) {
                valid = false;
                break;
            }
            clock %= CLOCKOFFS[pos % 7];
        }

        if(!valid) {
            it = seeds.erase(it);
        } else {
            ++it;
        }
    }

    return 0;
}
