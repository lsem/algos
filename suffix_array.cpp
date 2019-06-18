#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<unsigned> suffix_array(const string &s) {

  const int L = s.length();

  //
  // initialization, the aim is to sort by first character in O(N),
  // is done with CountSort
  //
  const int ALPHABET_SIZE = 256;
  vector<unsigned> count(ALPHABET_SIZE, 0);
  for (char c : s) {
    count[c]++;
  }
  for (int i = 1; i < count.size(); ++i) {
    count[i] += count[i - 1];
  }
  // Now we can generate initial order
  vector<unsigned> order(L);
  for (int i = L - 1; i >= 0; --i) {
    auto si_place = --count[s[i]];
    order[si_place] = i;
  }
  // Compute classes
  vector<unsigned> eclass(L);
  eclass[order[0]] = 0;
  unsigned currentClass = 0;
  for (int i = 1; i < L; ++i) {
    if (s[order[i]] != s[order[i - 1]]) {
      eclass[order[i]] = eclass[order[i - 1]] + 1;
    } else {
      eclass[order[i]] = eclass[order[i - 1]];
    }
  }

  //
  // Iterations
  //
  int k = 1;
  while (k < L) {
    vector<unsigned> newOrder(L);

    // Not, to sort by first key, lets use CountSort again to sort newOrder.
    vector<unsigned> count(L, 0);
    for (int i = 0; i < L; ++i) {
      count[eclass[i]]++;
    }
    for (int i = 1; i < L; ++i) {
      count[i] += count[i - 1];
    }
    for (int i = L - 1; i >= 0; --i) {
      size_t start = (order[i] - (k) + L) % L;
      auto cl = eclass[start];
      count[cl]--;
      newOrder[count[cl]] = start;
    }

    // Compute newEclass, just compare previous and next parts elements of
    // newOrder. since we cannot do it directly, we need to
    vector<unsigned> newEclass(L);
    newEclass[newOrder[0]] = 0;
    for (int i = 1; i < L; ++i) {
      auto cur = newOrder[i];
      auto prev = newOrder[i - 1];
      auto mid = (cur + k) % L;
      auto midPrev = (prev + k) % L;

      if (eclass[cur] != eclass[prev] || eclass[mid] != eclass[midPrev]) {
        newEclass[cur] = newEclass[prev] + 1;
      } else {
        newEclass[cur] = newEclass[prev];
      }
    }

    order = std::move(newOrder);
    eclass = std::move(newEclass);

    k = k * 2;
  }

  return std::move(order);
}

vector<string> generate_syclic_shifts_naive(string s) {
  vector<string> result;
  int N = s.length();
  for (int i = 0; i < N; ++i) {
    result.emplace_back(N, '\0');
    for (int j = 0; j < N; ++j) {
      result.back()[j] = s[(i + j + N) % N];
    }
  }
  return result;
}

vector<int> naively_generated_sa(string s) {
  // printf("naively generated suffix array: \n");
  vector<pair<string, int>> naiveSA;
  auto cyclic_shifts = generate_syclic_shifts_naive(s);
  for (int i = 0; i < cyclic_shifts.size(); ++i) {
    naiveSA.emplace_back(cyclic_shifts[i], i);
  }
  std::sort(naiveSA.begin(), naiveSA.end());
  vector<int> result;
  for (auto x : naiveSA) {
    // printf("%d: %s\n", x.second, x.first.c_str());
    result.push_back(x.second);
  }
  return result;
}

string random_string(int size) {
  static char symbols[] = {'A', 'C', 'G', 'T'};
  // generate random string first.
  string s;
  for (int i = 0; i < size; ++i) {
    // s.push_back( symbols[rand() % 4] );
    s.push_back(symbols[1]);
  }
  s.push_back('$');
  return s;
}

void match_pattern_in_ca(const string &s, const vector<unsigned> &sa,
                         const string &pattern, vector<bool>& printed) {
  auto cmp_fn = [&](int sa_offset) {
    return s.compare(sa[sa_offset], pattern.length(), pattern, 0,
                     pattern.length());
  };
  auto pattern_less_than_nth_suffix = [&](int n) { return cmp_fn(n) >= 0; };
  auto pattern_equal_to_nth_suffix = [&](int sa_offset) {
    return cmp_fn(sa_offset) == 0;
  };

  size_t lo = 0, hi = sa.size();
  while (lo < hi) {
    size_t mid = lo + (hi - lo) / 2;
    if (pattern_less_than_nth_suffix(mid)) {
      hi = mid;
    } else {
      lo = mid + 1;
    }
  }
  // lo must be lower bound now.
  while (lo < sa.size() && pattern_equal_to_nth_suffix(lo)) {
    if (!printed[lo]) {
      printf("%d ", sa[lo]);
      printed[lo] = true;
    }
    lo++;
  }
  // printf("\n");
}

int main() {
  string s;
  cin >> s;
  s += "$";

  int n;
  cin >> n;

  vector<string> patterns(n);
  for (int i = 0; i < n; ++i) {
    cin >> patterns[i];
  }

  auto sa = suffix_array(s);
  vector<bool> printed(sa.size(), false);

  for (auto &p : patterns) {
    match_pattern_in_ca(s, sa, p, printed);
  }
  printf("\n");
}
