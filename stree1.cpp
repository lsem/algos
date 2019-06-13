#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

using uint64 = uint64_t;
using namespace std;

static int nodesRemoved = 0;
static int nodesCreated = 0;

template <typename T> struct PoolAllocator {
private:
  T *m_head = nullptr, *m_tail = nullptr;
  std::size_t m_blockSize = 0;

public:
  PoolAllocator(uint64 blockSize) : m_blockSize(blockSize) { allocateBlock(); }
  T *alloc() {
    if (m_head == m_tail) {
      allocateBlock();
    }
    return m_head == m_tail ? nullptr : m_head++;
  }

private:
  void allocateBlock() {
    auto allocate_start = std::chrono::high_resolution_clock::now();
    m_head = m_tail =
        reinterpret_cast<T *>(std::malloc(sizeof(T) * m_blockSize));
    auto allocate_end = std::chrono::high_resolution_clock::now();

    auto allocate_duration =
        std::chrono::duration_cast<std::chrono::microseconds>(allocate_end -
                                                              allocate_start);

    std::cout << "allocate_duration: " << allocate_duration.count()
              << std::endl;

    m_tail += m_blockSize;
  }
};

struct Node;

struct Edge {
  uint16_t begin, end;
  Node *node = nullptr;

  bool isEmpty() const { return node == nullptr; }
};

enum ESYMBOLS { A, C, T, G, DOLLAR, ESYMBOLS_MAX };

unsigned decode_symbol_to_edge_id(int s) {
  switch (s) {
  case 'A':
    return A;
  case 'C':
    return C;
  case 'T':
    return T;
  case 'G':
    return G;
  case '$':
    return DOLLAR;
  }
  return -1;
}

struct Node {
  using Iter = vector<Edge>::const_iterator;
  int nodeNum = 0;
  // vector<Edge> edges;
  Edge edges[ESYMBOLS_MAX];

  const Edge *get_first_nonempty_edge() const {
    for (unsigned i = 0; i != ESYMBOLS_MAX; ++i) {
      if (!edges[i].isEmpty())
        return &edges[i];
    }
    return nullptr;
  }
  unsigned edges_count() const {
    unsigned count = 0;
    for (unsigned i = 0; i != ESYMBOLS_MAX; ++i) {
      if (!edges[i].isEmpty())
        count++;
    }
    return count;
  }

  const Edge &find_edge(int c) const {
    return edges[decode_symbol_to_edge_id(c)];
  }

  void compress() {
    // for (auto &edge : edges) {
    for (unsigned edgeId = 0; edgeId != ESYMBOLS_MAX; edgeId++) {
      if (edges[edgeId].isEmpty())
        continue;
      auto &edge = edges[edgeId];
      while (edge.node->edges_count() == 1) {
        // while (edge.node->edges.size() == 1) {
        edge.end++; // take its symbol
        // edge.node = std::move(edge.node->edges[0].node);
        // edge.node = edge.node->edges[0].node;
        edge.node = edge.node->get_first_nonempty_edge()->node;
      }
      // nothing to collapse anymore
      edge.node->compress();
    }
  }
}; // struct Node

static PoolAllocator<Node> NodesPool{1024 * 1024 * 16};

namespace {

Node *createNode() { return NodesPool.alloc(); }

} // anonymous namespace

void print(Node *node, const std::string &text) {
  if (!node)
    return;
  for (unsigned edgeId = 0; edgeId != ESYMBOLS_MAX; edgeId++) {
    if (node->edges[edgeId].isEmpty())
      continue;
    auto &e = node->edges[edgeId];
    fwrite(&text[0] + e.begin, sizeof(char), e.end - e.begin, stdout);
    putc('\n', stdout);
    print(e.node, text);
  }
}

struct STree {
  unique_ptr<Node> root{new Node()};

  void compress();
  void print(const std::string &text) { ::print(root.get(), text); }
};

void add_string(STree &stree, const string &text, size_t offset) {
  Node *currentNode = stree.root.get();
  for (size_t i = offset; i < text.size(); ++i) {
    char c = text[i];
    auto &edge = currentNode->find_edge(c);
    if (!edge.isEmpty()) {
      currentNode = edge.node;
    } else {
      auto newNode = createNode();
      auto newEdge = Edge{};
      newEdge.node = std::move(newNode);
      newEdge.begin = i;
      newEdge.end = i + 1;
      auto nextNode = newEdge.node;
      currentNode->edges[decode_symbol_to_edge_id(c)] = std::move(newEdge);
      currentNode = nextNode;
    }
  }
}

void STree::compress() { this->root->compress(); }

STree build_stree(const string &text) {
  STree stree;

  auto build_start = std::chrono::high_resolution_clock::now();
  for (size_t i = 0; i != text.size(); ++i) {
    add_string(stree, text, i);
  }
  auto build_finish = std::chrono::high_resolution_clock::now();

  auto compress_start = std::chrono::high_resolution_clock::now();
  stree.compress();
  auto compress_finish = std::chrono::high_resolution_clock::now();

  auto print_start = std::chrono::high_resolution_clock::now();
  // stree.print(text);
  auto print_finish = std::chrono::high_resolution_clock::now();

  auto build_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      build_finish - build_start);
  auto compress_duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(compress_finish -
                                                            compress_start);
  auto print_duration = std::chrono::duration_cast<std::chrono::milliseconds>(
      print_finish - print_start);

  std::cout << "build_duration: " << build_duration.count() << std::endl;
  std::cout << "compress_duration: " << compress_duration.count() << std::endl;
  std::cout << "print_duration: " << print_duration.count() << std::endl;

  return std::move(stree);
}

int main() {
  std::string text;
  std::cin >> text;

  build_stree(text);
}