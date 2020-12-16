#include <iostream>
#include <string>
#include <unordered_map>

#include "aho-corasick.hpp"

int main() {
	auto text = "abccabaaaaaaa";
	auto counts = ac_count({"a", "ab", "bab", "bc", "bca", "c", "caa", "aa"}, text);

	printf("Text: %s\n", text);
	for (auto [substring, count] : counts) {
		printf("%s occured %d times\n", substring.c_str(), count);
	}
}
