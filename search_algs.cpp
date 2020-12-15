#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <cassert>
#include <cstring>
#include <array>
#include <cmath>
#include <stdexcept>

using namespace std;

const auto BUFSIZE = 4096;

int search_basic(string needle, string text) {
	auto pos = text.find(needle);
	auto c = 0;
	while (pos != string::npos) {
		pos = text.find(needle, pos+1);
		c++;
	}
	return c;
}

int search_brute(string needle, string text) {
	auto c = 0;
	for (size_t i = 0; i < text.size(); ++i) {
		auto ok = true;
		for (size_t j = 0; j < needle.size(); ++j) {
			if (text[i+j] != needle[j]) {
				ok = false;
				break;
			}
		}
		if (ok) c++;
	}

	return c;
}

int search_brute2(string needle, string text) {
	if (needle.size() == 0) return 0;
	
	size_t i = 0;
	auto c = 0;
	while (i < text.size()) {
		size_t j = 0;
		for (; j < needle.size() && i < text.size(); i++, j++) {
			while (text[i] != needle[j] && i < text.size()) {
				i = i-j + 1;
				j = 0;
			}
		}
		if (j == needle.size() && i < text.size()) c++;
	}

	return c;
}

vector<int> kmp_init(string s) {
	vector<int> t(s.size()+1, 0);
	t[0] = -1;
	size_t i = 1, j = 0;
	while (i < s.size()) {
		if (s[i] == s[j]) t[++i] = ++j;
		else {
			// Sorcery
			if (j != 0) j = t[j];
			// If j was 0, we didn't find any matches for this
			// length
			else t[++i] = 0;
		}
	}

	return t;
}

int search_kmp(string pattern, string text) {
	if (pattern.size() == 0) return 0;
	
	auto t = kmp_init(pattern);
	auto count = 0;
	size_t i; int j;
	for (i = 0, j = 0; i < text.size(); ++i, ++j) {
		if ((size_t) j == pattern.size()) {
			j = t[j];
			count++;
		}
		
		while ((j >= 0) && (text[i] != pattern[j])) j = t[j];
	}

	return count;
}

class RKHasher {
public:
	RKHasher(uint64_t length, uint64_t d = 32, uint64_t q = 33554393)
		: state(0), d(d), q(q) {
		dM = 1;
		for (size_t i = 0; i < length-1; ++i) dM = dM*d % q;
	}

	static RKHasher from_text(string text) {
		auto hasher = RKHasher(text.size());
		for (auto c: text) hasher.adjust(0, c);

		return hasher;
	}

	// Given whatever character was first (prev) and what character will
	// become last, adjusts the hash.
	void adjust(uint64_t prev, uint64_t next) {
		state = ((state+d*q - dM*prev) * d + next) % q;
	}

	uint64_t state;

private:
	uint64_t d;
	uint64_t q;
	uint64_t dM;
};

int search_rk(string pattern, string text) {
	auto M = pattern.size(), N = text.size();
	if (M == 0 || M > N) return 0;

	auto p_hash = RKHasher::from_text(pattern).state;
	auto text_hasher = RKHasher::from_text(text.substr(0, M));

	auto count = 0;
	if (p_hash == text_hasher.state) ++count;

	for (size_t i = 0; i < N-M; ++i) {
		text_hasher.adjust(text[i], text[i+M]);
		if (p_hash == text_hasher.state) ++count;
	}

	return count;
}

int search_rk_multi(vector<string> patterns, string text) {
	auto M = patterns[0].size(), N = text.size();
	for (auto p : patterns) {
		if (p.size() != M)
			throw runtime_error("Pattern sizes don't match!");
	}

	vector<uint64_t> p_hashes;
	for (auto p : patterns)
		p_hashes.push_back(RKHasher::from_text(p).state);

	auto text_hasher = RKHasher::from_text(text.substr(0, M));

	auto count = std::count(p_hashes.begin(), p_hashes.end(), text_hasher.state);

	for (size_t i = 0; i < N-M; ++i) {
		text_hasher.adjust(text[i], text[i+M]);
		count += std::count(p_hashes.begin(), p_hashes.end(), text_hasher.state);
	}

	return count;
}	

// Index with whatever character mismatched
array<size_t, 256> init_bm(string pattern) {
	auto M = pattern.size();
	array<size_t, 256> v;
	for (size_t i = 0; i < 256; ++i) v[i] = M;
	for (size_t i = 0; i < M; ++i) v[pattern[i]] = M-i-1;

	return v;
}

int search_bm(string pattern, string text) {
	auto M = pattern.size(), N = text.size();
	if (M == 0 || M > N) return 0;

	auto t = init_bm(pattern);
	auto count = 0;

	size_t i = M - 1;
	while (i < N) {
		if (text[i] == pattern[M-1]) {
			size_t j = 1;
			for (; j < M; ++j) {
				if (text[i-j] != pattern[M-1-j]) break;
			}
			if (j == M) ++count;
			i++;
		}
		i += t[(uint8_t) text[i]];
	}

	return count;
}

// bm2 is the version in Sedgewick's book (plus a bugfix...)
// It's really slow.
// Only returns the index of the first result
size_t search_bm2(const char* pattern, const char* text, size_t start_pos) {
	size_t i, j, M = strlen(pattern), N = strlen(text);
	if (start_pos + M > N) return N;
	auto t = init_bm(pattern);

	for (i = start_pos + M-1, j = M-1; ; --i, --j) {
		while (text[i] != pattern[j]) {
			auto k = t[pattern[j]];
			i += (M-j > k) ? M-j : k;
			if (i >= N) return N;
			j = M-1;
		}

		if (j == 0) break;
	}

	return i+1;
}

// Returns count by calling search_bm2 over and over
int search_bm2_all(string pattern, string text) {
	auto count = 0;
	const char *p = pattern.c_str(), *t = text.c_str();
	auto i = search_bm2(p, t, 0);
	while (i < text.size()) {
		++count;
		i = search_bm2(p, t, i+1);
	}
	return count;
}

double elapsed(chrono::time_point<chrono::high_resolution_clock> start) {
	auto stop = chrono::high_resolution_clock::now();
	auto dur = chrono::duration_cast<chrono::microseconds>(stop - start);
	return (double) dur.count() / 1000000;
}

int main(int argc, char *argv[]) {
	if (argc < 3) {
		printf("Not enough args\n");
		exit(1);
	}

	string needle(argv[1]);
	string path(argv[2]);
	fstream file;
	file.open(path, ios::in);
	if (!file.is_open()) {
		printf("Couldn't open file\n");
		exit(1);
	}

	cout << "Needle: " << needle << endl;
	cout << "Path: " << path << endl;

	string text;
	char buf[BUFSIZE];
	while (file.read(buf, BUFSIZE)) text.append(buf, 0, file.gcount());
	text.append(buf, 0, file.gcount());
	file.close();

	cout << "Read " << text.size() << " Chars" << endl;

	vector<string> words;
	for (size_t i = 0; i < text.size() && words.size() < 100; ++i) {
		words.push_back(text.substr(i, 10));
	}

	cout << "[Basic] Test: " << search_basic(needle, text) << endl;
	cout << "[Brute] Test: " << search_brute(needle, text) << endl;
	cout << "[Brute2] Test: " << search_brute2(needle, text) << endl;
	cout << "[KMP] Test: " << search_kmp(needle, text) << endl;
	cout << "[BM] Test: " << search_bm(needle, text) << endl;
	cout << "[BM2] Test: " << search_bm2_all(needle, text) << endl;
	cout << "[RK] Test: " << search_rk(needle, text) << endl;
	cout << "[RK Multi] Test: " << search_rk_multi({needle}, text) << endl;
	cout << endl;

	//------------

	auto start = chrono::high_resolution_clock::now();
	auto total = 0;
	for (auto word : words) total += search_basic(word, text);
	cout << "[Basic] Total: " << total << endl;
	cout << "[Basic] Time: " << elapsed(start) << endl << endl;
	
	start = chrono::high_resolution_clock::now();	
	total = 0;
	for (auto word : words) total += search_bm(word, text);
	cout << "[BM] Total: " << total << endl;
	cout << "[BM] Time: " << elapsed(start) << endl << endl;

	start = chrono::high_resolution_clock::now();	
	total = 0;
	for (auto word : words) total += search_rk(word, text);
	cout << "[RK] Total: " << total << endl;
	cout << "[RK] Time: " << elapsed(start) << endl << endl;

	start = chrono::high_resolution_clock::now();	
	total = search_rk_multi(words, text);
	cout << "[RK Multi] Total: " << total << endl;
	cout << "[RK Multi] Time: " << elapsed(start) << endl << endl;

	start = chrono::high_resolution_clock::now();	
	total = 0;
	for (auto word : words) total += search_brute(word, text);
	cout << "[Brute] Total: " << total << endl;
	cout << "[Brute] Time: " << elapsed(start) << endl << endl;
	
	start = chrono::high_resolution_clock::now();	
	total = 0;
	for (auto word : words) total += search_brute2(word, text);
	cout << "[Brute2] Total: " << total << endl;
	cout << "[Brute2] Time: " << elapsed(start) << endl << endl;
	
	start = chrono::high_resolution_clock::now();	
	total = 0;
	for (auto word : words) total += search_kmp(word, text);
	cout << "[KMP] Total: " << total << endl;
	cout << "[KMP] Time: " << elapsed(start) << endl << endl;
}
