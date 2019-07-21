#include <iostream>
#include <algorithm>
#include <vector>
#include <cstdint>

using namespace std;

// minimum_coins Returns the minimum number of coins with denominations 1, 3, 4 that changes money.
uint minimum_coins_naive(uint money) {
	if ( money == 1 || money == 3 || money == 4) {
		return 1;
	}
	if (money == 2) {
		return 2;		
	}
	if (money == 0) {
		return 0;
	}	

	auto m1 = minimum_coins_naive(money - 1);
	auto m2 = minimum_coins_naive(money - 3);
	auto m3 = minimum_coins_naive(money - 4);
	return std::min(std::min(m1, m2), m3) + 1;
}

// How to construct DP table for this problem?
//	 q: how many coins needed to change 0 money? a: 0
//	 q: how many coins needed to change 1 money? 1
//	 q: how many coins needed to change 2 money? min(DP[2 - 1] + 1)
//   q: how many coins needed to change 3 money? min(DP[3 - 1], DP[3 - 3], DP[3 - 4])
//   q: how many coins needed to change M money? min(DP[M - 1], DP[M - 3], DP[M - 4]) + 1 for M > 3
uint minimum_coins(uint money) {
	vector<uint> dp(money + 1);
	dp[0] = 0;
	dp[1] = 1;
	dp[2] = 2;
	dp[3] = 1;
	dp[4] = 1;
	for (uint m = 5; m <= money; ++m) {
		dp[m] = std::min(dp[m-4], std::min(dp[m-1], dp[m-3])) + 1;
	}
	return dp[money];
}

int main() {
	uint n;
	cin >> n;
	cout << minimum_coins(n) << endl;
}