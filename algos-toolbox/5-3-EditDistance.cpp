#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <limits>

using namespace std;

uint edit_distance(string s1, string s2) {
    if (s1.empty()) {
        return s2.length();
    }
    else if (s2.empty()) {
        return s1.length();
    }

    auto deliting = edit_distance(string(s1, 1), s2) + 1;
    auto insertion = edit_distance(s1, string(s2, 1)) + 1;
    auto modifications = edit_distance(string(s1, 1), string(s2, 1)) + ((s1[0] == s2[0]) ? 0 : 1);

    return std::min(std::min(deliting, insertion), modifications);
}

void print_dp(const vector<vector<uint>>& dp, string s1, string s2) {
	uint M = s2.length(), N = s1.length();

	printf("       ");
	for (uint i = 0; i < N; ++i) {
		printf("%2c ", s1[i]);	
	}
	cout << endl;
	for (uint i = 0; i <= M; ++i) {
		if ( i > 0 ) {
			printf("%2c  ", s2[i-1]);
		}
		else { 
			printf("    ");
		}

		for (uint j = 0; j <= N; ++j) {
			printf("%2d ", dp[i][j]);
		}
		cout << endl;
	}	
}

uint edit_distance_dp(string s1, string s2) {
	uint N = s1.length(), M = s2.length();
	vector<vector<uint>> dp(M + 1, vector<uint>(N + 1));

	// Fill in first column. that would be what is edit distance between all prefixes of s2 and empty string,
	// we need to insert all characters of s2 prefix.
	for (uint i = 0; i <= M; ++i) {
		dp[i][0] = i;
	}
	// Fill in first row.
	for (uint i = 0; i <= N; ++i) {
		dp[0][i] = i;
	}

	for (uint i = 1; i <= M; ++i) {
		for (uint j = 1; j <= N; ++j) {
			bool match = s1[j-1] == s2[i-1];
			uint delete_cost = dp[i][j-1] + 1;
			uint insert_cost = dp[i-1][j] + 1;
			uint keep_or_mismatch_cost = dp[i-1][j-1] + (match ? 0 : 1);
			dp[i][j] = std::min(std::min(delete_cost, insert_cost), keep_or_mismatch_cost);
		}
	}

	//print_dp(dp, s1, s2);

	return dp[M][N];
}

int main() {
    string s1, s2;
    cin >> s1;
    cin >> s2;
    cout << edit_distance_dp(s1, s2) << endl;
}
