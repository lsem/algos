#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<string> generate_syclic_shifts(string s) {
  vector<string> result;
  int N = s.length();
  for (int i = 0; i < N; ++i) {
    result.push_back(string(N, '\0'));
    for (int j = 0; j < N; ++j) {
      result.back()[j] = s[(i + j + N) % N];
    }
  }
  return result;
}

int main() {
  for (auto &cs : generate_syclic_shifts("ABCDEFGHIJK")) {
    for (auto &c : cs) {
      printf("%c ", c);
    }
    printf("\n");
  }
}