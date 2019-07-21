#include <algorithm>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <cstdint>

using namespace std;

struct segment_t {
	int64_t from, to;
	bool operator<(const segment_t& s2) const {
		auto &s1 = *this;
		return (s1.from < s2.from) || (s1.from == s2.from && s1.to < s2.to);
	}
};

vector<uint> lottery(vector<segment_t> segments, vector<int64_t> points) {
	vector<uint> result(points.size());

	vector<int64_t> segment_begins(segments.size());
	vector<int64_t> segment_ends(segments.size());
	for (size_t i = 0; i < segments.size(); ++i) {
		segment_begins[i] = segments[i].from;
		segment_ends[i] = segments[i].to;
	}
	std::sort(segment_begins.begin(), segment_begins.end());
	std::sort(segment_ends.begin(), segment_ends.end());

	// printf("DEBUG: segment begins:\n");
	// for (auto sb : segment_begins) {
	// 	printf("%d\n", sb);
	// }
	// cout << endl;

	uint pi = 0;
	for (auto p : points) {
		auto begin_lb = std::upper_bound(segment_begins.begin(), segment_begins.end(), p);
		uint begins_count = std::distance(segment_begins.begin(), begin_lb);
		//printf("at point %d segments began: %d\n", p, begins_count);

		auto end_lb = std::lower_bound(segment_ends.begin(), segment_ends.end(), p);
		uint ends_count = std::distance(segment_ends.begin(), end_lb);
		//printf("at point %d segments ended: %d\n", p, ends_count);

		result[pi] = begins_count - ends_count;
		pi++;
	}

	return result;
}

vector<uint> lottery_naive(vector<segment_t> segments, vector<int64_t> points) {
	vector<uint> result(points.size());
	uint pi = 0;
	for (auto p : points) {
		for (auto s : segments) {
			if (p >= s.from && p <= s.to) {
				result[pi]++;				
			}
		}
		pi++;
	}
	return result;
}

void stress_test() {
	for (int test_n = 0; test_n < 10000; ++test_n) {
		// generate N random segments 	
		int N = 30;
		vector<segment_t> random_segments(N);
		for (int i = 0; i < N; ++i) {
			int range_left = rand() % 100 - 100;
			int range_right = rand() % 100 - 50;
			if (range_left > range_right) {
				std::swap(range_left, range_right);
			}
			random_segments[i] = {range_left, range_right};
		}
		// generate M random points
		int M = 50;
		vector<int64_t> random_points(M);
		for (int i = 0; i < M; ++i) {
			random_points[i] = rand() % 300 - 300;
		}

		auto naive_result = lottery_naive(random_segments, random_points);
		auto lottery_result = lottery(random_segments, random_points);
		if (naive_result  != lottery_result) {
			cout << "DIFF:\n";
			cout << "Segments:\n";
			for (auto s : random_segments) {
				cout << s.from << ".." << s.to << "; ";
			}
			cout << endl;
			cout << "Points:\n";
			for (auto p : random_points) {
				cout << p << " ";
			}
			cout << endl;
			cout << "Naive:\n";
			for (auto p : naive_result) {
				cout << p << " ";
			} 
			cout << endl;
			cout << "Lotery:\n";
			for (auto p : lottery_result) {
				cout << p << " ";
			} 
			cout << endl;
			break;
		} else {
			cout << test_n << " - PASS\n";
		}
	}
}

int main() {
	// stress_test();
	// return 0;

	int n, m;
	cin >> n;
	cin >> m;
	vector<segment_t> segments(n);
	for (int i = 0; i < n; ++i) {
		cin >> segments[i].from;
		cin >> segments[i].to;
	}
	vector<int64_t> points(m);
	for (int i = 0; i < m; ++i) {
		cin >> points[i];
	}
	for (auto x : lottery(std::move(segments), std::move(points))) {
		cout << x << " ";
	}
	cout << endl;
}
