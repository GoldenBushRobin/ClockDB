#include "path_utils.h"
#include "log_utils.h"
#include "io_utils.h"
#include "constants.h"

#include <filesystem>
namespace fs = std::filesystem;

fs::path binDir;
fs::path countFile;
fs::path indexFile;

fs::path clockFiles[SEG_COUNT];
fs::path seedFiles[SEG_COUNT];

void initialize_paths(uint32_t skipped) {
    binDir = fs::path("bin") / std::to_string(skipped);
    countFile = binDir / "counts.bin";
    indexFile = binDir / "indices.bin";
    for(uint32_t seg = 0; seg < SEG_COUNT; ++seg) {
        auto segStr = std::to_string(seg);
        clockFiles[seg] = binDir / ("clocks" + segStr + ".bin");
        seedFiles[seg] = binDir / ("seeds" + segStr + ".bin");
    }
}

void setup_filestructure() {
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
}


void seeddata::loadfiles(size_t skipped) {
    initialize_paths(skipped);
    void* tmp;
    idxfd = make_map(indexFile.c_str(), tmp, (MAX_CLOCK + 1) * SIZE32);
    indices = static_cast<uint32_t*>(tmp);

    for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
        seedfd[seg] = make_map(seedFiles[seg].c_str(), tmp, (1 << 30));
        seedfiles[seg] = static_cast<uint32_t*>(tmp);
    }
}

void seeddata::close() {
    void* tmp = static_cast<void*>(indices);
    close_map(idxfd, tmp, (MAX_CLOCK + 1) * SIZE32);
    
    for(size_t seg = 0; seg < SEG_COUNT; ++seg) {
        tmp = static_cast<void*>(seedfiles[seg]);
        close_map(seedfd[seg], tmp, (1 << 30));
    }
}