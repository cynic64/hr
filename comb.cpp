#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;

vector<vector<int>> comb(vector<int> s) {
	if (s.size() == 0) return {{}};
	
	vector<vector<int>> out;
	for (auto i = 0; i < s.size(); ++i) {
		auto head = s[i];
		if (i < s.size() - 1) out.push_back({head});
		vector<int> tail {s.begin() + i + 1, s.end()};

		auto tails = comb(tail);
		for (auto t : tails) {
			t.insert(t.begin(), head);
			out.push_back(t);
		}
	}

	return out;
}

bool works(int k, vector<int> s) {
	for (auto i = 0; i < s.size(); ++i)
		for (auto j = 0; j < s.size(); ++j)
			if (i != j && (s[i] + s[j]) % k == 0)
				return false;
	return true;
}

int nonDivisibleSubset(int k, vector<int> s) {
	auto l = 0;
	for (auto c : comb(s))
		if (c.size() > l && works(k, c))
			l = c.size();
	return l;
}

int main() {
	vector<int> a;
	for (int i = 0; i < 100; ++i) a.push_back(i);
	auto c = nonDivisibleSubset(4, a);
	cout << c << endl;
}
