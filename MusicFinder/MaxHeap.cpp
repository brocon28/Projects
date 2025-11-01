//
// Created by bgoco on 10/28/2025.
//
#include "MaxHeap.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>
using namespace std;

// Helper to truncate long strings (for printTopN)
static string truncate(const string &s, size_t width) {
    if (s.size() <= width) return s;
    return s.substr(0, width - 3) + "...";
}

void MaxHeap::heapifyUp(int index) {
    // Temporarily set global sortMode to match this heap
    SortMode prevMode = Song::sortMode;
    Song::sortMode = heapMode;

    while (index > 0) {
        int parent = (index - 1) / 2;
        if (heap[index] > heap[parent]) {
            swap(heap[index], heap[parent]);
            index = parent;
        } else break;
    }

    Song::sortMode = prevMode; // restore
}

void MaxHeap::heapifyDown(int index) {
    SortMode prevMode = Song::sortMode;
    Song::sortMode = heapMode;

    int n = (int)heap.size();
    while (true) {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        if (left < n && heap[left] > heap[largest]) largest = left;
        if (right < n && heap[right] > heap[largest]) largest = right;

        if (largest != index) {
            swap(heap[index], heap[largest]);
            index = largest;
        } else break;
    }

    Song::sortMode = prevMode;
}

void MaxHeap::insert(const Song &song) {
    heap.push_back(song);
    heapifyUp((int)heap.size() - 1);
}

Song MaxHeap::extractMax() {
    if (heap.empty()) throw runtime_error("MaxHeap::extractMax: heap is empty");

    SortMode prevMode = Song::sortMode;
    Song::sortMode = heapMode;

    Song max = heap[0];
    heap[0] = heap.back();
    heap.pop_back();
    if (!heap.empty()) heapifyDown(0);

    Song::sortMode = prevMode;
    return max;
}

bool MaxHeap::isEmpty() const { return heap.empty(); }

int MaxHeap::size() const { return (int) heap.size(); }

vector<Song> MaxHeap::peekTop(int count) const {
    // Non-destructive: copy heap and extract up to count items
    vector<Song> result;
    if (count <= 0 || heap.empty()) return result;

    // Make a local copy of the heap structure and operate on it
    MaxHeap tmp = *this; // copies heap and heapMode
    int take = min(count, tmp.size());
    result.reserve(take);
    for (int i = 0; i < take && !tmp.isEmpty(); ++i) {
        result.push_back(tmp.extractMax());
    }
    return result;
}

void MaxHeap::printTopN(int n) const {
    if (heap.empty()) {
        cout << "Heap is empty. Load data first.\n";
        return;
    }

    MaxHeap temp = *this;
    cout << left;
    cout << "---------------------------------------------------------------------------------------------------------\n";
    cout << setw(5)  << "Rank"
         << setw(25) << "Song Name"
         << setw(25) << "Artist"
         << setw(12) << "Popularity"
         << setw(12) << "Dance"
         << setw(12) << "Energy"
         << setw(12) << "Duration"
         << endl;
    cout << "---------------------------------------------------------------------------------------------------------\n";

    for (int i = 0; i < n && !temp.isEmpty(); ++i) {
        Song s = temp.extractMax();
        cout << setw(5)  << i + 1
             << setw(25) << truncate(s.name, 24)
             << setw(25) << truncate(s.artist, 24)
             << setw(12) << s.popularity
             << setw(12) << fixed << setprecision(2) << s.danceability
             << setw(12) << fixed << setprecision(2) << s.energy
             << setw(12) << s.duration_ms
             << endl;
    }

    cout << "---------------------------------------------------------------------------------------------------------\n";
}
