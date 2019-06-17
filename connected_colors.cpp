#include <iostream>
#include <string>
#include <vector>

using namespace std;

int number_of_color(const vector<vector<string>> &cells, int cell_row,
                    int cell_col, string color,
                    vector<vector<bool>> &visited_cells) {
  if (visited_cells[cell_row][cell_col]) {
    return 0;
  }

  if (cells[cell_row][cell_col] != color) {
    return 0;
  }

  int N = cells.size(), M = cells[0].size();

  visited_cells[cell_row][cell_col] = true;

  int right = 0, left = 0, up = 0, down = 0;
  if (cell_col < (M - 1)) {
    right =
        number_of_color(cells, cell_row, cell_col + 1, color, visited_cells);
  }
  if (cell_col > 0) {
    left = number_of_color(cells, cell_row, cell_col - 1, color, visited_cells);
  }
  if (cell_row < (N - 1)) {
    down = number_of_color(cells, cell_row + 1, cell_col, color, visited_cells);
  }
  if (cell_row > 0) {
    up = number_of_color(cells, cell_row - 1, cell_col, color, visited_cells);
  }

  return 1 + right + left + up + down;
}

void find_max_colored_area(vector<vector<string>> cells) {
  vector<vector<bool>> visited_cells(cells.size(),
                                     vector<bool>(cells[0].size()));
  int max = 0;
  string max_color;
  for (auto i = 0; i < cells.size(); ++i) {
    for (auto k = 0; k < cells.size(); ++k) {
      int n = number_of_color(cells, i, k, cells[i][k], visited_cells);
      if (n > max) {
        max = n;
        max_color = cells[i][k];
      }
    }
  }
  printf("Max %d of color: %s\n", max, max_color.c_str());
}

int main() {
  int n, m;
  cin >> n;
  cin >> m;

  vector<vector<string>> cells(n, vector<string>(m, ""));
  for (int i = 0; i < n; ++i) {
    for (int j = 0; j < m; ++j) {
      cin >> cells[i][j];
    }
  }

  find_max_colored_area(cells);
}