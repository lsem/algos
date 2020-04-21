//
// Max-flow, Stock-charts.
// (c) Libomyr Semkiv, 2020
//
// Task:
// You’re in the middle of writing your newspaper’s end-of-year economics
// summary, and you’ve decided that you want to show a number of charts to
// demonstrate how different stocks have performed over the course of the last
// year. You’ve already decided that you want to show the price of 𝑛 different
// stocks, all at the same 𝑘 points of the year. A simple chart of one stock’s
// price would draw lines between the points (0, 𝑝𝑟𝑖𝑐𝑒0),(1, 𝑝𝑟𝑖𝑐𝑒1), . . . ,(𝑘
// − 1, 𝑝𝑟𝑖𝑐𝑒𝑘−1), where 𝑝𝑟𝑖𝑐𝑒𝑖 is the price of the stock at the 𝑖-th point in
// time. In order to save space, you have invented the concept of an overlaid
// chart. An overlaid chart is the combination of one or more simple charts, and
// shows the prices of multiple stocks (simply drawing a line for each one). In
// order to avoid confusion between the stocks shown in a chart, the lines in an
// overlaid chart may not cross or touch.
// Given a list of 𝑛 stocks’ prices at each of 𝑘 time points, determine the
// minimum number of overlaid charts you need to show all of the stocks’ prices.
//
// Input Format:
// The first line of the input contains two integers 𝑛 and 𝑘 — the number of
// stocks and the number of points in the year which are common for all of them.
// Each of the next 𝑛 lines contains 𝑘 integers. The 𝑖-th of those 𝑛 lines
// contains the prices of the 𝑖-th stock at the corresponding 𝑘 points in the
// year
//

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;

// Checks is stock i can be above stock j.
// data[i][j] denotes stock price of stock j for point i.
bool can_be_above(const vector<vector<unsigned>> &data, size_t i, size_t j) {
  for (size_t point = 0; point < data.size(); ++point) {
    if (data[point][i] <= data[point][j]) {
      return false;
    }
  }
  return true;
}

struct Edge {
  Edge(int y, int capacity)
      : y(y), flow(0), capacity(capacity), residual_capacity(capacity) {}
  int y; // vertex this edge connected to
  int flow;
  int capacity;
  int residual_capacity;
  Edge *back_edge_ptr = nullptr;
};

// Represents flow graph (residual graph).
struct Graph {
  unsigned size() const { return this->edges.size(); }

  // edges[i] keeps all edges that start from vertex i.
  vector<list<Edge>> edges;

  Graph(int vertices_total_count) : edges(vertices_total_count, list<Edge>{}) {}

  // connects vertex vx to vy with capacity c.
  Edge *connect(int vx, int vy, int c) {
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

tuple<vector<int>, vector<Edge *>> find_augmenting_path(Graph &g, int source,
                                                        int target) {
  vector<bool> discovered_nodes(g.size(), false);
  assert(!discovered_nodes[source]);
  vector<int> parents(g.size(), -1);
  vector<Edge *> edges(g.size(), nullptr);
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
      return make_tuple<vector<int>, vector<Edge *>>(std::move(path),
                                                     std::move(edges));
    }
    for (auto &e : g.edges[current_v]) {
      if (discovered_nodes[e.y]) {
        continue; // skip already discovered vertices
      }
      if (e.residual_capacity == 0) {
        continue; // skip saturated edges
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
  return make_tuple<vector<int>, vector<Edge *>>({}, {});
}

void find_max_flow(Graph &g) {
  // algorithm is like this.
  // having current flow F.
  // we generate residual graph Gf for current G and f.
  // try find a path in Gf.
  // increase current flow by found path
  // otherwise, end, resulting flow is maximum possible and corresponds to
  // mincut.
  while (true) {
    auto path_and_edges = find_augmenting_path(g, 0, g.edges.size() - 1);
    auto &path = std::get<0>(path_and_edges);
    auto &edges = std::get<1>(path_and_edges);
    // auto [path, edges] = find_augmenting_path(g, 0, g.edges.size() - 1);
    if (path.empty()) {
      // cout << "have a solution!\n";
      return;
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
  unsigned stocks_n, points_in_year_n;
  cin >> stocks_n;
  cin >> points_in_year_n;
  vector<vector<unsigned>> data(points_in_year_n,
                                vector<unsigned>(stocks_n, 0));
  for (unsigned stock = 0; stock < stocks_n; ++stock) {
    for (unsigned point = 0; point < points_in_year_n; ++point) {
      unsigned price; // price of stock [point] at point [point].
      cin >> price;
      data[point][stock] = price;
    }
  }

  Graph g{(int)stocks_n * 2 + 2};
  // we will be finding max-flow between all stocks
  // and other all stocks, plus source and sink.

  unsigned SOURCE = 0;
  unsigned LEFT_ROW_BEGIN = 1;
  unsigned LEFT_ROW_END = LEFT_ROW_BEGIN + stocks_n;
  unsigned RIGHT_ROW_BEGIN = LEFT_ROW_END;
  unsigned RIGHT_ROW_END = RIGHT_ROW_BEGIN + stocks_n;
  unsigned SINK = RIGHT_ROW_END;

  // connect source to left row
  for (unsigned i = LEFT_ROW_BEGIN; i < LEFT_ROW_END; ++i) {
    g.connect_with_residual(SOURCE, i, 1);
  }

  // find crossings for each stock
  for (unsigned stock_i = 0; stock_i < stocks_n; ++stock_i) {
    for (unsigned stock_j = 0; stock_j < stocks_n; ++stock_j) {
      if (can_be_above(data, stock_i, stock_j)) {
        // cout << "stock " << stock_i << " can be above stock " << stock_j
        //      << "\n";
        g.connect_with_residual(LEFT_ROW_BEGIN + stock_i,
                                RIGHT_ROW_BEGIN + stock_j, 1);
      } else {
        // cout << "stock " << stock_i << " cannot be above stock " << stock_j
        //      << "\n";
      }
    }
  }

  // conect right row with sink
  // connect source to left row
  for (unsigned i = RIGHT_ROW_BEGIN; i < RIGHT_ROW_END; ++i) {
    g.connect_with_residual(i, SINK, 1);
  }

  find_max_flow(g);

  // return number of the same vertex on opposite side of bipartite graph.
  auto opposite_side = [&](int vertex) {
    if (vertex >= LEFT_ROW_BEGIN && vertex < LEFT_ROW_END) {
      return (vertex - LEFT_ROW_BEGIN) + RIGHT_ROW_BEGIN;
    } else if (vertex >= RIGHT_ROW_BEGIN && vertex < RIGHT_ROW_END) {
      return (vertex - RIGHT_ROW_BEGIN) + LEFT_ROW_BEGIN;
    }
    throw std::logic_error("opposite_side: Something went wrong: " +
                           to_string(vertex));
  };

  // To find out the answer, we need for each unmatched vertex X (each one
  // without flow assigned) find vertex that has flow assigned to X from right
  // side.

  //                    7 (s)
  //
  //            4         5         6
  //              \
   //                \
   //                  \
   //                    \
   //            1         2         3
  //
  //                      0 (s)
  //
  // On schema above, first unmatched one is (1), which corresponding vertex on
  // right side is (4). So first we find unmatched one (1) and this is a first
  // node in our path, then we find that (2) to points to opposite(1)==4 so we
  // take this (2), next we try to find next matched vertex which has flow to
  // opposote to (2) which is (5). Since we not found anything, this is end of
  // current path {1, 2}. Next unmatched one is (3), we try to find vertex which
  // has positive flow to opposite to (3) which is (6). Since we cannot found
  // anything, {6} is the only vertex. Final solution is:
  //  {1, 2} go to one combined chart (1 can be above 2)
  //  {3} goes to another.

  // trace edges with positive flow from left to right and back.
  list<vector<unsigned>> unswer_paths;
  for (unsigned i = LEFT_ROW_BEGIN; i < LEFT_ROW_END; ++i) {
    const bool is_unmatched =
        std::all_of(g.edges[i].begin(), g.edges[i].end(),
                    [](const Edge &x) { return x.flow == 0; });
    if (is_unmatched) {
      vector<unsigned> asnwer_path; // one of answer paths.
      for (unsigned k = i;;) {
      k_start:
        asnwer_path.push_back(k);
        // find a vertex on left row (the same row as i),
        // which has positive flow to opposite vertex to i on right side.
        const auto oi = opposite_side(k);
        for (unsigned j = LEFT_ROW_BEGIN; j < LEFT_ROW_END; ++j) {
          for (auto &e : g.edges[j]) {
            if (e.y == oi && e.flow > 0) {
              // Found next vertex (j) to current path, will be added to the
              // solution on the beginning of the next k-cycle.
              k = j;
              goto k_start;
            }
          }
        } // j
        // We have reached a vertex which has no other matching vertices and
        // that means we finished tracing, asnwer_path contains charts that can
        // go one below another.
        unswer_paths.push_back(std::move(asnwer_path));
        break;
      } // k
    }
  }
  // For this task we just need to print amount of non-overlapping graphs.
  cout << unswer_paths.size() << "\n";
}
