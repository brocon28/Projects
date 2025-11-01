//
// Created by bgoco on 10/28/2025.
//
#ifndef DATASETLOADER_H
#define DATASETLOADER_H

#include <vector>
#include "Song.h"
using namespace std;

class DatasetLoader {
public:
    static vector<Song> loadFromCSV(const string &filename, size_t maxRows = SIZE_MAX);
    static vector<Song> generateSynthetic(size_t n);
};

#endif // DATASETLOADER_H
