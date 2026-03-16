#include <iostream>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <cstring>
#include <string>
#include "utils.h"

using std::cin,std::cout,std::endl,std::flush;
using std::stoi,std::strlen,std::string;

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
    
    
    string log_entry = "STAGE ONE: Clock calculation and temporary storage\n";
    if (access("bin/log.txt", F_OK) == 0) {
        log_entry = "Starting from existing chunks\n";
    }
    cout << log_entry << flush;
    int logfd = open("bin/log.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
    write(logfd, log_entry.c_str(), log_entry.length());
    close(logfd);
    
    
    
    for(uint32_t block = 0; block < 0x100; ++block) {
        char fname[20];
        sprintf(fname, "bin/clocks%02X.bin", block);
        
        if (access(fname, F_OK) == 0) {
            continue;
        }
        
        int fd = open("bin/temp.bin", O_RDWR | O_CREAT, 0666);
        size_t fsize = 0x1000000 * sizeof(uint32_t);
        ftruncate(fd, fsize);
        
        uint32_t * data = (uint32_t *) mmap(NULL, fsize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (data == MAP_FAILED) {
            perror("mmap");
            return 1;
        }
        
        sfmt_t sfmt;
        uint32_t target = skipped * 2;
        
        uint32_t offset = block << 24;
        #pragma omp parallel for schedule(static)
        for(uint32_t seed = 0; seed < 0x1000000; ++seed) {
            sfmt_init_gen_rand(&sfmt, offset | seed);
            sfmt_skip_rands(&sfmt,target);
            uint32_t clocks = calc_clocks(&sfmt);
            data[seed] = clocks;
        }
        
        msync(data, fsize, MS_SYNC);
        munmap(data, fsize);
        close(fd);
        
        int read_fd = open("bin/temp.bin", O_RDONLY);
        struct stat stat_buf;
        fstat(read_fd, &stat_buf);
        int write_fd = open(fname, O_WRONLY | O_CREAT | O_TRUNC, stat_buf.st_mode);
        if (write_fd == -1) {
            perror("Error opening destination file");
            close(read_fd);
            return -1;
        }

        off_t off = 0;
        // Blast the bytes from one file to the other
        if (sendfile(write_fd, read_fd, &off, stat_buf.st_size) == -1) {
            perror("Error with sendfile");
            close(read_fd);
            close(write_fd);
            return -1;
        }
        close(read_fd);
        close(write_fd);
        
        int logfd = open("bin/log.txt", O_WRONLY | O_APPEND, 0666);
        char log_entry[256];
        sprintf(log_entry, "Block %02X completed\n", block);
        write(logfd, log_entry, strlen(log_entry));
        close(logfd);
        cout << log_entry << flush;
    }
}