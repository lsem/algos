
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

struct FastStack {
  FastStack(size_t size_hint) : items() { this->items.reserve(size_hint); }

  void push(uint x) { this->items.push_back(x); }

  void pop() { this->items.pop_back(); }

  uint top() const { return this->items.back(); }

  bool empty() const { return this->items.empty(); }

  vector<uint> items;
};

struct StackWithMax {
  StackWithMax(size_t size_hint)
      : items_stack(size_hint), max_stack(size_hint) {}

  void push(uint x) {
    this->items_stack.push(x);
    if (this->max_stack.empty() || this->max_stack.top() <= x) {
      this->max_stack.push(x);  // new max
    }
  }

  void pop() {
    if (this->items_stack.top() == this->max_stack.top()) {
      this->max_stack.pop();
    }
    this->items_stack.pop();
  }

  uint top() const { return this->items_stack.top(); }

  uint max() const { return this->max_stack.top(); }

  bool empty() const { return this->items_stack.empty(); }

  FastStack items_stack, max_stack;
};

struct QueueWithMax {
  QueueWithMax(size_t size_hint) : main(size_hint), aux(size_hint) {}

  void enqueue(uint x) { this->main.push(x); }

  uint dequeue() {
    if (this->aux.empty()) {
      while (!this->main.empty()) {
        this->aux.push(this->main.top());
        this->main.pop();
      }
    }
    auto x = this->aux.top();
    this->aux.pop();
    return x;
  }

  uint max() {
    if (this->main.empty()) {
      assert(!this->aux.empty());
      return this->aux.max();
    } else if (this->aux.empty()) {
      assert(!this->main.empty());
      return this->main.max();
    } else {
      return std::max(this->main.max(), this->aux.max());
    }
  }

  StackWithMax main, aux;
};

vector<uint> max_in_sliding_window(vector<uint> nums, uint size) {
  vector<uint> maxs;

  QueueWithMax queue(size);

  for (uint i = 0; i < nums.size(); ++i) {
    queue.enqueue(nums[i]);
    // condition can be replaced with queue.size() == size
    // in this case loop can be replaced with foreach
    if (i >= size - 1) {
      maxs.push_back(queue.max());
      queue.dequeue();     
    }
  }

  return maxs;
}

int main() {
  uint n;
  cin >> n;
  vector<uint> nums(n);
  for (auto& num : nums) {
    cin >> num;
  }
  uint m;
  cin >> m;

  for (auto x : max_in_sliding_window(std::move(nums), m)) {
    cout << x << " ";
  }
  cout << endl;
}
