
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <list>
#include <numeric>
#include <vector>

using namespace std;

// out_error -- first unmatched closing bracket if there is such, otherwise 
//				first unmatched opening bracket.
bool ballanced(const string &s, int& out_error) {
	auto opposite = [](char c) { 
		if (c == '{') return '}';
		if (c == '[') return ']';
		if (c == '(') return ')';
		return '0';
	};

	list<pair<char, size_t>> stack;
	size_t pos = 0;
	for (uint pos = 0; pos < s.length(); ++pos) {
		char c = s[pos];
		if (c == '{' || c == '[' || c == '(') {
			stack.push_front({c, pos});
		} else {
			if (c == '}' || c == ']' || c == ')') {
				if (stack.empty()) {
					out_error = pos;
					return false;
				}
				if (opposite(stack.front().first) != c) {
					out_error = pos;
					return false;
				}

				stack.pop_front();
			}
		}
	}

	if (!stack.empty()) {
		out_error = stack.front().second;
		return false;
	}

	return true;
}

void tests() {
	int error;
	assert( !ballanced("{[}", error) && error + 1 == 3);
	assert( !ballanced("foo(bar[i);", error) && error + 1 == 10);	
	assert( !ballanced("[](()", error) && error + 1 == 3);
}

int main() {
	// tests();
	string s;
	cin >> s;
	int error;
	cout << (ballanced(s, error) ? "Success" :  to_string(error + 1)) << endl;
}