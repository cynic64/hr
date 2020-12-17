#include <array>
#include <vector>
#include <string>
#include <list>
#include <cstring>
#include <algorithm>
#include <unordered_map>

const auto ALPH = 26;

struct Node {
	Node() : children({0}), is_dict(false), suffix(nullptr) {}

	void insert(const char* s, size_t dict_idx) {
		if (strlen(s) == 0) {
			is_dict = true;
			this->dict_idx = dict_idx;
			return;
		}

		char c = s[0];
		auto idx = c-'a';
		if (!children[idx]) children[idx] = new Node;

		children[idx]->insert(s + 1, dict_idx);
	}

	std::array<Node*, ALPH> children;

	// Whether this node is in the dictionary or not
	bool is_dict;

	// This node's index within the dictionary (ignored if is_dict is false)
	size_t dict_idx;

	// Pointer to the node that is the longest complete suffix of this
	// node, or, if none, the root node. The root node itself has a suffix
	// of nullptr.
	Node* suffix;
};

struct State {
	std::array<size_t, ALPH> next;
	std::vector<size_t> match;
};

class ACAutomaton {
public:
	ACAutomaton(std::vector<std::string>& dict) : states({0}) {
		Node root;
		root.suffix = &root;
		dict_size = dict.size();
		for (size_t i = 0; i < dict_size; ++i) root.insert(dict[i].c_str(), i);

		states.resize(dict_size+1);
		std::unordered_map<Node *, size_t> indices ({{&root, dict_size}});
		std::list<Node *> queue = {&root};

		while (!queue.empty()) {
			auto n = queue.front();
			queue.pop_front();

			// Insert all children of n into states, setting match but not
			// setting next
			for (auto i = 0; i < 26; ++i) {
				auto c = n->children[i];
				if (!c) continue;

				// Find and set child's longest suffix
				auto s = n->suffix;
				while (s != &root && !s->children[i]) s = s->suffix;
				if (s->children[i] && s->children[i] != c) c->suffix = s->children[i];
				else c->suffix = &root;

				auto match = states[indices[c->suffix]].match;
				size_t idx;
			        if (c->is_dict) {
					idx = c->dict_idx;
					match.push_back(idx);
					states[idx] = {{}, match};
				} else {
					idx = states.size();
					states.push_back({{}, match});
				}

				indices[c] = idx;
				queue.push_back(c);
			}

			// Set next of the current node
			for (auto i = 0; i < 26; ++i) {
				auto next = n;
				while (next != &root && !next->children[i]) next = next->suffix;
				if (next->children[i]) next = next->children[i];
				states[indices[n]].next[i] = indices[next];
			}
		}
	}

	std::vector<int> search(const char * text) {
		std::vector<int> counts(dict_size, 0);
		auto state = states[dict_size];

		while (strlen(text)) {
			state = states[state.next[text[0]-'a']];
			for (auto m : state.match) counts[m]++;
			text++;
		}

		return counts;
	}

private:
	std::vector<State> states;
	size_t dict_size;
};
