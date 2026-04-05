#include "build_utils.h"
#include "io_utils.h"
#include "constants.h"
#include "log_utils.h"
// I/O
#include <iostream>
#include <filesystem>
// #include <fstream>

// // data types
// #include <vector>
// #include <string>
// #include <cstring>

// // other
#include <numeric>
#include <algorithm>
// #include <cassert>

using std::cin,std::cout,std::cerr,std::endl,std::flush;
using std::stoi;
// ,std::string,std::vector,std::memcpy;
// using std::ifstream,std::ofstream;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    uint32_t skipped;
    if (argc > 2 && std::strcmp(argv[1],"--offset") == 0) {
        skipped = stoi(argv[2]);
    } else {
        cout << "Offset> ";
        cin >> skipped;
    }
    
    fs::path binDir = fs::path("bin") / std::to_string(skipped);
    fs::path countFile = binDir / "counts.bin";
    fs::path indexFile = binDir / "indices.bin";
    fs::path clockFiles[SEG_COUNT];
    fs::path seedFiles[SEG_COUNT];

    for(uint32_t seg = 0; seg < SEG_COUNT; ++seg) {
        auto segStr = std::to_string(seg);
        clockFiles[seg] = binDir / ("clocks" + segStr + ".bin");
        seedFiles[seg] = binDir / ("seeds" + segStr + ".bin");
    }
    
    try {
        if(fs::create_directories(binDir)) {
            LOG(INFO, "Created directory: %s", binDir.c_str());
        } else {
            LOG(INFO, "Directory already exists: %s", binDir.c_str());
        }
    } catch (const fs::filesystem_error &e) {
        LOG(INFO, "Error creating directory: %s", e.what());
    }
    
    for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
        creat(clockFiles[seg].c_str());
        creat(seedFiles[seg].c_str());
    }
    
    size_t BLOCKCOUNT = 16;
    size_t BLOCKSIZE = (1ULL << 32) / BLOCKCOUNT;
    size_t segsizes[SEG_COUNT] = {0}; // by indices
    size_t filesizes[SEG_COUNT] = {0}; // by bytes

    LOG(INFO, "STAGE ONE: Clock calculation and temporary storage");
    
    uint32_t extra = SEG_COUNT * PAGEENTS;
    size_t datasize = BLOCKSIZE + extra;
    
    psort32 pair_sort;
    pair_sort.Init(datasize);
    
    uint32_t * clocks;
    uint32_t * seeds;
    void* tmp;

    posix_memalign(&tmp, PAGESIZE, datasize * SIZE32);
    clocks = static_cast<uint32_t*>(tmp);
    posix_memalign(&tmp, PAGESIZE, datasize * SIZE32);
    seeds = static_cast<uint32_t*> (tmp);
    {
        for(size_t block = 0; block < BLOCKCOUNT; ++block) {
            buildClocks(skipped, block * BLOCKSIZE, BLOCKSIZE, pair_sort, clocks, seeds);
            char repl = -1;
            if(block == BLOCKCOUNT - 1) repl = 0;
            memset(clocks + BLOCKSIZE, repl, extra * 4);
            memset(seeds + BLOCKSIZE, repl, extra * 4);
            
            size_t sizes[SEG_COUNT] = {0};
            size_t offsets[SEG_COUNT] = {0};

            // SEG_SIZE
            for(size_t seg = 0; seg < SEG_COUNT - 1; ++seg) {
                size_t cutoff = (seg + 1) * SEG_SIZE - 1;
                auto it = std::upper_bound(clocks, clocks + datasize, cutoff);
                sizes[seg] = std::distance(clocks, it);
            }
            sizes[SEG_COUNT - 1] = BLOCKSIZE;
            
            for(size_t seg = SEG_COUNT - 1; seg > 0; --seg) {
                sizes[seg] = sizes[seg] - sizes[seg - 1];
            }

            for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
                segsizes[seg] += sizes[seg];
                offsets[seg] = PAGEENTS - (sizes[seg] % PAGEENTS);
            }

            size_t pos = BLOCKSIZE;
            for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
                for(size_t count = 0; count < offsets[seg]; ++count) {
                    clocks[pos++] = (seg + 1) * SEG_SIZE - 1;
                }
            }
            // feed back to be sorted to separate blocks page aligned(allow for faster sorting)
            auto sorted = pair_sort.Sort(clocks, seeds, pos);
            
            pos = 0;
            for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
                size_t cutoff = (seg + 1) * SEG_SIZE - 1;
                size_t writesize = (sizes[seg] + offsets[seg]) * SIZE32;
                
                // there will be less than 1024 of these, so is fine to iterate
                for(size_t index = pos + sizes[seg] + offsets[seg] - 1; sorted.first[index] == cutoff;--index) {
                    uint32_t check = 0xFFFFFFFF;
                    if(!repl) check = 0;
                    if(sorted.second[index] == check) {
                        sorted.first[index] = 0xFFFFFFFF;
                        sorted.second[index] = 0;
                    }
                }
                
                append(clockFiles[seg].c_str(), sorted.first + pos, writesize);
                append(seedFiles[seg].c_str(), sorted.second + pos, writesize);
                // let's pretend it worked and move on
                pos += sizes[seg] + offsets[seg];
                filesizes[seg] += writesize;
            }
        }
    }
    free(clocks);
    free(seeds);
    pair_sort.Init(0);

    LOG(INFO, "STAGE ONE COMPLETE\n");

    LOG(INFO, "STAGE TWO: Sort built files");
    
    {
        size_t sortsize = *std::max_element(filesizes, filesizes + SEG_COUNT);

        pair_sort.Init(sortsize / SIZE32);
        posix_memalign(&tmp, PAGESIZE, sortsize * SIZE32);
        clocks = static_cast<uint32_t*>(tmp);
        posix_memalign(&tmp, PAGESIZE, sortsize * SIZE32);
        seeds = static_cast<uint32_t*> (tmp);
        
        for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
            LOG(INFO, "Coping clocks from file %s", clockFiles[seg].c_str());
            read_map(clockFiles[seg].c_str(), clocks, filesizes[seg]);
            
            LOG(INFO, "Coping seeds from file %s", clockFiles[seg].c_str());
            read_map(seedFiles[seg].c_str(), seeds, filesizes[seg]);
            
            LOG(INFO, "Sorting clocks and seeds");
            auto sorted = pair_sort.Sort(clocks, seeds, filesizes[seg] / SIZE32);

            LOG(INFO, "Writing sorted clocks to file");
            write(clockFiles[seg].c_str(), sorted.first, segsizes[seg] * SIZE32);
            LOG(INFO, "Writing sorted seeds to file");
            write(seedFiles[seg].c_str(), sorted.second, segsizes[seg] * SIZE32);
        }
    }
    free(clocks);
    free(seeds);
    pair_sort.Init(0);

    LOG(INFO, "STAGE TWO COMPLETE\n");

    LOG(INFO, "STAGE THREE: Count clocks in files");
    
    {
        uint32_t * counts = (uint32_t*) calloc((MAX_CLOCK + 1), SIZE32);
        for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
            LOG(INFO, "Counting clocks for block %ld",seg);
            countClocks(clockFiles[seg].c_str(), counts, segsizes[seg]);    
            LOG(INFO, "Segment %ld clocks added.",seg);
        }
        
        LOG(INFO, "Calculating indices from counts to %s", indexFile.c_str());
        std::exclusive_scan(counts,counts + MAX_CLOCK + 1,counts,0);
        write(indexFile.c_str(), counts, (MAX_CLOCK + 1) * SIZE32);
        free(counts);
    }
    LOG(INFO, "STAGE THREE COMPLETE\n");
    
    return 0;
}