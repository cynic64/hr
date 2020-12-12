#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <cassert>
#include <cstring>

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
	for (auto i = 0; i < text.size(); ++i) {
		auto ok = true;
		for (auto j = 0; j < needle.size(); ++j) {
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
	
	auto i = 0, c = 0;
	while (i < text.size()) {
		auto j = 0;
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
	auto i = 1, j = 0;
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
	for (auto i = 0, j = 0; i < text.size(); ++i, ++j) {
		if (j == pattern.size()) {
			j = t[j];
			count++;
		}
		
		while ((j >= 0) && (text[i] != pattern[j])) j = t[j];
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

	auto last = 0;
	auto pos = text.find(' ');
	vector<string> words;
	while (pos != string::npos && words.size() < 5000) {
		auto word = text.substr(last, (pos-last));
		if (word.size() && find(words.begin(), words.end(), word) == words.end()) words.push_back(word);
		last = pos+1;
		pos = text.find(' ', pos+1);
	}

	cout << "[Basic] Test: " << search_basic(needle, text) << endl;
	cout << "[Brute] Test: " << search_brute(needle, text) << endl;
	cout << "[Brute2] Test: " << search_brute2(needle, text) << endl;
	cout << "[KMP] Test: " << search_kmp(needle, text) << endl;
	cout << endl;

	//------------

	auto start = chrono::high_resolution_clock::now();
	auto total = 0;
	for (auto word : words) total += search_basic(word, text);
	cout << "[Basic] Total: " << total << endl;
	cout << "[Basic] Time: " << elapsed(start) << endl << endl;

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
