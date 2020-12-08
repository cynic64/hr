#include <vector>
#include <iostream>

using namespace std;

vector<int> next(vector<int> l) {
	vector<int> out = {1};
	for (auto it = l.begin(); it+1 != l.end(); ++it)
		out.push_back(*it + *(it+1));
	out.push_back(1);
	return out;
}

int main() {
	vector<int> a = {1};
	for (auto i = 0; i < 20; ++i) {
		a = next(a);
		for (auto x : a) printf("%3d ", x);
		printf("\n");
	}
}
