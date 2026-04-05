#pragma once
#include "constants.h"
#include <filesystem>

extern std::filesystem::path binDir, countFile, indexFile;
extern std::filesystem::path clockFiles[SEG_COUNT];
extern std::filesystem::path seedFiles[SEG_COUNT];

void initialize_paths(uint32_t skipped);
void setup_filestructure();
void remove_clocks();

typedef struct seeddata {
    int idxfd;
    uint32_t* indices;
    int seedfd[SEG_COUNT];
    uint32_t* seedfiles[SEG_COUNT];
    void loadfiles(size_t skipped);
    void close();
} seeddata;

