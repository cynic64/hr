#include <bits/stdc++.h>

using namespace std;

vector<string> split_string(string);

const auto ALPH = 26;

struct Node {
	Node() : children({0}), suffix(nullptr) {}

	void insert(const char* s, size_t dict_idx) {
		if (strlen(s) == 0) {
			dict_idxs.push_back(dict_idx);
			return;
		}

		char c = s[0];
		auto idx = c-'a';
		if (!children[idx]) children[idx] = new Node;

		children[idx]->insert(s + 1, dict_idx);
	}

	std::array<Node*, ALPH> children;

	// This node's indices within the dictionary (there can be multiple, if
	// the same string appears multiple times in the dictionary)
	std::vector<size_t> dict_idxs;

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
				// All indices will be included in match, but
				// only the first one will be referenced by
				// other nodes' nexts (ref_idx)
				auto idxs = c->dict_idxs;
				size_t ref_idx;
				if (idxs.size()) {
					match.insert(match.end(), idxs.begin(), idxs.end());
					ref_idx = idxs[0];
					states[ref_idx] = {{}, match};
				} else {
					ref_idx = states.size();
					states.push_back({{}, match});
				}
                    
				indices[c] = ref_idx;
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

	uint64_t get_health(const char * text, std::vector<int>& healths, size_t first, size_t last) {
		uint64_t health = 0;
		auto state = states[dict_size];

		while (strlen(text)) {
			state = states[state.next[text[0]-'a']];
			for (auto m : state.match) {
				if (m >= first && m <= last) health += healths[m];
			}
			text++;
		}

		return health;
	}

private:
	std::vector<State> states;
	size_t dict_size;
};


int main()
{
	int n;
	cin >> n;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	string genes_temp_temp;
	getline(cin, genes_temp_temp);

	vector<string> genes_temp = split_string(genes_temp_temp);

	vector<string> genes(n);

	for (int i = 0; i < n; i++) {
		string genes_item = genes_temp[i];

		genes[i] = genes_item;
	}

	string health_temp_temp;
	getline(cin, health_temp_temp);

	vector<string> health_temp = split_string(health_temp_temp);

	vector<int> health(n);

	for (int i = 0; i < n; i++) {
		int health_item = stoi(health_temp[i]);

		health[i] = health_item;
	}

	int s;
	cin >> s;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');

	printf("start constructing\n");
	ACAutomaton ac(genes);
	printf("done\n");

	uint64_t h_min = UINT64_MAX;
	uint64_t h_max = 0;
	for (int s_itr = 0; s_itr < s; s_itr++) {
		string firstLastd_temp;
		getline(cin, firstLastd_temp);

		vector<string> firstLastd = split_string(firstLastd_temp);

		int first = stoi(firstLastd[0]);

		int last = stoi(firstLastd[1]);

		string d = firstLastd[2];
        
		auto h = ac.get_health(d.c_str(), health, first, last);
        
		h_min = min(h_min, h);
		h_max = max(h_max, h);
	}
    
	cout << h_min << " " << h_max << endl;

	return 0;
}

vector<string> split_string(string input_string) {
	string::iterator new_end = unique(input_string.begin(), input_string.end(), [] (const char &x, const char &y) {
		return x == y and x == ' ';
	});

	input_string.erase(new_end, input_string.end());

	while (input_string[input_string.length() - 1] == ' ') {
		input_string.pop_back();
	}

	vector<string> splits;
	char delimiter = ' ';

	size_t i = 0;
	size_t pos = input_string.find(delimiter);

	while (pos != string::npos) {
		splits.push_back(input_string.substr(i, pos - i));

		i = pos + 1;
		pos = input_string.find(delimiter, i);
	}

	splits.push_back(input_string.substr(i, min(pos, input_string.length()) - i + 1));

	return splits;
}
