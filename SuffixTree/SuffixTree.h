#pragma once

#include "pch.h"
#include <iostream>
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <string>

#include <memory>

#include <fstream>
#include <chrono>
#include <algorithm>

#include <memory>

#include "SuffixQueryTree.h"


using namespace std;

class TreeNode {
public:
	int start, end;
	int suffixLink; // point to index of suffix node
	using typemap = map<int, int>;
	shared_ptr<typemap> cp;


	inline typemap & children() const {
		return *cp;
	}

	//unordered_map<int, int> children;
	TreeNode() :start(0), end(0), suffixLink(-1), cp(nullptr) {}

	inline int edgeLength() const {
		return end - start;
	}

	inline bool edgeContain(const int &ch) const {
		if (!cp) return false;
		return cp->find(ch) != cp->end();
	}

	inline void edgePut(const int &ch, const int &target) {
		if (isLeaf()) cp.reset(new typemap());
		(*cp)[ch] = target;
	}

	inline int edgeGet(const int &ch) {
		return (*cp)[ch];
	}

	inline bool isLeaf() const {
		return !cp;
	}
};


class SuffixTree {
public:
	vector<TreeNode> nodes;
private:
	int activeNodeIdx;
	int preNodeIdx;
	vector<int> theString; // use int as char
	int activeStart;
	int activeEnd;
	int endPos;
	int stringNum;
	bool preserveString;
	vector<string> strs;

	vector<int> leafAdd;

	inline int activeLength() {
		return activeEnd - activeStart;
	}

	inline int edge() {
		return theString[activeStart];
	}

	inline TreeNode & nodeAt(const int & pos) {
		return nodes[pos];
	}

	inline int edgeLength(const int & nodeIdx) {
		const TreeNode & node = nodeAt(nodeIdx);
		if (node.isLeaf()) return endPos - node.start;
		else return node.edgeLength();
	}

	inline TreeNode & activeNode() {
		return nodes[activeNodeIdx];
	}

	inline int nextNodeIdx() {
		int nodeIdx = activeNode().children()[edge()];
		return nodeIdx;
	}

	inline TreeNode& nextNode() {
		return nodes[nextNodeIdx()];
	}

	inline bool walkThrough(const int & nodeIdx) {
		if (activeLength() >= edgeLength(nodeIdx)) {
			activeNodeIdx = nodeIdx;
			activeStart += edgeLength(nodeIdx);
			return true;
		}
		return false;
	}

	inline void addSuffixLink(const int & nodeIdx) {
		if (preNodeIdx >= 0) {
			nodeAt(preNodeIdx).suffixLink = nodeIdx;
		}
		preNodeIdx = nodeIdx;
	}

	inline void leafAdded() {
		leafAdd.push_back((int)nodes.size() - 1);
	}

	void traverseAdd(set<int> & s, const TreeNode & node) {
		if (node.isLeaf() && node.end != 0) { // end == 0 means root
			auto lastch = theString[node.end - 1];
			s.insert((-lastch) - 1); //make negative to positive and index starts from 0
		}
		if (node.isLeaf()) return;
		for (const auto& kv : node.children()) {
			traverseAdd(s, nodeAt(kv.second));
		}
	}


public:
	SuffixTree(bool preserveString) :
		preserveString(preserveString),
		activeNodeIdx(0),
		preNodeIdx(-1),
		activeStart(0),
		activeEnd(0),
		endPos(0),
		stringNum(0) {
		TreeNode &&t = TreeNode();
		nodes.push_back(t);
	}

	void addChar(int ch) {
		endPos += 1;
		theString.push_back(ch);
		preNodeIdx = -1;
		while (activeStart <= activeEnd) {
			if (!activeNode().edgeContain(edge())) {
				//create leaf
				auto && leaf = TreeNode();
				leaf.start = endPos - 1;
				nodes.push_back(leaf);
				addSuffixLink(activeNodeIdx);
				activeNode().edgePut(edge(), (int)nodes.size() - 1);
				//activeNode().children[edge()] = nodes.size() - 1; // the last one
				leafAdded();
			}
			else {
				auto nextIdx = nextNodeIdx();
				auto & next = nextNode();
				if (walkThrough(nextIdx)) continue;
				int chcmp = this->theString[next.start + activeLength()];
				if (chcmp == ch) {
					addSuffixLink(activeNodeIdx);
					break;
				}
				// ------------ split edge -------------
				auto && split = TreeNode();
				split.start = next.start;
				split.end = next.start + activeLength();
				next.start = split.end;
				auto && leaf = TreeNode();
				leaf.start = endPos - 1;
				nodes.push_back(split);
				int splitIdx = (int)nodes.size() - 1;
				nodes.push_back(leaf);
				leafAdded();

				// ------------ change child-------------
				activeNode().edgePut(edge(), splitIdx);
				nodeAt(splitIdx).edgePut(ch, (int)nodes.size() - 1);
				nodeAt(splitIdx).edgePut(chcmp, nextIdx);

				addSuffixLink(splitIdx);

			} // end of else
			// ----------  move reminder chars --------
			if (activeNodeIdx != 0) { // not root
				if (activeNode().suffixLink >= 0)
					activeNodeIdx = activeNode().suffixLink;
				else
					activeNodeIdx = 0; // move to root
			}
			else {
				activeStart += 1;
			}
		}// end of while
		activeEnd += 1;
	}

	void addString(const string & s) {
		stringNum += 1;
		for (auto it : s) {
			int ch = (int)it;
			this->addChar(ch);
		}
		//add end of string
		this->addChar(-stringNum);
		for (auto x : leafAdd) {
			nodeAt(x).end = (int)theString.size();
		}
		leafAdd.clear();
		if (preserveString)
			strs.push_back(s);
	}

	/*
		return a list of string index
	*/
	vector<int> findSubStringIdx(const string & s) {
		set<int> resultset;
		vector<int> result;
		int pos = 0; //make it at root
		int edgeLen = 0;
		for (auto ch0 : s) {
			int ch = (int)ch0;
			TreeNode & node = nodeAt(pos);
			//1. end of current edge, check edge go to next node,
			//2. in the middle of an edge, the last char should be leaf
			if (edgeLen == node.edgeLength() || pos == 0) {
				if (node.isLeaf()) return result;
				if (!node.edgeContain(ch)) return result;
				pos = node.children()[ch];
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
		traverseAdd(resultset, nodeAt(pos));
		result.insert(result.begin(),resultset.begin(),resultset.end());
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


	void addStrings(const vector<string> & strings) {
		int n = (int)strings.size();
		int size = 0;
		for (const auto & s : strings) {
			size += (int)s.length();
		}

		size = size + 2 * n + 10;
		this->nodes.reserve(this->nodes.size() + size);

		for (const auto & s : strings) {
			this->addString(s);
		}
	}

	QSuffixTree queryTree() const {
		QSuffixTree && qst = QSuffixTree(this->stringNum, this->preserveString);
		qst.nodes.reserve(this->nodes.size());
		qst.children.reserve(this->nodes.size() - 1);
		int children_i = 0;
		qst.leafNodesNum = 0;
		for (const auto & node : this->nodes) {
			if (node.isLeaf())
				qst.leafNodesNum += 1;

			int clen = 0;
			if (!node.isLeaf())
				clen = (int)node.children().size();

			QTreeNode qnode(node.start, node.end, children_i, clen);
			qst.nodes.push_back(qnode);

			if (!node.isLeaf())
				for (const auto& kv : node.children()) {
					QChildren child(kv.first, kv.second);
					qst.children.push_back(child);
					children_i++;
				}
		}
		qst.theString = this->theString;
		if (preserveString)
			qst.strs = this->strs;
		return qst;
	}

	int nodeSize() {
		return (int)nodes.size();
	}
};