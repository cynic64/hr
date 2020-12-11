#include <iostream>
#include <vector>

using namespace std;

void combs(int n, int m, int s, int k, vector<int> v) {
	if (k == 0) {
		for (auto x : v) cout << x << " ";
		cout << endl;
		return;
	}

	for (auto i = n; i < m; ++i) {
		v[s] = i;
		combs(i+1, m, s+1, k-1, v);
	}
}

int main() {
	vector<int> v (3);
	combs(0, 5, 0, 3, v);
}
