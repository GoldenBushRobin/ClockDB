#include "search_utils.h"
#include "log_utils.h"
#include "constants.h"
#include "io_utils.h"
#include "path_utils.h"

#include <iostream>
#include <filesystem>
#include <format>
#include <cstring>

using std::cin,std::cout,std::cerr,std::endl;
using std::stoi,std::string,std::vector,std::set;
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

    // Load index file into mmapped memory
    seeddata data;
    data.loadfiles(skipped);
    // Load seed files into memory
    
    string input;
    do {
        cout << "Input clock values> ";
        std::getline(cin, input);
        if(!input.length()) continue;

        vector<uint32_t> clockin;
        if(parseLine(input,clockin)) {
            cerr << "Error in parsing input";
            continue;
        }
        int clock = clockConvert(clockin);
        if(clock == -1) {
            cerr << "Error in converting clocks";
            continue;
        }
        set<uint32_t> clkgroups;
        if(clockCandidates(clock, clockin.size(), clkgroups)) {
            cerr << "Error in generating clock candidates";
            continue;
        }

        size_t num_cand = countCandidates(clkgroups, clockin.size(), data);
        cout << "There are " << num_cand << " candidates for the seed\n";

        set<uint32_t> seeds;
        if(num_cand < 100000) { // with 7 inputs, shouldd be aroun 20,000
            if(getSeeds(clkgroups, clockin.size(), data, seeds)) {
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

    data.close();
}