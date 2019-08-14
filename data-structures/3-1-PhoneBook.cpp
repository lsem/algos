#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;

namespace {
uint calc_hash(const string& s) {
  uint hash = 0;
  for (uint i = 0; i < s.size(); ++i) {
    hash = (hash * 31 + s[i]) % 2147483647;
  }
  return hash;
}
}  // namespace

struct ChainNode {
  ChainNode(string number, string name, unique_ptr<ChainNode> next = nullptr)
      : number(number), name(name), next_node(std::move(next)) {}

  string number, name;
  unique_ptr<ChainNode> next_node;
};

class PhoneBook {
 public:
  PhoneBook() : hash_table(50000) {}

  void add(const string& number, const string& name) {
    size_t bucket = calc_hash(number) % this->hash_table.size();

    // handle update case
    for (auto* link = this->hash_table[bucket].get(); link != nullptr;
         link = link->next_node.get()) {
      if (link->number == number) {
        link->name = name;
        return;
      }
    }

    if (!this->hash_table[bucket]) {
      hash_table[bucket] = std::make_unique<ChainNode>(number, name);
    } else {
      auto existing_chain = std::move(this->hash_table[bucket]);
      this->hash_table[bucket] =
          std::make_unique<ChainNode>(number, name, std::move(existing_chain));
    }
  }

  void del(const string& number) {
    size_t bucket = calc_hash(number) % this->hash_table.size();
    if (!this->hash_table[bucket]) {
      return;
    } else if (this->hash_table[bucket]->number == number) {
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
      if (link->number == number) {
        auto removed_node = std::move(prev_link->next_node);
        prev_link->next_node = std::move(removed_node->next_node);
        return;
      }
      prev_link = link;
    }
  }

  bool find(const string& number, string& out_name) {
    size_t bucket = calc_hash(number) % this->hash_table.size();
    if (!this->hash_table[bucket]) {
      return false;
    }
    for (auto* link = this->hash_table[bucket].get(); link != nullptr;
         link = link->next_node.get()) {
      if (link->number == number) {
        out_name = link->name;
        return true;
      }
    }
    return false;
  }

 private:
  vector<unique_ptr<ChainNode>> hash_table;
};

vector<string> process_phonebook_queries(vector<string> input) {
  vector<string> answers;

  PhoneBook phonebook;

  for (size_t i = 0; i < input.size();) {
    auto& q = input[i];
    if (q == "add") {
      auto& name = input[++i];
      auto& number = input[++i];
      phonebook.add(name, number);
    } else if (q == "del") {
      auto& number = input[++i];
      phonebook.del(number);
    } else if (q == "find") {
      auto& number = input[++i];
      string name;
      if (phonebook.find(number, name)) {
        answers.emplace_back(name);
      } else {
        answers.emplace_back("not found");
      }
    }

    i += 1;
  }

  return answers;
}

class PhoneBookWithCheating {
 public:
  bool find(const string& number, string& out_name) {
    if (this->numbers_to_names.find(number) != this->numbers_to_names.end()) {
      out_name = this->numbers_to_names[number];
      return true;
    }
    return false;
  }

  void del(const string& number) { this->numbers_to_names.erase(number); }

  void add(const string& number, const string& name) {
    this->numbers_to_names[number] = name;
  }

  unordered_map<string, string> numbers_to_names;
};


int main() {
  uint n;
  cin >> n;

  vector<string> queries;
  for (uint i = 0; i < n; ++i) {
    string q;
    cin >> q;
    queries.emplace_back(q);
    if (q == "add") {
      string arg;
      cin >> arg;
      queries.emplace_back(arg);
      cin >> arg;
      queries.emplace_back(arg);
    } else if (q == "find") {
      string arg;
      cin >> arg;
      queries.emplace_back(arg);
    } else if (q == "del") {
      string arg;
      cin >> arg;
      queries.emplace_back(arg);
    }
  }

  for (auto answer : process_phonebook_queries(std::move(queries))) {
    cout << answer << endl;
  }
}
