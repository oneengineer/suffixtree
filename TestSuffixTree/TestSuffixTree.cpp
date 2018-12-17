// st.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "pch.h"
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <string>
#include "SuffixTree.h"

using namespace std;

template<int len>
string randStr() {
	char s[len + 1];
	for (int i = 0; i < len; i++) {
		int r = rand();      // Returns a pseudo-random integer between 0 and RAND_MAX.
		r %= 5;
		s[i] = (char)((int)'a' + r);
	}
	s[len] = '\0';
	return string(s);
}

void benchmark() {
	SuffixTree *st = new SuffixTree(true);
	vector<string> strs;
	std::ifstream file("test.txt");
	std::string str;
	cout << "started " << endl;
	int c = 0;
	while (std::getline(file, str))
	{
		strs.push_back(str);
		c += 1;
		//if (c > 100000) break;
	}
	cout << "finished reading " << strs.size() << endl;
	file.close();

	typedef std::chrono::milliseconds ms;
	auto start = std::chrono::system_clock::now();
	st->addStrings(strs);
	c = 0;
	for (auto i : strs) {
		//st.addString(i);
		//c += 1;
		//if (c % 10000 == 0) {
		//	auto end = std::chrono::system_clock::now();
		//	auto duration = end - start;
		//	cout << "time cost " << (chrono::duration_cast<ms>(duration).count()) << endl;
		//}

	}

	auto end = std::chrono::system_clock::now();
	auto duration = end - start;

	cout << "time cost " << (chrono::duration_cast<ms>(duration).count()) << endl;
	cout << "end nodes: " << st->nodeSize() << endl;


	auto qtree = st->queryTree();
	auto end2 = std::chrono::system_clock::now();
	duration = end2 - end;
	cout << "time cost create qtree" << (chrono::duration_cast<ms>(duration).count()) << endl;

	delete st;

	auto end3 = std::chrono::system_clock::now();
	duration = end3 - end2;
	cout << "time cost delete tree" << (chrono::duration_cast<ms>(duration).count()) << endl;

	qtree.cacheIntermediateNode(1.0);

	auto end4 = std::chrono::system_clock::now();
	duration = end4 - end3;
	cout << "time cost counting tree" << (chrono::duration_cast<ms>(duration).count()) << endl;

	//cout << qtree.histogramNodeCounting();

	//for (int i = 0; i < strs.size(); i++) {
	//	auto s = strs[i];
	//	//cout << "to find " << s << endl;
	//	auto result = qtree.findSubString(s);

	//	if (result.size() != 1) {
	//		cout << "Error" << i << " " << s << endl; break;
	//	}

	//}

	char ch;
	cin >> ch;
	cout << "done" << endl;

}


void benchmark_QSuffixTree() {
	SuffixTree *st = new SuffixTree(true);
	vector<string> strs;
	std::ifstream file("test.txt");
	std::string str;
	cout << "started " << endl;
	int c = 0;
	while (std::getline(file, str))
	{
		strs.push_back(str);
		c += 1;
		//if (c > 10000) break;
	}
	cout << "finished reading " << strs.size() << endl;
	file.close();

	typedef std::chrono::milliseconds ms;
	auto start = std::chrono::system_clock::now();
	st->addStrings(strs);
	c = 0;

	auto end = std::chrono::system_clock::now();
	auto duration = end - start;

	cout << "time cost " << (chrono::duration_cast<ms>(duration).count()) << endl;
	cout << "end nodes: " << st->nodeSize() << endl;

	auto qtree = st->queryTree();
	auto end2 = std::chrono::system_clock::now();
	duration = end2 - end;
	cout << "time cost create qtree" << (chrono::duration_cast<ms>(duration).count()) << endl;

	delete st;

	auto end3 = std::chrono::system_clock::now();
	duration = end3 - end2;
	cout << "time cost delete tree" << (chrono::duration_cast<ms>(duration).count()) << endl;

	qtree.cacheIntermediateNode(1.0);

	auto end4 = std::chrono::system_clock::now();
	duration = end4 - end3;
	cout << "time cost counting tree" << (chrono::duration_cast<ms>(duration).count()) << endl;

	//cout << qtree.histogramNodeCounting();

	srand(10);

	qtree.serializeToFile("mya.bin");

	cout << "after seralize " << endl;

	QSuffixTree q2;

	q2.deserializeFromFile("mya.bin");

	//return;

	int total_find = 0;
	for (int i = 0; i < (strs.size() < 100000 ? strs.size() : 100000); i++) {
		auto s = strs[i];
		int len1 = rand() % s.size();
		int len2 = rand() % s.size();
		if (len1 < len2)
			s = s.substr(len1, len2);
		else
			s = s.substr(len2, len1);

		if (len1 == len2) continue;

		auto result = q2.findSubString(s);
		auto result2 = qtree.findSubString(s);
		if (result.size() != result2.size()) {
			cout << "error" << endl;
		}
		total_find += (int)result.size();

		if (result.size() < 1) {
			cout << "error" << endl;
		}
	}

	auto end5 = std::chrono::system_clock::now();
	duration = end5 - end4;
	cout << "time cost search " << (chrono::duration_cast<ms>(duration).count()) << endl;
	cout << "total find: " << total_find << "  " << endl;

	//char ch;
	//cin >> ch;
	cout << "done" << endl;

}


void test() {
	SuffixTree st(false);
	set<string> strs;
	const int len = 4;
	int n = 20;

	const int len_test = 2;
	int n_test = 4;

	vector<string> data;
	for (int i = 0; i < n; i++) {
		strs.insert(randStr<len>());
	}

	for (auto i : strs) {
		cout << "put string  " << i << endl;
		data.push_back(i);
		st.addString(i);
	}

	cout << "-------------------------" << endl;

	for (int i = 0; i < n; i++) {
		auto temp = randStr<len_test>();
		cout << "To sub " << temp << endl;
		auto result = st.findSubStringIdx(temp);
		for (auto j : result) {
			cout << "\t get: " << data[j] << endl;
		}
	}
}




int main()
{
	srand(time(NULL));   // Initialization, should only be called once.

	//benchmark();
	benchmark_QSuffixTree();


	//test();


}

