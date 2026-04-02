#include "utils.h"
// I/O
#include <iostream>
#include <filesystem>
#include <format>
#include <fstream>

// data types
#include <vector>
#include <string>
#include <cstring>

// memory mapping
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

// other
#include <numeric>
#include <algorithm>
#include <cassert>

using std::cin,std::cout,std::cerr,std::endl,std::flush;
using std::stoi,std::string,std::vector,std::memcpy;
using std::ifstream,std::ofstream;
namespace fs = std::filesystem;

int main(int argc, char* argv[]) {
    uint32_t skipped;
    int argpos = 1;
    if (argc > 2 && std::strcmp(argv[1],"--offset") == 0) {
        skipped = stoi(argv[2]);
        argpos += 2;
    } else {
        cout << "Offset> ";
        cin >> skipped;
    }
    uint32_t target = skipped * 2;

    string binDir = std::format("bin/{:d}/", skipped);
    string stateName = binDir + "state.bin";
    string countName = binDir + "counts.bin";
    string indexName = binDir + "indices.bin";
    string boundsName = binDir + "bounds.bin";
    string clockNames[SEG_COUNT];
    string seedNames[SEG_COUNT];
    for(uint32_t seg = 0; seg < SEG_COUNT; ++seg) {
        clockNames[seg] = std::format("{}clocks{:d}.bin", binDir, seg);
        seedNames[seg] = std::format("{}seeds{:d}.bin", binDir, seg);
    }
    
    try {
        if(fs::create_directories(binDir)) {
            LOG_INFO("Created directory: %s", binDir.c_str());
        } else {
            LOG_INFO("Directory already exists: %s", binDir.c_str());
        }
    } catch (const fs::filesystem_error &e) {
        LOG_INFO("Error creating directory: %s", e.what());
    }
    long PAGESIZE = sysconf(_SC_PAGESIZE);
    
    LOG_INFO("STAGE ONE: Clock calculation and temporary storage");
    
    for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
        int clockfd = open(clockNames[seg].c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
        int seedfd = open(seedNames[seg].c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0644);
        close(clockfd);
        close(seedfd);
    }

    uint32_t extra = SEG_COUNT * PAGESIZE;
    psort32 pair_sort;
    
    size_t segsizes[SEG_COUNT] = {0}; // by index
    size_t filesizes[SEG_COUNT] = {0};
    
    size_t BLOCKCOUNT = 256;
    size_t BLOCKSIZE = (1ULL << 32) / BLOCKCOUNT;
    {
        pair_sort.Init((BLOCKSIZE + extra) * 2);
        vector<uint32_t> clocks(BLOCKSIZE + extra);
        vector<uint32_t> seeds(BLOCKSIZE + extra);
        
        for(size_t block = 0; block < BLOCKCOUNT; ++block) {
            buildClocks(pair_sort, block, target, clocks, seeds);
            memset(clocks.data() + BLOCKSIZE, -1, extra);
            memset(seeds.data() + BLOCKSIZE, -1, extra);
            
            size_t sizes[SEG_COUNT] = {0};
            size_t offsets[SEG_COUNT] = {0};

            // SEG_SIZE
            for(size_t seg = 0; seg < SEG_COUNT - 1; ++seg) {
                size_t cutoff = (seg + 1) * SEG_SIZE - 1;
                auto it = std::upper_bound(clocks.begin(),clocks.end(), cutoff);
                sizes[seg] = std::distance(clocks.begin(), it);
            }
            sizes[SEG_COUNT - 1] = BLOCKSIZE;
            // for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
            //     cout << sizes[seg] << endl;
            // }            
            for(size_t seg = SEG_COUNT - 1; seg > 0; --seg) {
                sizes[seg] = sizes[seg] - sizes[seg - 1];
            }

            for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
                segsizes[seg] += sizes[seg];
                offsets[seg] = PAGESIZE - (sizes[seg] % PAGESIZE);
            }

            size_t pos = BLOCKSIZE;
            for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
                for(size_t count = 0; count < offsets[seg]; ++count) {
                    clocks[pos++] = (seg + 1) * SEG_SIZE - 1;
                }
            }
            // feed back to be sorted to separate blocks page aligned(allow for faster sorting)
            auto sorted = pair_sort.Sort(clocks.data(), seeds.data(), pos);
            
            pos = 0;
            for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
                int clockfd = open(clockNames[seg].c_str(), O_WRONLY);
                int seedfd = open(seedNames[seg].c_str(), O_WRONLY);
                
                off_t offset = lseek(clockfd, 0, SEEK_END);
                size_t writesize = (sizes[seg] + offsets[seg]) * sizeof(uint32_t);
                pwrite(clockfd, sorted.first + pos, writesize, offset);
                pwrite(seedfd, sorted.second + pos, writesize, offset);
                pos += sizes[seg] + offsets[seg];
                filesizes[seg] += writesize;
                close(clockfd);
                close(seedfd);
            }
        }
    }
    LOG_INFO("STAGE ONE COMPLETE\n");

    LOG_INFO("STAGE TWO: Sort built files");
    {
        size_t sortsize = *std::max_element(filesizes, filesizes + SEG_COUNT);
        sortsize /= sizeof(uint32_t);
        pair_sort.Init(sortsize);
        vector<uint32_t> clocks(sortsize);
        vector<uint32_t> seeds(sortsize);

        for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
            int clockfd = open(clockNames[seg].c_str(), O_RDONLY);
            void* clockdata = mmap(NULL, filesizes[seg], PROT_READ, MAP_PRIVATE, clockfd, 0);
            if(clockdata == MAP_FAILED) {
                perror("Map Failed");
                return 1;
            }
            memcpy(clocks.data(), clockdata, filesizes[seg]);
            munmap(clockdata, filesizes[seg]);
            close(clockfd);

            int seedfd = open(seedNames[seg].c_str(), O_RDONLY);
            void* seeddata = mmap(NULL, filesizes[seg], PROT_READ, MAP_PRIVATE, seedfd, 0);
            if(seeddata == MAP_FAILED) {
                perror("Map Failed");
                return 1;
            }
            memcpy(seeds.data(), seeddata, filesizes[seg]);
            munmap(seeddata, filesizes[seg]);
            close(seedfd);

            auto sorted = pair_sort.Sort(clocks.data(), seeds.data(), filesizes[seg] / sizeof(uint32_t));

            clockfd = open(clockNames[seg].c_str(), O_WRONLY | O_TRUNC);
            pwrite(clockfd, sorted.first, segsizes[seg] * sizeof(uint32_t), 0);
            close(clockfd);
            
            seedfd = open(seedNames[seg].c_str(), O_WRONLY | O_TRUNC);
            pwrite(seedfd, sorted.second, segsizes[seg] * sizeof(uint32_t), 0);
            close(seedfd);
        }
    }
    LOG_INFO("STAGE TWO COMPLETE\n");

    LOG_INFO("STAGE THREE: Count clocks in files");
        

    {
        vector<uint32_t> counts(MAX_CLOCK + 1, 0);
        for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
            LOG_INFO("Counting clocks for block %ld",seg);
            countClocks(clockNames[seg], counts);    
            LOG_INFO("Segment %ld clocks added.",seg);
        }
        writeVector(counts, MAX_CLOCK, countName);
        
        vector<uint32_t> indices(MAX_CLOCK + 1, 0);
        LOG_INFO("Calculating indices from counts to %s", indexName.c_str());
        std::exclusive_scan(counts.begin(),counts.end(),indices.begin(),0);
        writeVector(indices, MAX_CLOCK + 1, indexName);
    }
    LOG_INFO("STAGE THREE COMPLETE\n");
    
    return 0;
}