#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <tuple>
#include <vector>

using namespace std;

struct DisjoinSet {
  DisjoinSet(size_t max_size)
      : rank(max_size), cardinality(max_size), parent(max_size, -1),
        max_cardinality_(0) {}

  void add(uint x, uint count) {
    assert(x < this->parent.size());
    this->parent[x] = x;
    this->cardinality[x] = count;
    this->max_cardinality_ =
        std::max(this->max_cardinality_, this->cardinality[x]);
  }

  void unite(uint x, uint y) {
    if (x == y) {
      return;
    }

    auto idx = find(x), idy = find(y);
    if (idx == idy) {
      return;
    }

    // union-by-rank heuristics: attach shorter to longer.
    if (this->rank[idx] > this->rank[idy]) {
      std::swap(idx, idy);
    }
    assert(this->rank[idx] <= this->rank[idy]);

    // attach x to y.
    this->parent[idx] = idy;

    if (this->rank[idx] == this->rank[idy]) {
      this->rank[idy] += 1;
    }

    // y cardinality now should increase
    this->cardinality[idy] += this->cardinality[idx];

    this->max_cardinality_ =
        std::max(this->max_cardinality_, this->cardinality[idy]);
  }

  uint max_cardinality() const { return this->max_cardinality_; }

  uint find(uint x) const {
    while (x != this->parent[x]) {
      x = this->parent[x];
    }
    return x;
  }

  vector<uint> rank;
  vector<uint> cardinality;
  vector<int> parent;
  uint max_cardinality_;
};

vector<uint> merge_tables(vector<uint> rows,
                          vector<pair<uint, uint>> merge_queries) {
  vector<uint> query_max_count;
  // initially, all tables has some data (rows[i] denotes to number of rows in
  // table i). Next, when we asked to merge data from source to destination,
  // we just union source and dastination disjoin sets.

  DisjoinSet ds{rows.size()};
  for (uint i = 0; i < rows.size(); ++i) {
    ds.add(i, rows[i]);
  }

  for (auto source_dest : merge_queries) {
    ds.unite(source_dest.first - 1, source_dest.second - 1);
    query_max_count.emplace_back(ds.max_cardinality());
  }

  return query_max_count;
}

int main() {
  int n; // number of tables
  cin >> n;
  int m; // number of merge queries
  cin >> m;

  vector<uint> rows(n);
  for (auto &r : rows) {
    cin >> r;
  }

  vector<pair<uint, uint>> merge_queries(m);
  for (auto &x : merge_queries) {
    cin >> x.first;
    cin >> x.second;
  }

  for (auto x : merge_tables(rows, merge_queries)) {
    cout << x << " ";
  }
  cout << endl;
}
