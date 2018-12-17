// suffix tree only for query, save space
#pragma once

#include "pch.h"
//#include <iostream>

#include <sstream>
#include <vector>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <string>

#include <memory>

#include <fstream>
#include <chrono>
#include <algorithm>

#include <memory>
#include <functional>

#include <cmath>

using namespace std;

class SerializationUtil {
public:
	template<typename T> 
	static inline void bytewrite(ostream & o, T & t) {
		o.write((const char *)&t, sizeof(T));
	}

	static inline void bytewrite(ostream & o, string & t) {
		int size = (int)t.size() + 1;
		bytewrite(o, size);
		o.write(t.c_str(), sizeof(char) * size );
	}

	template<typename T>
	static inline void byteread(istream & i, T & t) {
		i.read((char *)&t, sizeof(T));
	}

	static inline void byteread(istream & i, string & t) {
		int size;
		byteread(i, size);
		char * ch = new char[size];
		i.read(ch, sizeof(char) * size);
		t = ch;
		delete ch;
	}

	template<typename T>
	static inline void serializeStruct(ostream & o, const vector<T> & v) {
		int size = (int)v.size();
		bytewrite(o, size);
		for (const auto & i : v) {
			i.serialize(o);
		}
	}

	template<typename T>
	static inline void deserializeStruct(istream & i, vector<T> & v) {
		int size;
		byteread(i, size);
		v.reserve(size);
		for (int j = 0; j < size; j++) {
			T t;
			t.deserialize(i);
			v.push_back(t);
		}
	}

	template<typename T>
	static inline void serializeVector(ostream & o, vector<T> & v) {
		int size = (int)v.size();
		bytewrite(o, size);
		for (auto & i : v) {
			bytewrite(o, i);
		}
	}

	template<typename T>
	static inline void deserializeVector(istream & i, vector<T> & v) {
		int size;
		byteread(i, size);
		v.reserve(size);
		for (int j = 0; j < size; j++) {
			T t;
			//t.deserialize(i);
			byteread(i,t);
			v.push_back(t);
		}
	}
};

class QTreeNode {
public:
	int start;
	unsigned int lengthStr;
	int childrenIdx;
	unsigned int lengthChildren;

	QTreeNode(){}

	QTreeNode(const int & start, const int & end, 
		const int & childrenIdx, const int & lengthChildren):
			start(start), lengthStr(end - start), 
			childrenIdx(childrenIdx), lengthChildren(lengthChildren){}
	inline bool isLeaf() const {
		return lengthChildren < 1;
	}

	inline int end() const {
		return start + lengthStr;
	}

	inline void serialize( ostream & o) const {
		SerializationUtil::bytewrite(o, start);
		SerializationUtil::bytewrite(o, lengthStr);
		SerializationUtil::bytewrite(o, childrenIdx);
		SerializationUtil::bytewrite(o, lengthChildren);
	}

	inline void deserialize(istream & i) {
		SerializationUtil::byteread(i, start);
		SerializationUtil::byteread(i, lengthStr);
		SerializationUtil::byteread(i, childrenIdx);
		SerializationUtil::byteread(i, lengthChildren);
	}

};

class QChildren{ //store info of node
public:
	int ch;
	int pos;

	QChildren() {}

	QChildren(const int & ch, const int & pos):ch(ch), pos(pos){}

	inline void serialize(ostream & o) const {
		SerializationUtil::bytewrite(o, ch);
		SerializationUtil::bytewrite(o, pos);
	}

	inline void deserialize(istream & i) {
		SerializationUtil::byteread(i, ch);
		SerializationUtil::byteread(i, pos);
	}
};

class QCountingNode { //store info of node as well as string under it.
public:

	int strNum ;

	vector<int> * strSet_p = NULL;

	inline vector<int> & strIdx() const {
		return *strSet_p;
	}

	inline void strSet(set<int> s) {
		strSet_p = new vector<int>(s.begin(),s.end());
	}

	inline void serialize(ostream & o) const {
		SerializationUtil::bytewrite(o, strNum);
		if (strSet_p != NULL) {
			bool flag = true;
			SerializationUtil::bytewrite(o, flag);
			SerializationUtil::serializeVector(o, *strSet_p);
		}
		else {
			bool flag = false;
			SerializationUtil::bytewrite(o, flag);
		}
	}

	inline void deserialize(istream & i) {
		bool flag;
		SerializationUtil::byteread(i, strNum);
		SerializationUtil::byteread(i, flag);
		if (flag) {
			strSet_p = new vector<int>;
			SerializationUtil::deserializeVector(i, *strSet_p);
		}
		else {
			strSet_p = NULL;
		}
	}

	//no deconstruction function

};

class QSuffixTree { // suffix tree only for query
public:
	vector<QTreeNode> nodes;
	vector<QChildren> children;
	vector<QCountingNode> cnodes;

	vector<int> theString;
	vector<string> strs;

	bool preserveString;
	int strNum;
	double threshold ;
	double budget = -1.0;
	double sample_rate = 0.01;
	int leafNodesNum;

	QSuffixTree() {}

	QSuffixTree(int strNum,bool preserveString):strNum(strNum),preserveString(preserveString) {}

	void serialize(ostream & o) {
		

		SerializationUtil::bytewrite(o, preserveString);
		SerializationUtil::bytewrite(o, strNum);
		SerializationUtil::bytewrite(o, threshold);
		SerializationUtil::bytewrite(o, budget);
		SerializationUtil::bytewrite(o, sample_rate);
		SerializationUtil::bytewrite(o, leafNodesNum);
		SerializationUtil::bytewrite(o, withCache);

		SerializationUtil::serializeVector(o, theString);
		SerializationUtil::serializeVector(o, strs);

		SerializationUtil::serializeStruct(o ,nodes);
		SerializationUtil::serializeStruct(o, children);

		SerializationUtil::serializeStruct(o, cnodes);

	}

	void deserialize(istream & i) {

		

		SerializationUtil::byteread(i, preserveString);
		SerializationUtil::byteread(i, strNum);
		SerializationUtil::byteread(i, threshold);
		SerializationUtil::byteread(i, budget);
		SerializationUtil::byteread(i, sample_rate);
		SerializationUtil::byteread(i, leafNodesNum);
		SerializationUtil::byteread(i, withCache);

		SerializationUtil::deserializeVector(i, theString);
		SerializationUtil::deserializeVector(i, strs);

		SerializationUtil::deserializeStruct(i, nodes);
		SerializationUtil::deserializeStruct(i, children);
		SerializationUtil::deserializeStruct(i, cnodes);

	}

	void serializeToFile(const char * path) {
		ofstream o(path,ofstream::out | ofstream ::binary);
		this->serialize(o);
		o.close();
	}

	void deserializeFromFile(const char * path) {
		ifstream i;
		i.open(path, ifstream::in | ifstream::binary);
		if (!i) {
			cout << "cannot open" << endl;
			return;
		}
		this->deserialize(i);
		i.close();
	}

	int findChildren(const QTreeNode & node, int ch) {
		auto begin = children.begin() + node.childrenIdx;
		auto end = begin + node.lengthChildren;
		auto result = lower_bound(begin, end, ch, [](const QChildren & a, const int ch) {
			return a.ch < ch;
		});
		if (result == end || result->ch != ch) { // not found
			return -1;
		}
		else {
			return (int)(result - children.begin());
		}
	}

	template<bool withCache>
	void traverseAdd(set<int> & s, const int & pos) {
		const QTreeNode & node = nodes[pos];
		//cout << " at cnode " << cnodes[pos].strNum << endl;
		if (withCache) {
			//cout << " cached cnode " << cnodes[pos].strNum << endl;
			const QCountingNode & cnode = cnodes[pos];
			if (cnode.strSet_p) {
				s.insert(cnode.strSet_p->begin(), cnode.strSet_p->end());
				return;
			}
		}

		if (node.isLeaf() ) { 
			int lastch = theString[node.end() - 1];
			s.insert((-lastch) - 1); //make negative to positive and index starts from 0
			return;
		}
		auto begin = children.begin() + node.childrenIdx;
		auto end = begin + node.lengthChildren;
		for (auto i = begin; i != end; i++) {
			traverseAdd<withCache>(s, i->pos);
		}
	}

	vector<int> findSubStringIdx(const string & s) {
		set<int> resultset;
		vector<int> result;
		int pos = 0; //make it at root
		int edgeLen = 0;
		for (auto ch0 : s) {
			int ch = (int)ch0;
			QTreeNode & node = nodes[pos];
			//1. end of current edge, check edge go to next node,
			//2. in the middle of an edge, the last char should be leaf
			if (edgeLen == node.lengthStr || pos == 0) {
				if (node.isLeaf()) return result;
				int childpos = findChildren(node,ch);
				if (childpos < 0) return result;
				pos = children[childpos].pos;
				edgeLen = 1;
			}
			else {
				auto chcmp = theString[node.start + edgeLen];
				if (chcmp != ch) {
					return result;
				}
				edgeLen += 1;
			}
		}
		//cout << " at node children: " << nodes[pos].lengthChildren << " " << nodes[pos].lengthStr << endl;

		int found;
		if (this->withCache) {
			traverseAdd<true>(resultset, pos);
			found = cnodes[pos].strNum;
		}
		else {
			traverseAdd<false>(resultset, pos);
			found = (int)result.size();
		}
		result.insert(result.begin(), resultset.begin(), resultset.end());
		return result;
	}

	vector<string> findSubString(const string & s) {
		if (!preserveString) {
			throw runtime_error(string(" should not use this function if preserveString is false "));
		}
		vector<string> && result{};
		auto idx = findSubStringIdx(s);
		for (auto i : idx) {
			result.push_back(strs[i]);
		}	
		return result;
	}

	/*
		budget represents the memory cost, since caching needs memory to store indices on leaf node.
		budget = budgetRate * nodes.size()
	*/
	void cacheIntermediateNode(double budgetRate) { 

		this->budget = budgetRate * nodes.size();
		cnodes = vector<QCountingNode> (nodes.size(), QCountingNode());
		setOpCost = vector<float>(nodes.size(), 0.0f);
		setOpCostStatistics = vector<float>(nodes.size(), 0.0f);
		set<int> myset;
		this->statistics_sample_limit = (int)(5.0 * sqrt((double)nodes.size())) ;
		this->threshold = 5.0; //set initial ratio
		histogramIdx.reserve((int)(nodes.size() * sample_rate) * 2);
		nonLeaf_left = (int)nodes.size() - leafNodesNum;
		cacheNode(myset, 0, 0);
		this->withCache = true;
		this->histogramIdx.erase(histogramIdx.begin(), histogramIdx.end());
		this->histogramIdx.shrink_to_fit();
		cnodes[0].strSet(myset);
	}

	void clearStatistics() {
		setOpCostStatistics.erase(setOpCostStatistics.begin(), setOpCostStatistics.end());
		setOpCost.erase(setOpCost.begin(), setOpCost.end());
		setOpCostStatistics.shrink_to_fit();
		setOpCost.shrink_to_fit();
	}

	string histogramNodeCounting() {
		stringstream strBuffer;
		auto t = histogram<true>([this](const int & pos) {return cnodes[pos].strNum; },
			{ [](const int & pos) {return 1; },
			  [this](const int & pos) {return cnodes[pos].strSet_p ? 1 : 0; },
			  [this](const int & pos) {return cnodes[pos].strNum; },
			  [this](const int & pos) {return getOpCost(pos); }
			},50, 1e6f, 3, 1.5f);


		auto bin = get<0>(t);
		auto hist = get<1>(t);
		strBuffer << "strNum | \t count \t cached \t strNum \t setOpCost" << endl;
		for (auto i = 0; i < bin.size(); i++) {
			strBuffer << " strNum " << bin[i] ;
			for (auto j = 0; j < hist.size(); j++) {
				strBuffer << "\t" << hist[j][i];
			}
			strBuffer << endl;
		}

		t = histogram<true>([this](const int & pos) {return getOpCost(pos); },
			{ [](const int & pos) {return 1; },
			  [this](const int & pos) {return cnodes[pos].strSet_p ? 1 : 0; },
			  [this](const int & pos) {return cnodes[pos].strNum; },
			  [this](const int & pos) {return getOpCost(pos); }
			}, 1000, 1e6f, 3, 1.3f);
		bin = get<0>(t);
		hist = get<1>(t);

		strBuffer << "setOpCost | \t count \t cached \t setOpCost \t strNum" << endl;
		for (auto i = 0; i < bin.size(); i++) {
			strBuffer << " setOpCost " << bin[i];
			for (auto j = 0; j < hist.size(); j++) {
				strBuffer << "\t" << hist[j][i];
			}
			strBuffer << endl;
		}
		return strBuffer.str();
	}

	bool isWithCache() {
		return this->withCache;
	}

	virtual ~QSuffixTree() {
		if (withCache) {
			for (auto & i : cnodes) {
				if (i.strSet_p)
					delete i.strSet_p;
			}
		}
	}

private:

	int statistics_sample_count = 0;
	int statistics_sample_limit;
	int statistics_total_cost;
	int statistics_total_strNum;
	int nonLeaf_left;
	double cache_used = 0.0;
	vector<int> histogramIdx ;
	vector<float> setOpCost;
	vector<float> setOpCostStatistics;

	int cached, last_estimateCached;
	bool withCache = false;



	inline float thresholdCalc(const int & pos) {
		return setOpCostStatistics[pos] * 1.0f / (cnodes[pos].strNum + 2);
	}

	inline float getOpCostStatistics(const int & pos) {
		return setOpCostStatistics[pos];
	}

	inline float getOpCost(const int & pos) {
		return setOpCost[pos];
	}


	void updateStrategy() {
		//double ratio = statistics_total_cost * 1.0 / statistics_total_strNum;

		double budgetLeft = budget - cache_used;
		double weightLeft = statistics_total_strNum * 1.0 * nonLeaf_left; //  div statistics_total_strNum
		double gainLeft = statistics_total_cost * 1.0* nonLeaf_left; // div statistics_total_strNum

		// create histogram, and find the boudary where weight not exceed the budget
		auto t = histogram<false>(
			[this](const int & pos) {return (int)(thresholdCalc(pos)* 10.0f ); }, //make 10 time larger
			{ [](const int & pos) {return 1; },
			  [this](const int & pos) {return cnodes[pos].strNum; },
			  [this](const int & pos) {return getOpCost(pos); }
			}, 1000, 1000.0f, 1, 1.1f,10); //due to 10 time larger

		auto bin = get<0>(t);
		auto hist = get<1>(t);
		vector<int> accsum,accNodeNum;
		accsum.reserve( bin.size() + 1 );
		accNodeNum.reserve(bin.size() + 1);

		accsum.push_back(0); 
		accNodeNum.push_back(0); //leaf node is not in histogram

		for (auto & i : hist[1])  // strNum
			accsum.push_back(*(accsum.end() - 1) + i);
		for (auto & i : hist[0])  // strNum
			accNodeNum.push_back(*(accNodeNum.end() - 1) + i );

		//------------ debug histogram ----------
		//cout << "binsize " << bin.size() << endl;
		//cout << "ratio |\t count \t strNum \t setOpCost" << endl;
		//for (auto i = 0; i < bin.size(); i++) {
		//	cout << " ratio " << bin[i];
		//	for (auto j = 0; j < hist.size(); j++) {
		//		cout << "\t" << hist[j][i];
		//	}
		//	cout << endl;
		//}
		//------------ debug histogram ----------


		auto lastAccSum = *(accsum.end() - 1);
		auto lastAccNodeNum = *(accNodeNum.end() - 1);

		float boundary = 1e9; //infinity

		double factor = 1.0;
		if (cached > 10)
			factor = 0.5 * sqrt( 1.0 *last_estimateCached / cached ) + 0.5;

		for (int i = 1; i < bin.size() ; i++) {
			auto diffWeight = lastAccSum - accsum[i];/// = avg weight * node num

			double estimateWeight = 1.0 * diffWeight / lastAccNodeNum * nonLeaf_left;

			if (estimateWeight < factor * budgetLeft) {
				int estimateCached = lastAccNodeNum - accNodeNum[i];

				boundary = bin[i] / 10.0f; //due to 10 time larger
				last_estimateCached = estimateCached;

				//cout <<"factor "<< factor << " bin[i] " << bin[i] << " diffWeight " << diffWeight << " estimateWeight " << estimateWeight << " budgetLeft " << budgetLeft << endl;
				//cout << " cached " << cached << " estimate cache " << estimateCached << endl;
				// adjust estimate cache number
				break;
			}
		}

		if ( threshold > 3.0 && boundary < 1e8 && threshold < 1e8)
			threshold = 0.7 * boundary + 0.3 * threshold;//momentum
		else 
			threshold = boundary;


		statistics_total_cost = 0;
		statistics_total_strNum = 0;

		if (statistics_sample_limit <= (cnodes.size() - leafNodesNum) * sample_rate)
			statistics_sample_limit *= 2;
		if (histogramIdx.size() > (cnodes.size() - leafNodesNum ) / 10 )
			histogramIdx.erase(histogramIdx.begin(), histogramIdx.begin() + histogramIdx.size());
		// remove half histogram
		
		//cout << "threshold " << threshold << "  budget " << budget << " cache_used " << cache_used << " budgetLeft "<< budgetLeft << " nodes_left "<< nonLeaf_left << endl; //debug

		threshold = threshold > 3.0 ? threshold : 3.0;

		statistics_sample_count = 0;
		cached = 0;
	}

	/*
		online tuning:
		1. Each node has gain (the setopcost), and weight (strNum), 
		2. Without exceeding the budget (weight limit), what's the biggest gain
		3. It's like a knapsack problem.
		4. Solve it with greedy approach.
		5. keep a ratio of gain/weight, as long as a node's ratio is larger than it, 
		   it don't go beyong budget and gain the maximum(not exactly).
        6. however we cannot foresee the ratio, it has to be estimated all the time.

	*/
	void cacheNode(set<int> & parentSet, const int & pos, const int depth) {
		const QTreeNode & node = nodes[pos];
		QCountingNode & cnode = cnodes[pos];
		//cnode.depth = depth;

		if (node.isLeaf() ) { // end == 0 means root
			int lastch = theString[node.end() - 1];
			setOpCost[pos] = 1.0f;
			//setOpCostStatistics[pos] = setOpCost[pos]; //not necessary
			cnode.strNum = 1;
			this->statistics_total_strNum += cnode.strNum;
			parentSet.insert((-lastch) - 1); //make negative to positive and index starts from 0
			return;
		}
		statistics_sample_count += 1;
		if (statistics_sample_count >= statistics_sample_limit)
			updateStrategy();

		shared_ptr<set< int >> myset_p(new set<int>);
		set<int> & myset = *myset_p;
		setOpCost[pos] = 1.0f;
		auto begin = children.begin() + node.childrenIdx;
		auto end = begin + node.lengthChildren;
		for (auto i = begin; i != end; i++) {
			cacheNode(myset, i->pos, depth + 1);
			setOpCost[pos] += setOpCost[i->pos];
		}
		setOpCost[pos] += 3.0f * node.lengthChildren; // each children contribute cost

		cnode.strNum = (int)myset.size();
		setOpCost[pos] += (float)(cnode.strNum / 2); // self size as cost
		parentSet.insert(myset.begin(), myset.end());

		setOpCostStatistics[pos] = setOpCost[pos];

		this->statistics_total_strNum += cnode.strNum;

		double ratio = setOpCost[pos] * 1.0 / cnode.strNum;
		//cout << ratio <<" "<< cnode.setOpCost<<"  "<< cnode.strNum << endl;
		bool cache = this->thresholdCalc(pos) >= threshold ;
		//cache = false;
		if (cache) {
			cache_used += (double)cnode.strNum;
			cached += 1;
			//cnode.strSet_p = myset_p;
			cnode.strSet(*myset_p);

			setOpCost[pos] = (float)cnode.strNum; //reset cost to set size.

		}
		histogramIdx.push_back( pos );
		nonLeaf_left -= 1;
	}

	/*
		return bin boundary(start), and counting result
	*/
	template<bool histogramAll>
	std::tuple<vector<int>, vector<vector<int>>> histogram(
		std::function<int(const int &)> funcGet,
		vector<std::function<int(const int &)>> funcUpdates,
		int bins, float binMax, int binSize, float factor, int offset = 0) {
		// linear and exp cut bin
		vector<int> bin;
		float expbin = 1;
		for (int i = 0; i < bins; i++) {
			int temp = binSize * i + (int)expbin + offset;
			if (temp > binMax) break;
			expbin =  factor * expbin ;
			bin.push_back(temp);
		}
		int n = (int)bin.size();
		vector<vector<int>> histograms(funcUpdates.size(), vector<int>(n,0));
		//for (const auto & i : this->cnodes) 
		int i0_top = histogramAll ? (int)nodes.size() : (int)histogramIdx.size();
		for (int i0 = 0; i0 < i0_top;i0++ ) {
			int k = histogramAll ? i0 : histogramIdx[i0];
			auto i = cnodes[k];
			auto it = lower_bound(bin.begin(), bin.end(), funcGet(k));
			if (it != bin.begin())
				it--;
			int idx = (int)(it - bin.begin());
			for (int j=0;j<funcUpdates.size();j++)
				histograms[j][idx] += funcUpdates[j](k);
		}
		return make_tuple(bin, histograms);
	}


};
