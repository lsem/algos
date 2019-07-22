#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <numeric>
#include <vector>

using namespace std;

struct SuperStack {
  void push(uint arg) {
    stack_list.push_front(arg);
    if (max_stack_list.empty() || arg >= max_stack_list.front()) {
      max_stack_list.push_front(arg);
    }
  }

  void pop() {
    if (stack_list.front() == max_stack_list.front()) {
      max_stack_list.pop_front();
    }
    stack_list.pop_front();
  }

  uint max() { return max_stack_list.front(); }

  uint top() { return stack_list.front(); }

  list<uint> stack_list;
  list<uint> max_stack_list;
};

int main() {
  uint n;
  cin >> n;

  SuperStack super_stack;

  vector<uint> out;

  for (uint i = 0; i < n; ++i) {
    string cmd;
    cin >> cmd;
    if (cmd == "push") {
      uint arg;
      cin >> arg;
      super_stack.push(arg);
    } else if (cmd == "pop") {
      super_stack.pop();
    } else if (cmd == "max") {
      out.push_back(super_stack.max());
    }
  }
  for (auto o : out) {
    cout << o << endl;
  }
}
