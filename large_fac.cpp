#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>

typedef std::vector<uint8_t> Num;

// Little-endian
Num mul (Num a, Num b);

int main() {
	Num a = {1, 127, 255};
	Num b = {100, 101};
	mul(a, b);
}

Num mul (Num a, Num b)
{
	std::vector<Num> rows {b.size(), Num (a.size() + b.size(), 0)};

	for (auto y = 0; y < b.size(); y++) {
		uint16_t carry = 0;
		auto x = 0;
		for (; x < a.size(); x++) {
			uint16_t i = a[x];
			uint16_t j = b[y];
			uint16_t k = i * j + carry;
			carry = k / 256;
			k -= carry * 256;
			rows[y][x+y] = k;
			printf("Multiply %d by %d = %d, carry %d\n", i, j, k, carry);
		}
		rows[y][x+y] = carry;
	}

	for (auto row : rows) {
		for (auto c : row) printf("%3d ", c);
		printf("\n");
	}

	return {};
}
