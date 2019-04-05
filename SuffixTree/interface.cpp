#include "pch.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "SuffixTree.h"
#include "SuffixQueryTree.h"
#include "common.h"

using namespace std;

const int CHAR_ANY = numeric_limits<int>::max() - 1; //.
const int CHAR_WORD = numeric_limits<int>::max() - 2; // \w
const int CHAR_STRING_START = numeric_limits<int>::max() - 3; // ^
const int CHAR_STRING_END = numeric_limits<int>::max() - 4; // $

void * createSuffixTreePointer(vector<string> strs, bool persistString) {
	SuffixTree* tree = new SuffixTree(persistString);
	tree->addStrings(strs);
	return (void *)tree;
}

void * createSuffixQueryTreePointer(const void * p) {
	const SuffixTree & suffixtree = *(SuffixTree *)p;
	QSuffixTree* qtree = new QSuffixTree(move(suffixtree.queryTree()));
	return (void *)qtree;
}

void * emptySuffixQueryTreePointer() {
	QSuffixTree * tree = new QSuffixTree ();
	return (void *)tree;
}


void cacheIntermediateNode(void * p, double budgetRatio, double sampleRate) {
	auto & qtree = *(QSuffixTree*)p;
	qtree.sample_rate = sampleRate;
	qtree.cacheIntermediateNode(budgetRatio);
}

void cacheIntermediateNodeDefault(void * qtree) {
	cacheIntermediateNode(qtree, 0.5f, 0.1f);
}


string showStatistics(void * p) {
	QSuffixTree & qtree = *(QSuffixTree *)p;
	return qtree.histogramNodeCounting();
}

void clearStatistics(void * p) {
	QSuffixTree & qtree = *(QSuffixTree *)p;
	qtree.clearStatistics();
}

vector<int> findStringIdx(void * p, string s) {
	SuffixTree & tree = *(SuffixTree *)p;
	return tree.findSubStringIdx(s);
}

vector<string> findString(void * p, string s) {
	SuffixTree & tree = *(SuffixTree *)p;
	return tree.findSubString(s);
}

vector<int> findStringIdx_QTree(void * p, string s, bool case_sensitive) {
	QSuffixTree & qtree = *(QSuffixTree *)p;
	if (case_sensitive)
		return qtree.findSubStringIdx<true>(s);
	else 
		return qtree.findSubStringIdx<false>(s);
}

vector<string> findString_QTree(void * p, string s, bool case_sensitive) {
	QSuffixTree & qtree = *(QSuffixTree *)p;
	if (case_sensitive)
		return qtree.findSubString<true>(s);
	else 
		return qtree.findSubString<false>(s);
}

void suffixTreeAddStrings(void * p, vector<string> strs) {
	SuffixTree & tree = *(SuffixTree *)p;
	tree.addStrings(strs);
}

void suffixTreeAddString(void * p, string str) {
	SuffixTree & tree = *(SuffixTree *)p;
	tree.addString(str);
}

void freeSuffixTreePointer(void * p) {
	SuffixTree * tree = (SuffixTree *)p;
	delete tree;
}

void freeSuffixQueryTreePointer(void * p) {
	QSuffixTree * tree = (QSuffixTree *)p;
	delete tree;
}


void saveSuffixQueryTreeToFile(void * p, const char *path) {
	QSuffixTree * tree = (QSuffixTree *)p;
	tree->serializeToFile(path);
}

void saveSuffixQueryTree(void * p, ostream & o) {
	QSuffixTree * tree = (QSuffixTree *)p;
	tree->serialize(o);
}

void readSuffixQueryTreeFromFile(void * p, const char *path) {
	QSuffixTree * tree = (QSuffixTree *)p;
	tree->deserializeFromFile(path);
}

void readSuffixQueryTree(void * p, istream & i) {
	QSuffixTree * tree = (QSuffixTree *)p;
	tree->deserialize(i);
}

vector<int> findStringIdx_QTree_wildcard(void * qtree, const vector<Charset> & s, bool case_sensitive) {
	QSuffixTree * tree = (QSuffixTree *)qtree;
	if (case_sensitive)
		return tree->findSubStringIdx_wildCard<true>(s);
	else 
		return tree->findSubStringIdx_wildCard<false>(s);
}
vector<string> findString_QTree_wildcard(void * qtree, const vector<Charset> & s, bool case_sensitive) {
	QSuffixTree * tree = (QSuffixTree *)qtree;
	if (case_sensitive)
		return tree->findSubString_wildCard<true>(s);
	else
		return tree->findSubString_wildCard<false>(s);
}

vector<string> allString_SuffixQueryTree(void * qtree) {
	QSuffixTree * tree = (QSuffixTree *)qtree;
	return tree->getStrings();
}
vector<string> allString_SuffixTree(void * tree) {
	SuffixTree * p = (SuffixTree *)tree;
	return p->getStrings();
}