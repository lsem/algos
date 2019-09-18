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

struct node_t {
  int key, left, right;
};

bool is_bst(const vector<node_t>& nodes,
            size_t root,
            int& out_max,
            int& out_min,
            bool& out_max_valid) {
  if (root == -1) {
    out_max_valid = false;
    return true;
  }

  int left_max, right_max, left_min, right_min;
  bool left_max_valid, right_max_valid;
  if (!is_bst(nodes, nodes[root].left, left_max, left_min, left_max_valid)) {
    return false;
  } else if (!is_bst(nodes, nodes[root].right, right_max, right_min,
                     right_max_valid)) {
    return false;
  }

  if (left_max_valid && nodes[root].key < left_max) {
    return false;
  }
  if (right_max_valid && nodes[root].key > right_min) {
    return false;
  }

  out_min = nodes[root].key;
  out_max = nodes[root].key;
  if (left_max_valid) {
    out_min = std::min(out_min, left_min);
    out_max = std::max(out_max, left_max);
  }
  if (right_max_valid) {
    out_min = std::min(out_min, right_min);
    out_max = std::max(out_max, right_max);
  }

  out_max_valid = true;

  return true;
}

int main() {
  uint n;
  cin >> n;

  if (n == 0) {
    cout << "CORRECT" << endl;
    return 0;
  }

  vector<node_t> nodes(n);

  for (auto& n : nodes) {
    cin >> n.key;
    cin >> n.left;
    cin >> n.right;
  }

  int bstmax = -1, bstmin = -1;
  bool max_valid;
  if (is_bst(nodes, 0, bstmax, bstmin, max_valid)) {
    cout << "CORRECT" << endl;
  } else {
    cout << "INCORRECT" << endl;
  }
}