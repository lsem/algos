//
// Max-flow, Assigning Crew to Flights, Bipartite Matching
// (c) Libomyr Semkiv, 2020
//
// Task:
// The airline offers a bunch of flights and has a set of crews that can
// work on those flights. However, the flights are starting in different cities
// and at different times, so only some of the crews are able to work on a
// particular flight. You are given the pairs of flights and associated crews
// that can work on those flights. You need to assign crews to as many flights
// as possible and output all the assignments.
//
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <map>
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

  void connect_with_residual(int vx, int vy, int c) {
    auto main = this->connect(vx, vy, c);
    main->back_edge_ptr = this->connect(vy, vx, 0);
  }
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

void find_max_flow(Graph& g,
                   const unsigned CREWS_START,
                   const unsigned CREWS_END,
                   const unsigned FLIGHTS_START,
                   const unsigned FLIGHTS_END,
                   const unsigned SINK) {
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
      // Maximum flow has been assigned.
      // For this problem we need not a maximum flow but actual assignments
      // for each flight.
      // which can be found by enumerating all crews:
      map<unsigned, int> flight_assignments;
      for (auto crew = CREWS_START; crew < CREWS_END; ++crew) {
        // among all the crew's edges select one that has flow assigned.
        for (auto& e : g.edges[crew]) {
          if (e.flow > 0) {
            flight_assignments[(e.y)] = (crew);
            // cout << "found flow of " << e.flow << " from crew "
            //      << (crew - CREWS_START) << " to flight "
            //      << (e.y - FLIGHTS_START) << "\n";
          }
        }
      }
      string space = "";
      for (auto flight = FLIGHTS_START; flight != FLIGHTS_END; ++flight) {
        if (flight_assignments.count(flight) > 0) {
          cout << space << flight_assignments[flight];
        } else {
          cout << space << -1;
        }
        space = " ";
      }
      cout << "\n";

      // // currently assigned flow to target node should be a minimum cat now.
      // // find all edges to target: 
      // unsigned max_flow = 0;
      // for (unsigned v = 0; v < g.size() - 1; ++v) {
      //   for (auto& e : g.edges[v]) {
      //     if (e.y == g.size() - 1) {
      //       max_flow += e.flow;
      //     }
      //   }
      // }
      // cout << "max flow: " << max_flow << "\n";
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
  // The first line of the input contains integers 𝑛 and 𝑚 — the number of
  // flights and the number of crews respectively.int flights_n, crews_n;
  int flights_n, crews_n;
  cin >> flights_n;
  cin >> crews_n;
  // Each of the next 𝑛 lines contains 𝑚 binary integers (0 or 1). If the 𝑗-th
  // integer in the 𝑖 - th line is 1, then the crew number 𝑗 can work on the
  // flight number 𝑖, and if it is 0, then it cannot.

  unsigned SOURCE = 0;
  unsigned CREWS_START = 1;
  unsigned CREWS_END = CREWS_START + crews_n;
  unsigned FLIGHTS_START = CREWS_END;
  unsigned FLIGHTS_END = FLIGHTS_START + flights_n;
  unsigned SINK = FLIGHTS_END;

  Graph g{flights_n + crews_n + 2};  // +2 because we need source and sink.

  // connect source to all crews
  for (unsigned crew = CREWS_START; crew < (CREWS_START + crews_n); ++crew) {
    g.connect_with_residual(SOURCE, crew, 1);
  }

  // connect crews to flights
  for (unsigned flight = FLIGHTS_START; flight < (FLIGHTS_START + flights_n);
       ++flight) {
    for (unsigned crew = CREWS_START; crew < (CREWS_START + crews_n); ++crew) {
      unsigned crew_can_work_on_flight;
      cin >> crew_can_work_on_flight;
      if (crew_can_work_on_flight == 1) {
        g.connect_with_residual(crew, flight, 1);
      }
    }
  }

  // connect flights to sink
  for (unsigned flight = FLIGHTS_START; flight < (FLIGHTS_START + flights_n);
       ++flight) {
    g.connect_with_residual(flight, SINK, 1);
  }

  find_max_flow(g, CREWS_START, CREWS_END, FLIGHTS_START, FLIGHTS_END, SINK);
}
