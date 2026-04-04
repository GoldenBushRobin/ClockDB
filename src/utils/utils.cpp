#include "utils.h"

#include <fstream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

using std::vector, std::string;

void sfmt_skip_rands(sfmt_t * sfmt, uint32_t target) {
    uint32_t new_idx = sfmt->idx + target;
    uint32_t passes = new_idx / SFMT_N32;
    for (uint32_t i = 0; i < passes; ++i) sfmt_gen_rand_all(sfmt);
    sfmt->idx = new_idx % SFMT_N32;
}

void writeVector(vector<uint32_t> &vec, size_t size, string fname) {
    std::ofstream ofs(fname, std::ios::binary);
    ofs.write(reinterpret_cast<char*>(vec.data()),size * sizeof(uint32_t));
    ofs.close();
}

void readVector(std::vector<uint32_t>& vec, size_t size, std::string fname) {
    std::ifstream ifs(fname, std::ios::binary);
    ifs.read(reinterpret_cast<char*>(vec.data()),size * sizeof(uint32_t));
    ifs.close();
}

uint32_t calc_clocks(sfmt_t * sfmt) {
    uint32_t arr[7];
    uint32_t clocks = 0;
    for(int i = 0; i < 7; ++i) {
        arr[i] = sfmt_genrand_uint64(sfmt) % 17;
        clocks += clocks << 4;
        clocks += arr[i];
    }
    return clocks;
}

void buildClocks(psort32 &pair_sort, uint32_t block, uint32_t target, uint32_t * clocks, uint32_t * seeds, size_t blocksize) {
    uint32_t offset = block * blocksize;
    
    LOG_INFO("Calculating clocks for seeds from %08X to %08lX", offset, offset + blocksize - 1);
    
    #pragma omp parallel for
    for(uint32_t seed = 0; seed < blocksize; ++seed) {
        sfmt_t sfmt;
        sfmt_init_gen_rand(&sfmt, offset | seed);
        sfmt_skip_rands(&sfmt,target);
        clocks[seed] = calc_clocks(&sfmt);
        seeds[seed] = offset | seed;
    }
    
    LOG_INFO("Sorting clocks and seeds");
    
    auto sorted = pair_sort.Sort(clocks, seeds, blocksize);
    memcpy(clocks, sorted.first, blocksize * sizeof(uint32_t));
    memcpy(seeds, sorted.second, blocksize * sizeof(uint32_t));
    
    LOG_INFO("Copied into vector");
    return;
}

void countClocks(string clockName, vector<uint32_t> &counts, size_t size) {
    int fd = open(clockName.c_str(), O_RDONLY);
    if (fd < 0) throw;
    uint32_t * clockdata = (uint32_t *) mmap(NULL, size * sizeof(uint32_t), PROT_READ, MAP_SHARED, fd, 0);
    
    #pragma omp parallel for
    for(uint32_t seed = 0; seed < size; ++seed) {
        ++counts[clockdata[seed]];
    }
    munmap(clockdata, size * sizeof(uint32_t));
    close(fd);
}

size_t binarySearch(uint32_t* arr, size_t size, uint32_t val) {
    size_t pos = 0;
    size_t start = 1;
    while(start * 2 < size) start <<= 1;
    for(size_t jump = start; jump != 0;jump >>= 1) {
        if(pos + jump < size && arr[pos + jump] < val) {
            pos += jump;
        }
    }
    return pos + 1;
}