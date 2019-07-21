#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdint>
#include <limits>

using namespace std;

using V = vector<uint>;

V tail(V v) {
    return V(begin(v) + 1, end(v));
}

uint head(V v) {
    return v[0];
}

bool empty(V v) { 
	return v.empty();
}

uint lcs3(V s1, V s2, V s3) {
    if (empty(s1) || empty(s2) || empty(s3)) {
        return 0;
    }
    if (head(s1) == head(s2) && head(s1) == head(s3)) {
        return lcs3(tail(s1), tail(s2), tail(s3)) + 1;
    }
    V lcss { lcs3(tail(s1), s2, s3),
         	 lcs3(s1, tail(s2), s3),
             lcs3(s1, s2, tail(s3)) };
    return *max_element(begin(lcss), end(lcss));
}

uint lcs3_dp(V s1, V s2, V s3) {
	uint N = s1.size(), M = s2.size(), L = s3.size();
	vector<vector<V>> dp(N + 1, vector<V>(M + 1, V(L + 1)));
	for (uint i = 0; i < N; ++i) {
		dp[i][0][0] = 0;
	}
	for (uint i = 0; i < M; ++i) {
		dp[0][i][0] = 0;
	}
	for (uint i = 0; i < L; ++i) {
		dp[0][0][i] = 0;
	}

	for (uint i = 1; i <= N; ++i) {
		for (uint j = 1; j <= M; ++j) {
			for (uint k = 1; k <= L; ++k) {
				if (s1[i-1] == s2[j-1] && s1[i-1] == s3[k-1]) {
					dp[i][j][k] = dp[i-1][j-1][k-1] + 1;
				} else {
					uint l1 = dp[i-1][j][k],
						 l2 = dp[i][j-1][k],
						 l3 = dp[i][j][k-1];
						 dp[i][j][k] = max(max(l1, l2), l3);
				}
			}
		}
	}

	return dp[N][M][L];
}

int main() {
    uint n, m, l;
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
    cin >> l;
    vector<uint> s3(l);
    for (int i = 0; i < l; ++i) {
        cin >> s3[i];
    }
    // cout << lcs3(s1, s2, s3) << endl;
    cout << lcs3_dp(s1, s2, s3) << endl;
}

