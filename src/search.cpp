#include "utils.h"
#include "search_utils.h"

#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

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
    string ignore;
    getline(cin,ignore);

    string binDir = std::format("bin/{:d}/", skipped);
    string countName = binDir + "counts.bin";
    string indexName = binDir + "indices.bin";
    string clockNames[SEG_COUNT];
    string seedNames[SEG_COUNT];
    for(uint32_t seg = 0; seg < SEG_COUNT; ++seg) {
        clockNames[seg] = std::format("{}clocks{:d}.bin", binDir, seg);
        seedNames[seg] = std::format("{}seeds{:d}.bin", binDir, seg);
    }
    
    // Load index file into mmapped memory
    int idxfd = open(indexName.c_str(), O_RDONLY);
    uint32_t* indices = (uint32_t *) mmap(NULL, (MAX_CLOCK + 1) * sizeof(uint32_t), PROT_READ, MAP_PRIVATE, idxfd, 0);

    // Load seed files into memory

    int seedfd[SEG_COUNT];
    uint32_t* seedfiles[SEG_COUNT];
    
    for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
        seedfd[seg] = open(seedNames[seg].c_str(), O_RDONLY);
        seedfiles[seg] = (uint32_t *) mmap(NULL, (1 << 30), PROT_READ, MAP_PRIVATE, seedfd[seg], 0);
    }

    string input;
    do {
        cout << "Input clock values> ";
        std::getline(cin, input);
        if(!input.length()) continue;

        vector<uint32_t> clockin;
        if(int res = parseLine(input,clockin)) {
            cerr << "Error in parsing input, error code " << res;
            continue;
        }
        int clock = clockConvert(clockin);
        if(clock == -1) {
            cerr << "Error in converting clocks";
            continue;
        }
        vector<uint32_t> clocks;
        if(int res = clockCandidates(clock, clockin.size(), clocks)) {
            cerr << "Error in generating clock candidates, error code " << res;
            continue;
        }

        size_t num_cand = countCandidates(clocks, clockin.size(), indices);
        cout << "There are " << num_cand << " candidates for the seed\n";

        vector<uint32_t> seeds;
        if(num_cand < 100000) { // with 7 inputs, shouldd be aroun 20,000
            if(getSeeds(clocks, clockin.size(), seedfiles, indices, seeds)) {
                cerr << "Error in retrieving candidate seeds\n";
                continue;
            }
            if(filter(clockin,skipped,seeds)) {
                cerr << "Error in filtering seeds\n";
                continue;
            }
            if(seeds.size() < 100) {
                cout << "List of possible seeds:";
                for(auto seed : seeds) cout << ' ' << seed;
                cout << endl;
            } else {
                cout << "Too many remaining seeds, " << seeds.size() << " seeds left after filtering\n";
            }
        }
    } while(!input.empty());

    munmap(indices, (MAX_CLOCK + 1) * sizeof(uint32_t));
    close(idxfd);
}