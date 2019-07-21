#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <limits>

using namespace std;


namespace min_calc_operations {

static auto INF = std::numeric_limits<uint>::max();

// You are given a primitive calculator that can perform the following three 
// operations with the current number 𝑥: multiply 𝑥 by 2, multiply 𝑥 by 3, or add 1 to 𝑥. 
// Your goal is given a positive integer 𝑛, find the minimum number of operations needed 
// to obtain the number 𝑛 starting from the number 1.

uint nums(uint n) {
	if (n == 0) {
		return 0;
	} else if (n == 1) {
			return 0;
		return 0; // we already have 1 on the screen, nothing to do
	} else if (n == 2) {
			return 1;
		//return 1; // +1 | *2
	}

	uint m1 = nums(n - 1) + 1;
	uint m2 = n % 2 == 0 ? nums(n / 2) + 1 : INF;
	uint m3 = n % 3 == 0 ? nums(n / 3) + 1 : INF;

	return std::min(std::min(m1, m2), m3);
}

pair<uint, vector<uint>> nums_dp(uint num) {
	vector<uint> prev_choice(num + 1);

	vector<uint> dp(num + 1);
	dp[0] = 0; prev_choice[0] = 0;
	dp[1] = 0; prev_choice[1] = 0;
	dp[2] = 1; prev_choice[2] = 1;

	for (uint n = 3; n <= num; ++n) {
		uint m1 = dp[n - 1] + 1;
		uint m2 = n % 2 == 0 ? dp[n / 2] + 1 : INF;
		uint m3 = n % 3 == 0 ? dp[n / 3] + 1 : INF;

		uint min = std::min(std::min(m1, m2), m3);

		if (min == m1) {
			prev_choice[n] = n - 1;
		} 
		if (min == m2) {
			prev_choice[n] = n / 2 ;
		} 
		if (min == m3) {
			prev_choice[n] = n / 3;
		}

		dp[n] = min;
	}

	vector<uint> optimal_choices;
	for (uint k = num; k != 0; k = prev_choice[k] ) {
		optimal_choices.emplace_back(k);
	}
	std::reverse(optimal_choices.begin(), optimal_choices.end());

	return {dp[num], std::move(optimal_choices)};
}

} // namespace min_calc_operations

int main() {
	uint n;
	cin >> n;
	auto r = min_calc_operations::nums_dp(n);
	cout << r.first << endl;
	for (auto x : r.second) {
		cout << x << " ";
	}
	cout << endl;
}



