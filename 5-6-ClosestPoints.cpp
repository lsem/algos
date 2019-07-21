#include <vector>
#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <cstdint>
#include <limits>
#include <cmath>
#include <iomanip>

using namespace std;

struct point_t {
	int64_t x, y;
};

using iter_t = vector<point_t>::iterator;

double distance(const point_t& p1, const point_t& p2) {
	return std::sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

bool point_less_x(const point_t& p1, const point_t& p2) {
	return p1.x < p2.x;
}

double closest_points(iter_t begin, iter_t end) {
	size_t size = std::distance(begin, end); 

	if (size < 2) {
		return std::numeric_limits<double>::max();
	} else if (size == 2) {
		return distance(*begin, *(begin + 1));
	}

	size_t m = size / 2;
	auto middle_it = begin + m;

	auto sides_min_d = std::min(
		closest_points(begin, middle_it), 
		closest_points(middle_it, end)
	);

	// now lets consider element inside a stip of length 2*d.
	// construct fake element for lower bound	
	point_t strip_left_key { (int64_t)(middle_it->x - sides_min_d), 0 };
	auto strip_left_it = std::lower_bound(begin, middle_it, strip_left_key, point_less_x);

	point_t strip_right_key { (int64_t)(middle_it->x + sides_min_d), 0 };
	auto strip_right_it = std::upper_bound(middle_it, end, strip_right_key, point_less_x);

	vector<point_t> strip(std::distance(strip_left_it, strip_right_it));
	std::copy(strip_left_it, strip_right_it, strip.begin());

	std::sort(strip.begin(), strip.end(), [](point_t a, point_t b) {
		return a.y < b.y;
	});

	double stip_min_d = std::numeric_limits<double>::max();
	for (auto i = strip.begin(); i != strip.end(); ++i) {
		for (auto j = i + 1; j <= (i + 7) && j < strip.end(); ++j) {			
			stip_min_d = std::min(stip_min_d, distance(*i, *j));
		}
	}

	return std::min(stip_min_d, sides_min_d);
}

double closest_points_distance(vector<point_t> points) {
	std::sort(points.begin(), points.end(), [](point_t a, point_t b) {
		return a.x < b.x;
	});
	return closest_points(points.begin(), points.end());
}

double closest_points_naive(const vector<point_t>& points) {
	double min_d = std::numeric_limits<double>::max();
	for (size_t i = 0; i < points.size(); ++i) {
		for (size_t j = i + 1; j < points.size(); ++j) {
			min_d = std::min(min_d, distance(points[i], points[j]));
		}
	}
	return min_d;
}

void stress_test() {	
	for (int test_n = 0; test_n < 1000; ++test_n) {
		int N = 200;
		vector<point_t> rps(N);
		for (int i = 0; i < N; ++i) {
			rps[i].x = rand() % 100 - 50;
			rps[i].y = rand() % 100 - 50;
		}
		if (closest_points_naive(rps) != closest_points_distance(rps)) {
			cout << "DIFF:" << endl;
			cout << "Naive: " << closest_points_naive(rps) << endl;
			cout << "Fast: " << closest_points_distance(rps) << endl;
			break;
		} else {
			cout << "PASS\n";
		}
	}
}

int main() {
	uint n;
	cin >> n;

	vector<point_t>	points(n);
	for (uint i = 0; i < n; ++i) {
		cin >> points[i].x;
		cin >> points[i].y;
	}

	cout << std::fixed << std::setprecision(4) << 
		closest_points_distance(std::move(points)) << endl;
}
