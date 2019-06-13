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
    m_head = m_tail =
        reinterpret_cast<T *>(std::malloc(sizeof(T) * m_blockSize));
    m_tail += m_blockSize;
  }
};

struct Node;

struct Edge {
  Edge(Node *node, uint16_t begin, uint16_t end)
      : node(node), edge_num(s_edge_num), begin(begin), end(end) {
    s_edge_num++;
  }

  int len() const { return end - begin; }
  uint16_t begin = 0, end = 0;
  Node *node = nullptr;
  int edge_num;
  static int s_edge_num;
};
int Edge::s_edge_num = 0;

string to_string(const std::string &text, const Edge &edge) {
  return string(text, edge.begin, edge.end - edge.begin);
}

struct Match {
  Edge *edge;
  uint16_t mismatch_pos;

  Match() : edge(nullptr), mismatch_pos(0) {}
  Match(Edge *edge, uint16_t mismatch_pos)
      : edge(edge), mismatch_pos(mismatch_pos) {}

  operator bool() const { return edge != nullptr; }

  bool operator>(const Match &rhs) const {
    return mismatch_pos > rhs.mismatch_pos;
  }
};

struct Node {
  vector<Edge> edges;
  int node_num;
  static int s_node_num;

  Match find_max_edge_match(const std::string &text, uint16_t begin,
                            uint16_t end) {
    Match max_match;

    for (auto it = edges.begin(); it != edges.end(); ++it) {
      auto &edge = *it;
      // match input against edge label
      size_t i;
      size_t text_len = end - begin;
      for (i = 0; i < text_len && i < edge.len(); ++i) {
        if (text[begin + i] != text[edge.begin + i]) {
          break;
        }
      }
      Match current_match{&edge, (uint16_t)i};
      if (current_match > max_match) {
        max_match = current_match;
      }
    }
    return max_match;
  }

  static Node *get_node();
};

static PoolAllocator<Node> NodesPool{10000};

/*static*/ Node *Node::get_node() {
  auto n = NodesPool.alloc();
  n->node_num = s_node_num;
  s_node_num++;
  return n;
}

string to_string(const string &text, const Node &node) {
  string result = "[" + to_string(node.node_num) + "]:\n";
  for (auto &e : node.edges) {
    result += to_string(text, e) + "\n";
  }

  if (!node.edges.empty()) {
    result.resize(result.size() - 1);
  } else {
    result.resize(result.size() - 1);
    result += "{}";
  }
  return result;
}

struct STree {
  Node *root = Node::get_node();
};

// TODO: consider https://stackoverflow.com/questions/36311991/c-sharp-display-a-binary-search-tree-in-console/36313190#36313190
void pretty_print_node(Node *node, Edge *origin_edge, const string &text,
                         string indent, bool last) {
  std::cout << indent;
  if (last) {
    std::cout << "\\-";
    indent += "    ";
  } else {
    std::cout << "|-";
    indent += "|   ";
  }
  string edge_label;
  if (origin_edge != nullptr) {
    edge_label = "->" + to_string(text, *origin_edge) + "->";
  }
  std::cout << edge_label << "[" << node->node_num << "]\n";
  for (int i = 0; i < node->edges.size(); ++i) {
    pretty_print_node(node->edges[i].node, &node->edges[i], text, indent,
                        i == (node->edges.size() - 1));
  }
}

void pretty_print(STree &stree, const std::string &text) {
  pretty_print_node(stree.root, nullptr, text, "", true);
}

int Node::s_node_num = 0;

void print(Node *node, const std::string &text) {
  if (!node)
    return;
  for (auto &e : node->edges) {
    fwrite(&text[0] + e.begin, sizeof(char), e.len(), stdout);
    putc('\n', stdout);
    print(e.node, text);
  }
}

void add_string(STree &stree, const string &text, size_t offset) {

  auto *currentNode = stree.root;

  auto i = (uint16_t)offset;

  while (true) {

    auto match = currentNode->find_max_edge_match(text, i, text.size());
    if (!match) {
      auto newEdge = Edge{Node::get_node(), i, (uint16_t)text.size()};
      currentNode->edges.push_back(newEdge);
      break;
    }

    auto m = match.mismatch_pos;

    if ((match.edge->begin + m) < match.edge->end) {
      // Matched not entire node but just prefix, than split it to prefix:suffix
      // giving new suffix edge existing node while directing matched edge to
      // prefix node.
      auto newEdge = Edge{match.edge->node, (uint16_t)(match.edge->begin + m),
                          match.edge->end};
      match.edge->node = Node::get_node();
      // Trim existing edge
      match.edge->end = match.edge->begin + m;
      // attach new edge to node of matched edge
      match.edge->node->edges.push_back(newEdge);
    } else {
      // if matched all characters from edge, than just follow that node
      // skipping matched input text.
    }
    currentNode = match.edge->node;
    i += m;
  }
}

STree build_stree(const std::string &text) {
  STree stree;

  for (size_t i = 0; i != text.size(); ++i) {
    add_string(stree, text, i);
  }
  return std::move(stree);
}

int main(int argc, const char *argv[]) {
  std::string text;
  std::cin >> text;

  auto stree = build_stree(text);

  if (argc > 1 && string(argv[1]) == "tree") {
    pretty_print(stree, text);
  }
  else {
    print(stree.root, text);
  }
}