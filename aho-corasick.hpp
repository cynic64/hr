#include <array>
#include <string>
#include <list>
#include <cstring>
#include <algorithm>

const auto ALPH = 26;

struct Node {
	Node(char letter, bool is_dict)
		: letter(letter), is_dict(is_dict),
		  suffix(nullptr), dict_suffix(nullptr), child_count(0) {}

	void print(int level = 0, bool only_self = false) {
		for (auto i = 0; i < level; ++i) printf("    ");

		printf("I am %c. ", letter);
		if (is_dict) printf("I am in the dictionary. ");

		if (only_self || child_count == 0) {
			printf("\n");
			return;
		}

		printf("Here are my %d children:\n", child_count);
		
		for (auto c : children) {
			if (c != nullptr) {
				for (auto i = 0; i < level; ++i) printf("    ");
				c->print(level + 1);
			}
		}
	}

	void insert(const char* s) {
		if (strlen(s) == 0) {
			is_dict = true;
			return;
		}

		char c = s[0];
		auto idx = c-'a';
		if (!children[idx]) {
			children[idx] = new Node(c, false);
			++child_count;
		}

		children[idx]->insert(s + 1);
	}

	Node* get(const char* s) {
		if (strlen(s) == 0) return this;

		auto idx = s[0]-'a';
		if (!children[idx]) return nullptr;

		return children[idx]->get(s + 1);
	}

	char letter;

	// Whether this node is in the dictionary or not
	bool is_dict;
	// Pointer to the node that is the longest complete suffix of this
	// node, or, if none, the root node
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
		root = new Node('!', false);
	}

	void print() {
		root->print();
	}

	void insert(const char* s) {
		root->insert(s);
	}

	Node* get(const char* s) {
		return root->get(s);
	}

	// Caluclates all links between nodes
	void init() {
		find_suffixes();
	}

private:
	void find_suffixes() {
		printf("Traversing\n");
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

				auto extended_suffix = n->suffix->children[c->letter];
				auto root_suffix = root->children[c->letter];
				if (extended_suffix) c->suffix = extended_suffix;
				else if (root_suffix) c->suffix = root_suffix;
				else c->suffix = root;
			}
		}

		printf("Done traversing\n");
	}

	Node *root;
};
