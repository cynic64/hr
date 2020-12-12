#include <iostream>
#include <vector>
#include <complex>
#include <string>

const auto WIDTH = 100;
const auto HEIGHT = 40;

int main() {
	std::vector<std::string> v(HEIGHT, std::string(WIDTH, '.'));

	std::complex<double> pos(WIDTH/2, HEIGHT/2);
	std::complex<double> i(0, 1);

	for (auto j = 0.0; j < 7.0; j += 0.05) {
		auto npos = pos+4.0*j*(std::pow(i,j));
		std::cout << npos << std::endl;
		size_t x = npos.real(), y = npos.imag();
		if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) continue;
		v[y][x] = '#';
	}

	for (auto line : v) std::cout << line << std::endl;
}
