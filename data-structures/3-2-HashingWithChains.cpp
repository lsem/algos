#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;

uint64_t calc_hash(const string& s) {
  uint64_t hash = 0;
  uint64_t x = 1;
  for (uint i = 0; i < s.size(); ++i) {
    hash = (hash + s[i] * x) % 1000000007;
    x = x * 263 % 1000000007;
  }
  return hash;
}

struct ChainNode {
  ChainNode(string key, unique_ptr<ChainNode> next = nullptr)
      : key(std::move(key)), next_node(std::move(next)) {}

  string key;
  unique_ptr<ChainNode> next_node;
};

struct HashSet {
  HashSet(unsigned buckets_num) : hash_table(buckets_num) {}

  void add(const string& key) {
    size_t bucket = calc_hash(key) % this->hash_table.size();

    // handle update case
    for (auto* link = this->hash_table[bucket].get(); link != nullptr;
         link = link->next_node.get()) {
      if (link->key == key) {
        // ignore this query
        return;
      }
    }

    if (!this->hash_table[bucket]) {
      this->hash_table[bucket] = std::make_unique<ChainNode>(key);
    } else {
      auto existing_chain = std::move(this->hash_table[bucket]);
      this->hash_table[bucket] =
          std::make_unique<ChainNode>(key, std::move(existing_chain));
    }
  }

  void del(const string& key) {
    size_t bucket = calc_hash(key) % this->hash_table.size();
    if (!this->hash_table[bucket]) {
      return;
    } else if (this->hash_table[bucket]->key == key) {
      if (!this->hash_table[bucket]->next_node) {
        this->hash_table[bucket] = nullptr;
      } else {
        auto removed_node = std::move(this->hash_table[bucket]);
        this->hash_table[bucket] = std::move(removed_node->next_node);
      }
      return;
    }

    ChainNode* prev_link = nullptr;
    for (auto* link = this->hash_table[bucket].get(); link != nullptr;
         link = link->next_node.get()) {
      if (link->key == key) {
        auto removed_node = std::move(prev_link->next_node);
        prev_link->next_node = std::move(removed_node->next_node);
        return;
      }
      prev_link = link;
    }
  }

  bool find(const string& key) {
    size_t bucket = calc_hash(key) % this->hash_table.size();
    if (!this->hash_table[bucket]) {
      return false;
    }
    for (auto* link = this->hash_table[bucket].get(); link != nullptr;
         link = link->next_node.get()) {
      if (link->key == key) {
        return true;
      }
    }
    return false;
  }

  vector<string> fetch_bucket(size_t bucket) {
    vector<string> bucket_data;
    if (this->hash_table[bucket]) {
      for (auto* link = this->hash_table[bucket].get(); link != nullptr;
           link = link->next_node.get()) {
        bucket_data.emplace_back(link->key);
      }
    }
    return bucket_data;
  }

  vector<unique_ptr<ChainNode>> hash_table;
};

int main() {
  assert(4 == calc_hash("world") % 5);
  assert(4 == calc_hash("HellO") % 5);
  assert(2 == calc_hash("GooD") % 5);
  assert(2 == calc_hash("luck") % 5);

  uint m;
  cin >> m;

  uint n;
  cin >> n;

  HashSet hash_table(m);

  stringstream out_stream;

  for (uint i = 0; i < n; ++i) {
    string q;
    cin >> q;
    if (q == "add") {
      string arg;
      cin >> arg;
      hash_table.add(arg);
    } else if (q == "find") {
      string arg;
      cin >> arg;
      if (hash_table.find(arg)) {
        out_stream << "yes" << endl;
      } else {
        out_stream << "no" << endl;
      }
    } else if (q == "del") {
      string arg;
      cin >> arg;
      hash_table.del(arg);
    } else if (q == "check") {
      size_t bucket_num;
      cin >> bucket_num;
      string sep;
      for (auto& b : hash_table.fetch_bucket(bucket_num)) {
        out_stream << sep << b;
        sep = " ";
      }
      out_stream << endl;
    }
  }

  cout << out_stream.str() << endl;
}