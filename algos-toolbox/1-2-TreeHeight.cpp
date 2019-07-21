#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <vector>
#include <map>

using namespace std;

struct Node {
	int number;
	vector<Node*> children;
};


// Input Format. The first line contains the number of nodes 𝑛. 
// The second line contains 𝑛 integer numbers from −1 to 𝑛 − 1 — parents of nodes. 
// If the 𝑖-th one of them (0 ≤ 𝑖 ≤ 𝑛 − 1) is −1, node 𝑖 is the root, 
// otherwise it’s 0-based index of the parent of 𝑖-th node. 
// It is guaranteed that there is exactly one root. 
// It is guaranteed that the input represents a tree.

Node* create_tree(vector<int> nodes) {
	Node* root = nullptr;
	map<int, Node*> nodes_map;

	auto get_or_create_node = [&nodes_map](int n) {
		if (nodes_map.find(n) == nodes_map.end()) {
			auto new_node = new Node{n, {}};	
			nodes_map[n] = new_node;
			return new_node;
		} else {
			return nodes_map[n];
		}		
	};

	for (uint i = 0; i < nodes.size(); ++i) {
		Node *ith_node = get_or_create_node(i);
		if (nodes[i] == -1) {
			root = ith_node;
		} else {
			Node *ith_node_parent = get_or_create_node(nodes[i]);
			ith_node_parent->children.emplace_back(ith_node);
		}
	}

	return root;
}

int tree_height(Node* root) {
	if (root == nullptr) {
		return 0;
	}
	if (root->children.empty()) {
		return 1;
	}
	vector<uint> children_heights(root->children.size());
	for (uint i = 0; i < root->children.size(); ++i) {
		children_heights[i] = tree_height(root->children[i]);
	}
	return 1 + *std::max_element(children_heights.begin(), children_heights.end());
}


int main() {
	int n;
	cin >> n;

	vector<int> nodes(n);
	for (auto &n : nodes) {
		cin >> n;
	}

	auto tree_root = create_tree(std::move(nodes));

	cout << tree_height(tree_root) << endl;

}