#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <vector>
#include <cassert>

using namespace std;

size_t parent(size_t i) {
  return i / 2;
}

size_t left_child(size_t i) {
  return i * 2 + 1;
}

size_t right_child(size_t i) {
  return i * 2 + 2;
}

void sift_up(vector<uint>& heap, size_t pos, uint& out_swaps_count) {
  assert(heap.size() > 0);
  // take last element and propagate it up
  for (size_t i = pos; i > 0 && heap[parent(i)] > heap[i]; i = parent(i)) {
    std::swap(heap[i], heap[parent(i)]);
    printf("swap %d (%d) and %d (%d)\n", i, heap[i], parent(i),
           heap[parent(i)]);
    out_swaps_count++;
  }
}

void sift_down(vector<uint>& heap,
               size_t index,
               vector<pair<size_t, size_t>>& r_swaps) {
  size_t i = index;
  while (i < heap.size()) {
    size_t min_index = i;

    // cout << "i: " << i << endl;
    auto l = left_child(i);
    if (l < heap.size() && heap[l] < heap[min_index]) {
      min_index = l;
    }

    auto r = right_child(i);
    if (r < heap.size() && heap[r] < heap[min_index]) {
      min_index = r;
    }

    if (i != min_index) {
      std::swap(heap[min_index], heap[i]);
      r_swaps.emplace_back(i, min_index);
      i = min_index;
    } else {
      break;
    }
  }
}

void heap_insert(vector<uint>& heap, uint x, uint& out_swaps_count) {
  // place to leftmost free cell, sift up
  heap.resize(heap.size() + 1);
  heap[heap.size() - 1] = x;
  sift_up(heap, heap.size() - 1, out_swaps_count);
}

vector<pair<size_t, size_t>> build_heap_from_array(vector<uint> data) {
  vector<pair<size_t, size_t>> swaps;
  for (size_t i = data.size() / 2; i > 0; i--) {
    sift_down(data, i - 1, swaps);
  }
  return swaps;
}

int main() {
  uint n;
  cin >> n;

  vector<uint> data(n);
  for (auto& x : data) {
    cin >> x;
  }

  auto swaps = build_heap_from_array(std::move(data));
  cout << swaps.size() << endl;
  
  for (auto& swap : swaps) {
    cout << swap.first << " " << swap.second << endl;
  }
  cout << endl;
}
	