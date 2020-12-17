#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <string>

#include "aho-corasick.hpp"

int main() {
	std::vector<std::string> dict = {"a", "ab", "bab", "bc", "bca", "c", "caa"};
	ACAutomaton searcher(dict);
	auto counts = searcher.search("abccab");

	for (size_t i = 0; i < counts.size(); ++i) {
		printf("'%s': %d\n", dict[i].c_str(), counts[i]);
	}
}
