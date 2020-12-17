#include <array>
#include <vector>
#include <string>
#include <list>
#include <cstring>
#include <algorithm>
#include <unordered_map>

const auto ALPH = 26;

struct Node {
	Node(char letter, std::string fullname, bool is_dict)
		: letter(letter), fullname(fullname), is_dict(is_dict),
		  suffix(nullptr), dict_suffix(nullptr),
		  child_count(0), children({0}) {}

	void print(int level = 0, bool only_self = false) {
		for (auto i = 0; i < level; ++i) printf("    ");

		printf("I am '%s'(%c). dict: %s, suffix: '%s', dict_suffix: '%s'. ",
		       fullname.c_str(), letter, is_dict ? "yes" : "no",
		       suffix ? suffix->fullname.c_str() : "NULL", dict_suffix ? dict_suffix->fullname.c_str() : "NULL");

		if (only_self || child_count == 0) {
			printf("\n");
			return;
		}

		printf("Have %d children:\n", child_count);
		
		for (auto c : children) {
			if (c != nullptr) {
				for (auto i = 0; i < level; ++i) printf("    ");
				c->print(level + 1);
			}
		}
		printf("\n");
	}

	void insert(const char* s, std::string fullname) {
		if (strlen(s) == 0) {
			is_dict = true;
			this->fullname = fullname;
			return;
		}

		char c = s[0];
		auto idx = c-'a';
		if (!children[idx]) {
			auto child_name = fullname.substr(0, fullname.size() - strlen(s) + 1);
			children[idx] = new Node(c, child_name, false);
			++child_count;
		}

		children[idx]->insert(s + 1, fullname);
	}

	Node* get(const char* s) {
		if (strlen(s) == 0) return this;

		auto idx = s[0]-'a';
		if (!children[idx]) return nullptr;

		return children[idx]->get(s + 1);
	}

	// Returns all nodes that are suffixes of the current node and are in
	// the dictionary, including the current node if it is in the
	// dictionary.
	std::vector<Node*> matches() {
		std::vector<Node*> results;

		if (dict_suffix) results = dict_suffix->matches();
		if (is_dict) results.push_back(this);

		return results;
	}

	// Extends the current node to be the longest possible match ending in
	// c. If c is not one of the node's direct children, recursively checks
	// suffixes until one that matches is found or the root node is
	// encountered.
	Node* extend(char c) {
		if (children[c-'a']) return children[c-'a'];
		if (fullname == "") return this;
		return suffix->extend(c);
	}

	char letter;
	std::string fullname;

	// Whether this node is in the dictionary or not
	bool is_dict;
	// Pointer to the node that is the longest complete suffix of this
	// node, or, if none, the root node. The root node itself has a suffix
	// of nullptr.
	Node* suffix;
	// Same as above, with the restriction of being a dictionary suffix. If
	// there is none, will be NULL.
	Node* dict_suffix;

	int child_count;
	std::array<Node*, ALPH> children;
};

class Trie {
public:
	Trie() {
		root = new Node('!', "", false);
	}

	Trie(std::vector<std::string> entries) {
		root = new Node('!', "", false);

		for (auto entry : entries) insert(entry.c_str());

		init();
	}

	void print() {
		root->print();
	}

	void insert(const char* s) {
		root->insert(s, s);
	}

	Node* get(const char* s) {
		return root->get(s);
	}

	// Caluclates all links between nodes
	void init() {
		find_suffixes();
		find_dict_suffixes();
	}

	Node *root;

private:
	void find_suffixes() {
		std::list<Node*> queue {root};
		while (!queue.empty()) {
			auto n = queue.front(); queue.pop_front();

			if (n->child_count == 0) continue;

			for (auto c : n->children) {
				if (!c) continue;
				queue.push_back(c);

				// All children of root have root as a suffix
				if (n == root) {
					c->suffix = root;
					continue;
				}

				auto idx = c->letter - 'a';
				auto s = n->suffix;
				while (!s->children[idx] && s != root) s = s->suffix;
				if (s->children[idx]) c->suffix = s->children[idx];
				else c->suffix = root;
			}
		}
	}

	void find_dict_suffixes() {
		std::list<Node*> queue {root};

		while (!queue.empty()) {
			auto n = queue.front(); queue.pop_front();

			if (n->child_count > 0) {
				for (auto c : n->children) {
					if (!c) continue;
					queue.push_back(c);
				}
			}

			if (n == root) continue;

			if (n->suffix == root) n->dict_suffix = nullptr;
			else if (n->suffix->is_dict) n->dict_suffix = n->suffix;
			else if (n->suffix->dict_suffix == nullptr) n->dict_suffix = nullptr;
			else n->dict_suffix = n->suffix->dict_suffix;

		}
	}
};

std::unordered_map<std::string, uint64_t> ac_count(std::vector<std::string> patterns, std::string text) {
	Trie trie(patterns);
	std::unordered_map<std::string, uint64_t> counts;

	auto n = trie.root;
	for (auto c : text) {
		n = n->extend(c);
		for (auto m : n->matches()) counts[m->fullname]++;
	}

	return counts;
}
