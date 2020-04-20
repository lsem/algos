//
// Max-flow
// (c) Libomyr Semkiv, 2020
//
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;

struct Edge {
  Edge(int y, int capacity)
      : y(y), flow(0), capacity(capacity), residual_capacity(capacity) {}
  int y;  // vertex this edge connected to
  int flow;
  int capacity;
  int residual_capacity;
  Edge* back_edge_ptr = nullptr;
};

// Represents flow graph (residual graph).
struct Graph {
  unsigned size() const { return this->edges.size(); }

  // edges[i] keeps all edges that start from vertex i.
  vector<list<Edge>> edges;

  Graph(int vertices_total_count) : edges(vertices_total_count, list<Edge>{}) {}

  // connects vertex vx to vy with capacity c.
  Edge* connect(int vx, int vy, int c) {
    // cout << "g.edges.size(): " << edges.size() << "\n";
    assert(vx < this->edges.size() && vy < this->edges.size());
    this->edges[vx].emplace_back(vy, c);
    return &this->edges[vx].back();
  }

  void export_to_dot();
};

// Find a path in residual graph using BFS, effectively implementing
// Rabin-Karp variation. we are looking for a path from vertex 0 to vertex
// g.edge.size()
tuple<vector<int>, vector<Edge*>> find_augmenting_path(Graph& g,
                                                       int source,
                                                       int target) {
  vector<bool> discovered_nodes(g.size(), false);
  assert(!discovered_nodes[source]);
  vector<int> parents(g.size(), -1);
  vector<Edge*> edges(g.size(), nullptr);
  list<int> queue;
  queue.push_back(source);
  discovered_nodes[source] = true;
  while (!queue.empty()) {
    auto current_v = queue.front();
    queue.pop_front();
    if (current_v == target) {
      // reached the goal, retrace solution and return.
      vector<int> path;
      for (int vi = target; vi != source; vi = parents[vi]) {
        path.emplace_back(vi);
      }
      path.push_back(source);
      std::reverse(path.begin(), path.end());
      return make_tuple<vector<int>, vector<Edge*>>(std::move(path),
                                                    std::move(edges));
    }
    for (auto& e : g.edges[current_v]) {
      if (discovered_nodes[e.y]) {
        continue;  // skip already discovered vertices
      }
      if (e.residual_capacity == 0) {
        continue;  // skip saturated edges
      }
      discovered_nodes[e.y] = true;
      queue.push_back(e.y);
      parents[e.y] = current_v;
      // save piece of info that one must follow given edge to reach current
      // node.
      edges[e.y] = &e;
    }
  }
  // No augmenting path anymore.
  return make_tuple<vector<int>, vector<Edge*>>({}, {});
}

void find_max_flow(Graph& g) {
  // algorithm is like this.
  // having current flow F.
  // we generate residual graph Gf for current G and f.
  // try find a path in Gf.
  // increase current flow by found path
  // otherwise, end, resulting flow is maximum possible and corresponds to
  // mincut.
  while (true) {
    auto path_and_edges = find_augmenting_path(g, 0, g.edges.size() - 1);
    auto& path = std::get<0>(path_and_edges);
    auto& edges = std::get<1>(path_and_edges);
    // auto [path, edges] = find_augmenting_path(g, 0, g.edges.size() - 1);
    if (path.empty()) {
      // currently assigned flow to target node should be a minimum cat now.
      // find all edges to target: 
      unsigned max_flow = 0;
      for (unsigned v = 0; v < g.size() - 1; ++v) {
        for (auto& e : g.edges[v]) {
          if (e.y == g.size() - 1) {
            max_flow += e.flow;
          }
        }
      }
      cout << max_flow << "\n";
      break;
    } else {
      // assigning flow, but first find what is the minimum.
      unsigned min_residual_cap = std::numeric_limits<unsigned>::max();
      for (size_t i = 0; i < path.size() - 1; ++i) {
        auto edge_ptr = edges[path[i + 1]];
        assert(edge_ptr && "edge must exist");
        if (edge_ptr->residual_capacity < min_residual_cap) {
          min_residual_cap = edge_ptr->residual_capacity;
        }
      }
      // increase flow by min_residual_cap for each edge on the path.
      for (size_t i = 0; i < path.size() - 1; ++i) {
        auto edge_ptr = edges[path[i + 1]];
        assert(edge_ptr && "edge must exist");
        edge_ptr->flow += min_residual_cap;
        edge_ptr->residual_capacity -= min_residual_cap;

        if (edge_ptr->back_edge_ptr) {
          edge_ptr->back_edge_ptr->residual_capacity += min_residual_cap;
        }
      }
    }
  }
}

int main() {
  int cities_n, roads_m;
  cin >> cities_n;
  cin >> roads_m;
  Graph g{cities_n};
  for (unsigned road_num = 0; road_num < roads_m; ++road_num) {
    int road_from, road_to, road_cap;
    cin >> road_from;
    cin >> road_to;
    cin >> road_cap;
    auto forward_edge = g.connect(road_from - 1, road_to - 1, road_cap);
    auto backward_edge = g.connect(road_to - 1, road_from - 1,
                                   0);  // build/init edges for residual graph.
    forward_edge->back_edge_ptr = backward_edge;
  }
  g.export_to_dot();
  find_max_flow(g);
}

void Graph::export_to_dot() {
  auto& g = *this;
  stringstream ss;
  ss << "digraph doo {\n";
  for (unsigned road_num = 0; road_num < g.edges.size(); ++road_num) {
    for (auto e : g.edges.at(road_num)) {
      ss << "\t" << (road_num + 1) << " -> " << (e.y + 1)
         << " [weight=" << e.capacity << "];\n";
    }
  }
  ss << "}";
  if (auto fs = std::ofstream("graph.gv")) {
    fs << ss.str();
  } else {
    throw std::runtime_error("Cannot open file\n");
  }
}
