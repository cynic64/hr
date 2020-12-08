#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>



typedef std::vector<std::vector<int>> Matrix;

void print(Matrix mtx)
{
	for (auto row : mtx) {
		for (auto x : row) printf("%3d", x);
		std::cout << std::endl;
	}
}

std::pair<int, int> parent(int n, int m, int x, int y, int r)
{
	
	auto top_dist = y;
	auto bottom_dist = m-y-1;
	auto left_dist = x;
	auto right_dist = n-x-1;

	auto min_dist = std::min({top_dist, bottom_dist, left_dist, right_dist});
	auto perimeter = 2 * (n - 2*min_dist - 1) + 2 * (m - 2*min_dist - 1);
	r = (r % perimeter);

	if (r == 0) return {x, y};

	std::pair<int, int> pos;

	if (top_dist <= bottom_dist && top_dist <= left_dist && top_dist < right_dist)
		pos = {x+1, y};
	else if (bottom_dist <= top_dist && bottom_dist < left_dist && bottom_dist <= right_dist)
		pos = {x-1, y};
	else if (left_dist <= top_dist && left_dist <= bottom_dist && left_dist <= right_dist)
		pos = {x, y-1};
	else if (right_dist <= top_dist && right_dist <= bottom_dist && right_dist <= left_dist)
		pos = {x, y+1};
	else throw std::runtime_error("???");

	return parent(n, m, pos.first, pos.second, r-1);
}

Matrix rotate(Matrix in, int r)
{
	auto m = in.size();
	auto n = in[0].size();
	Matrix out {m, std::vector<int>(n, -1)};

	for (int y = 0; y < m; y++) {
		for (int x = 0; x < n; x++) {
			auto pos = parent(n, m, x, y, r);
			out[y][x] = in[pos.second][pos.first];
		}
	}

	return out;
}

int main()
{
	Matrix mtx {{1, 2, 3, 4},
		    {12, 1, 2, 5},
		    {11, 4, 3, 6},
		    {10, 9, 8, 7}};

	print(mtx);

	auto rotated = rotate(mtx, 1000);

	std::cout << std::endl;
	print(rotated);
}
