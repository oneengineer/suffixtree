#pragma once

#ifdef _WIN32
	#define MYDLL __declspec(dllexport) 
#else
	#define MYDLL
#endif

#include <string>
#include <vector>
#include "common.h"

using namespace std;

//(SuffixTree*)
void* createSuffixTreePointer(vector<string> strs, bool persistString);

//(QSuffixTree*)     (SuffixTree*)
void* createSuffixQueryTreePointer(const void * suffixtree);

//(QSuffixTree*) 
void * emptySuffixQueryTreePointer();

//QSuffixTree *
void cacheIntermediateNodeDefault(void* qtree);

//QSuffixTree *
void cacheIntermediateNode(void * qtree, double budgetRatio, double sampleRate);

//QSuffixTree *
string showStatistics(void * qtree);

//QSuffixTree *
void clearStatistics(void * qtree);

//SuffixTree *
vector<int> findStringIdx(void * tree, string s);
vector<string> findString(void * tree, string s);

//QSuffixTree *
vector<int> findStringIdx_QTree(void * qtree, string s, bool case_sensitive);
vector<string> findString_QTree(void * qtree, string s, bool case_sensitive);

vector<int> findStringIdx_QTree_wildcard(void * qtree, const vector<Charset> & s, bool case_sensitive);
vector<string> findString_QTree_wildcard(void * qtree, const vector<Charset> & s, bool case_sensitive);


void suffixTreeAddStrings(void * tree, vector<string> strs);
void suffixTreeAddString(void * tree, string str);

void freeSuffixTreePointer(void * tree);
void freeSuffixQueryTreePointer(void * tree);

void saveSuffixQueryTreeToFile(void * p, const char *path);
void saveSuffixQueryTree(void * p, ostream & o);

void readSuffixQueryTreeFromFile(void * p, const char *path);
void readSuffixQueryTree(void * p, istream & i);

vector<string> allString_SuffixQueryTree(void * qtree);
vector<string> allString_SuffixTree(void * tree);
