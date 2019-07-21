#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <limits>

using namespace std;

uint lcs(string s1, string s2) {
	if (s1.empty() || s2.empty()) {
		// nothing in common
		return 0;
	}
	if (s1[0] == s2[0]) {
		// +1
		return lcs(string(s1, 1), string(s2, 1)) + 1;
	} else {
		return std::max(lcs(s1, string(s2, 1)), lcs(string(s1, 1), s2));
	}
}

uint lcs_dp_vectors(vector<uint> s1, vector<uint> s2) {
	uint N = s1.size(), M = s2.size();
	vector<vector<uint>> dp(N + 1, vector<uint>(M + 1));
	for (uint i = 0; i <= N; ++i) {
		dp[i][0] = 0; // nothing in common
	}
	for (uint i = 0; i <= M; ++i) {
		dp[0][i] = 0; // nothing in common
	}
	for (uint i = 1; i <= N; ++i) {
		for (uint j = 1; j <= M; ++j) {
			if (s1[i-1] == s2[j-1]) {
				dp[i][j] = dp[i-1][j-1] + 1;
			} else {
				dp[i][j] = std::max(dp[i-1][j], dp[i][j-1]);
			}
		}
	}

	return dp[N][M];
}


int main() {
    uint n, m;
    cin >> n;
    vector<uint> s1(n);
    for (int i = 0; i < n; ++i) {
    	cin >> s1[i];
    }
    cin >> m;
    vector<uint> s2(m);
    for (int i = 0; i < m; ++i) {
    	cin >> s2[i];
    }
    cout << lcs_dp_vectors(s1, s2) << endl;
}
