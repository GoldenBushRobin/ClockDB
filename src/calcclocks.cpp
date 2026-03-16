#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "utils.h"

using std::cin,std::cout;
using std::stoi;


int main(int argc, char* argv[]) {
    uint32_t skipped;
    int argpos = 1;
    if (argc > 2 && argv[1] == "--offset") {
        skipped = stoi(argv[2]);
        argpos += 2;
    } else {
        cout << "Offset> ";
        cin >> skipped;
    }
    
    
    int fd = open("bin/clocks.bin", O_RDWR | O_CREAT, 0666);
    size_t fsize = 0x1000000 * sizeof(uint32_t);
    ftruncate(fd, fsize);
    
    uint32_t * data = (uint32_t *) mmap(NULL, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) {
        perror("mmap");
        return 1;
    }
    
    sfmt_t sfmt;
    uint32_t target = skipped * 2;
    #pragma omp parallel for schedule(static)
    for(uint32_t seed = 0; seed < 0x1000000; ++seed) {
        sfmt_init_gen_rand(&sfmt, seed);
        sfmt_skip_rands(&sfmt,target);
        uint32_t clocks = calc_clocks(&sfmt);
        data[seed] = clocks;
    }
    
    msync(data, fsize, MS_SYNC);
    munmap(data, fsize);
    close(fd);
}