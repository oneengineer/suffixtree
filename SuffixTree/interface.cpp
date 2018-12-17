#include "pch.h"

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "SuffixTree.h"
#include "SuffixQueryTree.h"

using namespace std;


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

vector<int> findStringIdx_QTree(void * p, string s) {
	QSuffixTree & qtree = *(QSuffixTree *)p;
	return qtree.findSubStringIdx(s);
}

vector<string> findString_QTree(void * p, string s) {
	QSuffixTree & qtree = *(QSuffixTree *)p;
	return qtree.findSubString(s);
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

