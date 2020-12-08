#include <vector>
#include <cstdint>
#include <iostream>
#include <algorithm>

typedef std::vector<uint8_t> Num;
const auto BASE = 10;

// Little-endian
Num mul (Num a, Num b);
Num add (Num a, Num b);
std::string format(Num n);
Num fac (Num n);

int main()
{
	std::cout << format(fac({7, 8, 9})) << std::endl;
}

Num fac(Num n)
{
	Num a {1};
	Num b {1};
	while (b != n) {
		a = mul(a, b);
		b = add(b, {1});
	}

	return a;
}

Num mul(Num a, Num b)
{
	std::vector<Num> rows {b.size(), Num (a.size() + b.size(), 0)};

	for (auto y = 0; y < b.size(); y++) {
		uint16_t carry = 0;
		auto x = 0;
		for (; x < a.size(); x++) {
			uint16_t i = a[x];
			uint16_t j = b[y];
			uint16_t k = i * j + carry;
			carry = k / BASE;
			k -= carry * BASE;
			rows[y][x+y] = k;
		}
		rows[y][x+y] = carry;
	}

	Num sum {0};
	for (auto row : rows) sum = add(sum, row);
	while (sum.back() == 0) sum.pop_back();

	return sum;
}

Num add(Num a, Num b)
{	
	Num n;
	auto len = std::max(a.size(), b.size());
	uint16_t carry = 0;
	
	for (auto i = 0; i < len; i++) {
		uint8_t j = i < a.size() ? a[i] : 0;
		uint8_t k = i < b.size() ? b[i] : 0;
		uint16_t x = j + k + carry;
		carry = x / BASE;
		x -= carry * BASE;
		n.push_back(x);
	}

	if (carry > 0) n.push_back(carry);

	return n;
}

std::string format(Num n)
{
	std::string out;
	for (auto it = n.rbegin(); it != n.rend(); ++it)
		out.push_back(*it + '0');

	return out;
}
