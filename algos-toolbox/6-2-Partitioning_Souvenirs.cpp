
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <vector>

#ifdef TESTING
#include "partition.h"
#endif  // TESTING

using namespace std;

vector<uint> concat(vector<uint> x, uint v) {
  x.push_back(v);
  return x;
}

void trace_back_multiple_solutions(vector<uint> a, uint N, uint W,
                                   const vector<vector<uint>>& dp,
                                   vector<uint> solution,
                                   vector<vector<uint>>& solutions) {
  if (N <= 0) {
    return;
  }

  if (W == 0) {
    std::fill_n(std::back_inserter(solution), N, 0);
    std::reverse(solution.begin(), solution.end());
    solutions.push_back(std::move(solution));
    return;
  }

  auto item_weight = a[N - 1];
  auto c1 = dp[W][N - 1];

  if (W >= item_weight) {
    auto c2 = dp[W - item_weight][N - 1] + item_weight;
    if (c1 == c2) {
      trace_back_multiple_solutions(a, N - 1, W, dp, concat(solution, 0), solutions);
      trace_back_multiple_solutions(a, N - 1, W - a[N - 1], dp, concat(solution, 1), solutions);
      return;
    }
    if (c1 > c2) {
      trace_back_multiple_solutions(a, N - 1, W, dp, concat(std::move(solution), 0), solutions);
    } else {
      trace_back_multiple_solutions(a, N - 1, W - a[N - 1], dp, concat(std::move(solution), 1), solutions);
    }
  } else {
    trace_back_multiple_solutions(a, N - 1, W, dp, concat(std::move(solution), 0), solutions);
  }
}

void trace_back(vector<uint> a, uint N, uint W, const vector<vector<uint>>& dp,
                vector<uint> solution) {
  for (int i = N, w = W; i > 0; --i) {
    auto item_weight = a[i - 1];
    auto c1 = dp[w][i - 1];
    if (w >= item_weight) {
      auto c2 = dp[w - item_weight][i - 1] + item_weight;
      if (c1 > c2) {
        cout << "no take: " << a[i - 1] << endl;
        solution.push_back(0);
      } else {
        cout << "take: " << a[i - 1] << endl;
        w -= a[i - 1];
        solution.push_back(1);
      }

    } else {
      // does not fit, do nothing, not taking this one
      cout << "no take (2): " << a[i - 1] << endl;
      solution.push_back(0);
    }
  }

  std::reverse(solution.begin(), solution.end());
  uint sum = 0;
  for (uint i = 0; i < solution.size(); ++i) {
    cout << solution[i] << ", ";
    if (solution[i]) {
      sum += a[i];
    }
  }
  cout << endl << sum << endl;
}

vector<vector<uint>> knapsack_dp(vector<uint> items, uint W) {
  const uint N = items.size();

  vector<vector<uint>> dp(W + 1, vector<uint>(N + 1));

  for (uint i = 0; i <= N; ++i) {
    dp[0][i] = 0;  // regardless how much elements we can take, value of 0
                   // knapsack is 0
  }

  for (uint i = 0; i <= W; ++i) {
    dp[i][0] = 0;  // no items, no value
  }

  for (uint i = 1; i <= W; ++i) {
    for (uint j = 1; j <= N; ++j) {
      dp[i][j] = max(dp[i][j], dp[i][j - 1]);
      if (items[j] <= i) {
        dp[i][j] = max(dp[i][j], dp[i - items[j]][j - 1] + items[j]);
      }
    }
  }

  // cout << "DP table:\n";
  // for (int i = 0; i <= W; ++i) {
  // 	for (int j = 0; j <= N; ++j) {
  // 		printf("%2d ", dp[i][j]);
  // 	}
  // 	cout << endl;
  // }

  vector<vector<uint>> solutions;
  trace_back_multiple_solutions(items, items.size(), W, dp, {}, solutions);

  return solutions;
}

void print_solutions(vector<vector<uint>> solutions, vector<uint> items) {
  for (auto& s : solutions) {
    assert(s.size() == items.size());
    for (uint i = 0; i < s.size(); ++i) {
      if (s[i] == 1) {
        cout << items[i] << " ";
      }
    }
    cout << endl;
  }
}

bool three_equal_sums(const vector<uint>& a, bool verbose = false) {
  if (a.size() < 3) {
    return false;
  }
  auto sum = std::accumulate(a.begin(), a.end(), 0);
  if (sum % 3 != 0) {
    return false;
  }

  if (verbose) {
    cout << "SUM IS: " << sum << endl;
    cout << "SUM / 3 IS: " << sum / 3 << endl;
  }

  auto one_third_solutions = knapsack_dp(a, sum / 3);
  for (auto& sol : one_third_solutions) {
    vector<uint> taken, not_taken;
    for (uint i = 0; i < sol.size(); ++i) {
      if (sol[i]) {
        taken.emplace_back(a[i]);
      } else {
        not_taken.emplace_back(a[i]);
      }
    }

    // taken represents elements that sum up to one third. so the rest should be
    // dividible by 3 too.
    auto two_third_solutions = knapsack_dp(not_taken, sum / 3);
    if (!two_third_solutions.empty()) {
      return true;
    }
  }
  return false;
}

#ifdef TESTING
bool three_equal_sums_naive(const vector<uint>& a, bool verbose = false) {
  if (a.size() < 3) {
    return false;
  }
  auto v = a;
  partition::iterator_k it(v.size(), 3);
  while (true) {
    try {
      // std::auto_ptr<std::vector<std::vector<Elem> > >
      auto next_partition = it[v];
      if (verbose) cout << "(";
      std::array<uint, 3> sums;
      sums[0] = 0;
      sums[1] = 0;
      sums[2] = 0;
      string psep = "";
      uint pi = 0;
      for (auto& partition : *next_partition) {
        if (verbose) cout << psep << "(";
        psep = " ";
        string sep = "";
        for (auto& p : partition) {
          if (verbose) cout << sep << p;
          sep = " ";
          sums[pi] += p;
        }

        if (verbose) cout << ")";
        pi++;
      }
      if (verbose)
        cout << ") = " << sums[0] << ", " << sums[1] << ", " << sums[2] << endl;
      if (sums[0] == sums[1] && sums[0] == sums[2]) {
        // printf("found!");
        return true;
        // break;
      }

      ++it;
    } catch (std::overflow_error& e) {
      break;
    }
  }

  return false;
}

void stress_test() {
  const uint N = 13;

  for (uint test_num = 0; test_num < 100; ++test_num) {
    uint randN = rand() % N + 1;  //  1 ≤ 𝑛 ≤ N
    vector<uint> randA(randN);
    for (auto& x : randA) {
      x = rand() % 30 + 1;  // 1 ≤ 𝑣𝑖 ≤ 30 for all 𝑖.
    }

    auto naive_answer = three_equal_sums_naive(randA);
    auto dp_answer = three_equal_sums(randA);
    if (naive_answer != dp_answer) {
      cout << "FAIL AT: [";
      string sep = "";
      for (auto x : randA) {
        cout << sep << x;
        sep = " ";
      }
      cout << "]" << endl;
      cout << "naive answer: " << naive_answer << endl;
      cout << "dp_answer: " << dp_answer << endl;
    } else {
      cout << "PASSED: " << test_num << endl;
    }
  }
}

void tests() {
  assert(three_equal_sums({3, 3, 3, 3}) == false);
  assert(three_equal_sums({40}) == false);
  assert(three_equal_sums({17, 59, 34, 57, 17, 23, 67, 1, 18, 2, 59}) == true);
  assert(three_equal_sums({1, 2, 3, 4, 5, 5, 7, 7, 8, 10, 12, 19, 25}) == true);
  assert(three_equal_sums({15, 25, 28, 20, 6, 6, 7, 22, 26, 30, 12, 4}) ==
         true);
  assert(three_equal_sums({10, 28, 24, 12, 11, 23, 14, 18, 22, 9}) ==
         true);  // ((10 24 23) (28 11 18) (12 14 22 9)) = 57, 57, 57
  assert(three_equal_sums({10, 20, 4, 14, 18, 28, 24, 3, 16, 7, 24}) ==
         true);  // ((10 20 3 16 7) (4 28 24) (14 18 24)) = 56, 56, 56
  assert(three_equal_sums({23, 4, 20, 10, 18, 11, 4, 10, 19, 27, 26, 8}) ==
         true);  // ((23 10 27) (4 20 18 10 8) (11 4 19 26)) = 60, 60, 60
  stress_test();
}

int main() { tests(); }
#else

int main() {
  uint n;
  cin >> n;
  vector<uint> a(n);
  for (uint i = 0; i < n; ++i) {
    cin >> a[i];
  }

  cout << three_equal_sums(a) << endl;
}

#endif  // #ifdef TESTING
