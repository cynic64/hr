#include <iostream>

#include "aho-corasick.hpp"

int main() {
	Trie trie;
	trie.insert("abc");
	trie.insert("abd");
	trie.insert("cab");
	trie.insert("c");

	trie.init();
	trie.print();
}
