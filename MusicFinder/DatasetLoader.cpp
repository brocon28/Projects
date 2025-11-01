//
// Created by bgoco on 10/28/2025.
//
#include "DatasetLoader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>
using namespace std;

// Trim whitespace helper
static string trim(const string &s) {
    size_t start = s.find_first_not_of(" \t\n\r");
    if (start == string::npos) return "";
    size_t end = s.find_last_not_of(" \t\n\r");
    return s.substr(start, end - start + 1);
}

// Split CSV line respecting quotes
static vector<string> splitCSVLine(const string &line) {
    vector<string> fields;
    string cur;
    bool inQuote = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '"') {
            if (inQuote && i + 1 < line.size() && line[i + 1] == '"') {
                cur.push_back('"'); // escaped quote
                ++i;
            } else {
                inQuote = !inQuote;
            }
        } else if (c == ',' && !inQuote) {
            fields.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    fields.push_back(cur);
    return fields;
}

vector<Song> DatasetLoader::loadFromCSV(const string &filename, size_t maxRows) {
    vector<Song> songs;
    ifstream in(filename, ios::binary);
    if (!in.is_open()) {
        cerr << "Error: Could not open CSV file: " << filename << endl;
        return songs;
    }

    string headerLine;
    if (!getline(in, headerLine)) {
        cerr << "Error: CSV file appears empty: " << filename << endl;
        return songs;
    }

    vector<string> headers = splitCSVLine(headerLine);
    auto idx_of = [&](const string &name) -> int {
        for (int i = 0; i < (int)headers.size(); ++i)
            if (trim(headers[i]) == name) return i;
        return -1;
    };

    int idx_name     = idx_of("name");
    int idx_artists  = idx_of("artists");
    int idx_pop      = idx_of("popularity");
    int idx_duration = idx_of("duration_ms");
    int idx_dance    = idx_of("danceability");
    int idx_energy   = idx_of("energy");

    if (idx_name < 0 || idx_artists < 0 || idx_pop < 0) {
        cerr << "Error: Required columns not found in CSV." << endl;
        return songs;
    }

    string line;
    size_t rowCount = 0;
    while (getline(in, line) && rowCount < maxRows) {
        vector<string> fields = splitCSVLine(line);
        if ((int)fields.size() <= max({idx_name, idx_artists, idx_pop})) continue;

        Song s;
        s.name   = trim(fields[idx_name]);
        s.artist = trim(fields[idx_artists]);
        try { s.popularity = stoi(trim(fields[idx_pop])); } catch (...) { s.popularity = 0; }
        try { s.duration_ms = stoi(trim(fields[idx_duration])); } catch (...) { s.duration_ms = 0; }
        try { s.danceability = stod(trim(fields[idx_dance])); } catch (...) { s.danceability = 0.0; }
        try { s.energy = stod(trim(fields[idx_energy])); } catch (...) { s.energy = 0.0; }

        songs.push_back(s);
        ++rowCount;
    }

    cout << "Loaded " << songs.size() << " songs from " << filename << endl;
    return songs;
}

vector<Song> DatasetLoader::generateSynthetic(size_t n) {
    vector<Song> v;
    v.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        Song s;
        s.name = "Synthetic_" + to_string(i);
        s.artist = "Artist_" + to_string(i % 100);
        s.popularity = rand() % 100;
        s.duration_ms = 180000 + (rand() % 120000);
        s.danceability = (rand() % 100) / 100.0;
        s.energy = (rand() % 100) / 100.0;
        v.push_back(s);
    }
    cout << "Generated synthetic dataset of " << n << " songs." << endl;
    return v;
}
