#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <vector>

using namespace std;

uint max_gold_bars(uint w, const vector<uint>& gold_bars, int n, vector<vector<uint>>& memo) {	
	if (memo[w][n] != std::numeric_limits<uint>::max()) {
		return memo[w][n];
	}
	if (w == 0) {
		return 0;
	} else if (n < 0) {
		return 0;
	}
	uint max_value = 0;
	for (int i = 0; i < n; ++i) {
		// consider value of not taking this bar
		max_value = max(max_value, max_gold_bars(w, gold_bars, i - 1, memo));
		if (gold_bars[i] <= w) {
			// gold bar fits into our knapsack
			// now consider taking it
			max_value = max(max_value, max_gold_bars(w - gold_bars[i], gold_bars, i - 1, memo) + gold_bars[i]);
		}
	}

	memo[w][n] = max_value;

	return max_value;
}

uint max_gold_bars_dp(uint w, const vector<uint>& gold_bars) {	
	vector<vector<uint>> dp(w + 1, vector<uint>(gold_bars.size() + 1));
	for (uint i = 0; i <= w; i++) {
		dp[i][0] = 0; // all knapsacks are worthless with no gold bars
	}
	for (uint i = 0; i <= gold_bars.size(); ++i) {
		dp[0][i] = 0; // all golds are worthless with zero knapsack 
	}
	for (uint i = 1; i <= w; ++i) {
		for (uint j = 1; j <= gold_bars.size(); ++j) {
			dp[i][j] = max(dp[i][j], dp[i][j-1]);
			if (gold_bars[j] <= i) {
				dp[i][j] = max(dp[i][j], dp[i - gold_bars[j]][j-1] + gold_bars[j]);
			}
		}
		
	}	

	cout << "DP table:\n";
	for (int i = 0; i <= w; ++i) {
		for (int j = 0; j <= gold_bars.size(); ++j) {
			cout << dp[i][j] << " ";
		}
		cout << endl;
	}

	return dp[w][gold_bars.size()];
}

int main() {
	int w, n;
	cin >> w;
	cin >> n;
	vector<uint> gold_bars(n);
	for (uint i = 0; i < n; ++i) {
		cin >> gold_bars[i];
	}
	vector<vector<uint>> memo(w + 1, vector<uint>(n + 1, std::numeric_limits<uint>::max()));

	cout << max_gold_bars(w, gold_bars, n, memo) << endl;
	cout << "MEMO table:\n";
	for (int i = 0; i <= w; ++i) {
		for (int j = 0; j <= gold_bars.size(); ++j) {
			cout << ((memo[i][j] != std::numeric_limits<uint>::max()) ? memo[i][j] : 0) << " ";
		}
		cout << endl;
	}

	cout << endl;

	cout << max_gold_bars_dp(w, gold_bars) << endl;
}