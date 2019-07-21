#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

using iter_t = vector<uint>::iterator;

//      9 8 7 3 2 1
//       /      \
//   9 8 7      3 2 1
//   /\          /\
// 9   8 7      3  2 1
//     \/           /\
// 9   7 8      3   1 2
//   \/             \/
// 7 8 9         1 2 3
//      \       /
//     1 2 3 7 8 9
uint merge(iter_t begin1, iter_t middle, iter_t end2) {
  // todo: add std equal optimization to handle already sorted array.
  uint misplaces = 0;
  iter_t begin1_copy = begin1, end1 = middle, begin2 = middle;
  const size_t size = std::distance(begin1, end1) + std::distance(begin2, end2);
  vector<uint> temp(size);
  auto out_it = temp.begin();
  while (begin1 != end1 && begin2 != end2) {
    if (*begin1 <= *begin2) {
      *out_it++ = *begin1++;
    } else {
      *out_it++ = *begin2++;
      // found 1 misplace + everything to the right in
      // left subarray is also misplaced.
      misplaces += 1 + std::distance(begin1 + 1, end1);
    }
  }

  // determine, which one exhausted
  iter_t remaining_it, remaining_end;
  if (begin1 == end1) {
    std::copy(begin2, end2, out_it);
  } else {
    std::copy(begin1, end1, out_it);
  }
  std::copy(temp.begin(), temp.end(), begin1_copy);

  return misplaces;
}

uint merge_sort(vector<uint>::iterator begin, vector<uint>::iterator end) {
  auto size = std::distance(begin, end);
  if (size < 2) {
    return 0;
  }
  size_t m = size / 2;
  auto misplaces_left = merge_sort(begin, begin + m);
  auto misplaces_right = merge_sort(begin + m, end);
  auto misplaces_this = merge(begin, begin + m, end);
  return misplaces_left + misplaces_right + misplaces_this;
}

int inversions_naive(const vector<uint> &v) {
  int count = 0;
  for (int i = 0; i < v.size(); ++i) {
    for (int j = i + 1; j < v.size(); ++j) {
      if (v[i] > v[j]) {
        count++;
      }
    }
  }
  return count;
}

int inversions_merge_sort(vector<uint> v) {
  return merge_sort(v.begin(), v.end());
  // cout << "Sorted:";
  // for (auto x : v) {
  //   cout << x << ", ";
  // }
  // cout << endl;
  return 0;
}

void test(const vector<uint> &v) {
  int naive = inversions_naive(v);
  int merge = inversions_merge_sort(v);
  if (naive != merge) {
    cout << "Found diff. Naive: " << naive << ", merge: " << merge << endl;
    cout << "Vec: " << endl;
    for (auto e : v) {
      cout << e << " ";
    }
    cout << endl;
  } else {
    cout << "PASS\n";
  }
}

void tests() {
  test({9, 8, 7, 3, 2, 1});
  test({4, 5, 6, 7, 8, 1, 2, 3, 4, 5});
}

void stress_test() {
  for (int length = 1; length < 20; ++length) {
    // generate random vector
    vector<uint> rand_vec(length);
    for (int i = 0; i < length; ++i) {
      rand_vec[i] = rand() % (length * 2);
    }
    test(rand_vec);
  }
}

void run_tests() {
  tests();
  stress_test();
}

int main() {
  int n;
  cin >> n;
  vector<uint> v(n);
  for (int i = 0; i < n; ++i) {
    cin >> v[i];
  }
  cout << merge_sort(v.begin(), v.end()) << endl;
}
