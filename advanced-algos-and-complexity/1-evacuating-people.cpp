//
// Max-flow
// (c) Libomyr Semkiv, 2020
//
#include <cassert>
#include <fstream>
#include <iostream>
#include <list>
#include <sstream>
#include <vector>

using namespace std;

struct Edge {
  Edge(int y, int weight) : y(y), weight(weight) {}
  int y; // vertex this edge connected to
  int weight;
};

struct Graph {
  // edges[i] keeps all edges that start from vertex i.
  vector<list<Edge>> edges;

  Graph(int vertices_total_count) : edges(vertices_total_count, list<Edge>{}) {}

  // connects vertex vx to vy with weight w.
  void connect(int vx, int vy, int w) {
    cout << "g.edges.size(): " << edges.size() << "\n";
    assert(vx < this->edges.size() && vy < this->edges.size());
    this->edges[vx].emplace_back(vy, w);
  }

  void export_to_dot();
};

void find_max_flow() {
	// algorithm is like this.
	// having current flow F.
	// we generate residual graph Gf for current G and f.
	// try find a path in Gf.
	// increase current flow by found path
	// otherwise, end, resulting flow is maximum possible and corresponds to
	// mincut.
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
    g.connect(road_from - 1, road_to - 1, road_cap);
  }
  g.export_to_dot();
}

void Graph::export_to_dot() {
  auto &g = *this;
  stringstream ss;
  ss << "digraph doo {\n";
  for (unsigned road_num = 0; road_num < g.edges.size(); ++road_num) {
    for (auto e : g.edges.at(road_num)) {
      ss << "\t" << (road_num + 1) << " -> " << (e.y + 1)
         << " [weight=" << e.weight << "];\n";
    }
  }
  ss << "}";
  if (auto fs = std::ofstream("graph.gv")) {
    fs << ss.str();
  } else {
    throw std::runtime_error("Cannot open file\n");
  }
}
