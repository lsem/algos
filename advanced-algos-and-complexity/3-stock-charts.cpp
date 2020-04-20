//
// Max-flow, Assigning Crew to Flights, Bipartite Matching
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
#include <sstream>
#include <tuple>
#include <vector>

using namespace std;

// compare stock prices of i and j.
// data[i][j] denotes stock price of stock j for point i.
bool stocks_cross(const vector<vector<unsigned>>& data, size_t i, size_t j) {
  // condition is the following:
  // for all points in year prices of stock i must be stricly greater or less
  // than corresponding prices of stock j.
  int first_sign = -2;
  for (size_t point = 0; point < data.size(); ++point) {
    int this_sign = 0;
    if (data[point][i] > data[point][j]) {
      this_sign = +1;
    } else if (data[point][i] < data[point][j]) {
      this_sign = -1;
    } else {
      // can return immidiately
      return true;
    }
    if (first_sign == -2) {
      first_sign = this_sign;
    }

    if (first_sign != this_sign) {
      // cross detected
      return true;
    }
  }

  return false;
}

int main() {
  unsigned stocks_n, points_in_year_n;
  cin >> stocks_n;
  cin >> points_in_year_n;
  vector<vector<unsigned>> data(points_in_year_n,
                                vector<unsigned>(stocks_n, 0));
  for (unsigned stock = 0; stock < stocks_n; ++stock) {
    for (unsigned point = 0; point < points_in_year_n; ++point) {
      unsigned price;  // price of stock [point] at point [point].
      cin >> price;
      data[point][stock] = price;
    }
  }

  // find crossings for each stock
  for (unsigned stock_i = 0; stock_i < stocks_n - 1; ++stock_i) {
    for (unsigned stock_j = stock_i + 1; stock_j < stocks_n; ++stock_j) {
      if (stocks_cross(data, stock_i, stock_j)) {
        // this two goes to different charts.
        cout << "stocks " << stock_i << " and " << stock_j
             << " go to different charts\n";
      } else {
        cout << "stocks " << stock_i << " and " << stock_j
             << " go to same chart\n";
      }
    }
  }
}
