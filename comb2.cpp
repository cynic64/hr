#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

auto c = 0;

void comb(int k, vector<int> s, size_t in_idx, size_t out_idx, vector<int> v) {
	if (k == 0) {
		// for (auto x : v) cout << x << " ";
		// cout << endl;
		c++;
		return;
	}

	for (auto i = in_idx; i < s.size(); ++i) {
		v[out_idx] = s[i];
		comb(k-1, s, i+1, out_idx+1, v);
	}
}

int main() {
	vector<int> a;
	for (auto i = 1; i <= 30; ++i) a.push_back(i);

	auto k = 10;
	vector<int> v (k);
	comb(k, a, 0, 0, v);
	cout << c << endl;
}
