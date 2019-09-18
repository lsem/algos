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

void preorder_traversal(const vector<node_t>& nodes, size_t root) {
  // visit self, than children
  if (root == -1) {
    return;
  }
  cout << nodes[root].key << " ";
  preorder_traversal(nodes, nodes[root].left);
  preorder_traversal(nodes, nodes[root].right);
}

void inorder_traversal(const vector<node_t>& nodes, size_t root) {
  if (root == -1) {
    return;
  }
  inorder_traversal(nodes, nodes[root].left);
  cout << nodes[root].key << " ";
  inorder_traversal(nodes, nodes[root].right);
}

void postorder_traversal(const vector<node_t>& nodes, size_t root) {
  if (root == -1) {
    return;
  }
  postorder_traversal(nodes, nodes[root].left);
  postorder_traversal(nodes, nodes[root].right);
  cout << nodes[root].key << " ";
}

int main() {
  uint n;
  cin >> n;

  vector<node_t> nodes(n);

  for (auto& n : nodes) {
    cin >> n.key;
    cin >> n.left;
    cin >> n.right;
  }

  inorder_traversal(nodes, 0);
  cout << endl;
  preorder_traversal(nodes, 0);
  cout << endl;
  postorder_traversal(nodes, 0);
  cout << endl;
}
