#include "search_utils.h"
#include "log_utils.h"
#include "rand_utils.h"
#include "constants.h"

#include <sstream>
#include <algorithm>

using std::string, std::vector, std::set;

int parseLine(string input, vector<uint32_t> &clocks) {
    if(input.length() > 256) {
        LOG(ERROR, "Input string is too long");
        return 1;
    }
    std::replace(input.begin(), input.end(), ',', ' ');
    std::istringstream ss(input);
    uint32_t clock = 0;
    while(ss >> clock) {
        if(clock > 16) {
            LOG(ERROR, "Input not within 0-16");
            return 1;
        }
        clocks.push_back(clock);
    }
    if (ss.fail() && !ss.eof()) {
        LOG(ERROR, "Invalid input: need space separated numbers 0-16");
        return 1;
    }
    return 0;
}

uint32_t clockConvert(vector<uint32_t> &clocks) {
    uint32_t clock = 0;
    for(size_t pos = 0; pos < 7 && pos < clocks.size(); ++pos) {
        if(clocks[pos] > 16) {
            LOG(ERROR, "Invalid input: clocks need to be in 0-16");
            return -1;
        }
        clock += CLOCKOFFS[pos] * clocks[pos];
    }
    return clock;
}

void addClockCands(size_t clockNum, set<uint32_t> &clkgroups) {
    set<uint32_t> addon;
    for(auto clock : clkgroups) {
        addon.insert((clock + CLOCKOFFS[clockNum]) % MAX_CLOCK);
        addon.insert((clock + 16 * CLOCKOFFS[clockNum]) % MAX_CLOCK);
    }
    clkgroups.merge(addon);
}

int clockCandidates(uint32_t clock, size_t num, set<uint32_t> &clkgroups, bool fuzzy) {
    if(clock >= MAX_CLOCK) {
        LOG(ERROR, "Clock input to large, %d", clock);
        return 1;
    } else if(num < 7 && clock % CLOCKOFFS[num - 1]) {
        LOG(ERROR, "Initial clock does not have zeroed tail");
    }
    if(!clkgroups.empty()) {
        LOG(ERROR, "Clock groups isnt empty");
        return 1;
    }
 
    clkgroups.insert(clock);
    if(fuzzy) {
        for(size_t pos = 0; pos < 7 && pos < num; ++pos) {
            addClockCands(pos, clkgroups);
        }
    }
    return 0;
}

size_t countCandidates(set<uint32_t> &clkgroups, size_t num, uint32_t *indices) {
    size_t offset = 1;
    if(num < 7) {
        offset = CLOCKOFFS[num - 1];
    }
    size_t count = 0;
    for(auto clock : clkgroups) {
        if(clock >= MAX_CLOCK) {
            LOG(ERROR, "Initial clock is too large, %d", clock);
            return 1;
        }
        if(clock % offset) {
            LOG(ERROR, "Initial clock does not have zeroed tail, %d", clock);
            return 1;
        }
        count += indices[clock + offset] - indices[clock];
    }
    return count;
}

int getSeeds(set<uint32_t> &clkgroups, size_t num, uint32_t * seedfiles[SEG_COUNT], uint32_t *indices, set<uint32_t> &seeds) {
    if(num > 7) num = 7;
    for(uint32_t start : clkgroups) {
        uint32_t end = start + CLOCKOFFS[num - 1];
        uint32_t fileno = start / SEG_SIZE;
        
        if((end - 1) / SEG_SIZE != fileno) {
            uint32_t mid = (fileno + 1) * SEG_SIZE;
            for(size_t pos = 0; pos < indices[end] - indices[mid]; ++pos) {
                seeds.insert(seedfiles[fileno + 1][pos]);
            }
            end = mid;
        }

        for(size_t pos = indices[start]; pos < indices[end]; ++pos) {
            seeds.insert(seedfiles[fileno][pos - indices[fileno * SEG_SIZE]]);
        }
    }

    return 0;
}

int filter(vector<uint32_t> &clocks, uint32_t skipped, set<uint32_t> &seeds, size_t start) {
    size_t num = clocks.size();
    if(seeds.size() >= 0x1000000) return 1;

    for(auto it = seeds.begin(); it != seeds.end();) {
        uint32_t seed = *it;
        sfmt_t sfmt;
        skipped += start * 2;
        sfmt_init_clocks(&sfmt, seed, skipped);
        
        uint32_t clock = 0;
        bool valid = true;
        for(size_t pos = start; pos < num; ++pos) {
            size_t off = pos - start;
            if(off % 7 == 0) clock = calc_clocks(&sfmt);
            uint32_t clk = clock / CLOCKOFFS[off % 7];
            if(clocks[pos] != clk && clocks[pos] != (clk + 1) % 17 && clocks[pos] != (clk + 16) % 17) {
                valid = false;
                break;
            }
            clock %= CLOCKOFFS[off % 7];
        }

        if(!valid) {
            it = seeds.erase(it);
        } else {
            ++it;
        }
    }

    return 0;
}
