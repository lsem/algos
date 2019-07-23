
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
	FastStack(size_t size_hint): items(){
		items.reserve(size_hint);
	}

	void push(uint x) {
		this->items.push_back(x);
	}

	void pop() {
		this->items.resize(this->items.size() - 1);
	}

	uint top() const {
		return this->items[this->items.size() - 1];
	}

	bool empty() const { return items.empty(); }

	vector<uint> items;
};

struct StackWithMax {
	StackWithMax(size_t size_hint): items_stack(size_hint), max_stack(size_hint) {		
	}

	void push(uint x) {
		items_stack.push(x);
		if (max_stack.empty() || max_stack.top() <= x) {
			max_stack.push(x); // new max
		}
	}	

	void pop() {
		if (items_stack.top() == max_stack.top()) {
			max_stack.pop();
		}
		items_stack.pop();
	}

	uint top() const {
		return items_stack.top();
	}

	uint max() const {
		return max_stack.top();
	}

	bool empty() const {
		return items_stack.empty();
	}

	FastStack items_stack, max_stack;
};

struct QueueWithMax {
	QueueWithMax(size_t size_hint): main(size_hint), aux(size_hint) {}

	void f_enqueue(uint x) {
		this->main.push(x);
	}

	uint f_dequeue() {
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

	StackWithMax main, aux; // aux stack could be temp but it is going to be reused so memory
							 // footprint will be better.
};

vector<uint> max_in_sliding_window(vector<uint> nums, uint size) {
	vector<uint> maxs;
	
	QueueWithMax queue(size);

	if (nums.size() == 1) {
		return {nums[0]};
	}

	for (uint i = 0; i < nums.size(); ++i) {
		queue.f_enqueue(nums[i]);
		
		if (i == size - 1) {
			// at this moment we need to start to yield max
			maxs.push_back(queue.max());
		} else if (i >= size) {
			queue.f_dequeue();
			maxs.push_back(queue.max());
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

