//
// Created by bgoco on 10/28/2025.
//
#ifndef MAXHEAP_H
#define MAXHEAP_H

#include <vector>
#include "Song.h"
using namespace std;

class MaxHeap {
private:
    vector<Song> heap;
    SortMode heapMode;  // which metric this heap ranks by
    void heapifyUp(int index);
    void heapifyDown(int index);

public:
    MaxHeap() = default;
    explicit MaxHeap(SortMode mode) : heapMode(mode) {}

    void insert(const Song &song);
    Song extractMax();
    bool isEmpty() const;
    int size() const;

    // Non-destructive peek: returns top `count` elements in order (largest first)
    vector<Song> peekTop(int count) const;

    // Convenience debugging print
    void printTopN(int n) const;
};

#endif // MAXHEAP_H
