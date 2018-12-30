#include "pch.h"


#ifdef _WIN32
	#define MYDLL __declspec(dllexport)
#else
	#define MYDLL
#endif


#include <string>
#include <vector>
#include "interface.h"

#include <Python.h>
#include <iostream>
#include <sstream>

using namespace std;

void deconstructSuffixTree(PyObject *capsule) {
	auto * x = PyCapsule_GetPointer(capsule, "SuffixTree");
	freeSuffixTreePointer(x);
}

void deconstructSuffixQueryTree(PyObject *capsule) {
	auto * x = PyCapsule_GetPointer(capsule, "SuffixQueryTree");
	freeSuffixQueryTreePointer(x);
}

string pyString_toString(PyObject *s) {
	auto *sobj = PyUnicode_AsUTF8String(s);  //automatically deref when return
	const char *c = PyBytes_AsString(sobj);
	return string(c);
}

vector<string> listString_toVector(PyObject *list) {
	PyGILState_STATE gstate = PyGILState_Ensure();
	auto x = PySequence_Length(list);
	vector<string> strs ;
	strs.reserve((int)x);
	for (int i = 0; i < (int)x; i++) {
		auto *obj = PySequence_GetItem(list,i);
		// turn obj to string

		string s = pyString_toString(obj);
		strs.push_back(s);

		Py_DECREF(obj);
	}
	PyGILState_Release(gstate);
	return strs;
}

PyObject * vectorInt_toPyList(const vector<int> & v) {
	PyObject * list = PyList_New(0);
	for (const auto & i : v) {
		PyObject * number = PyLong_FromLong(i);
		PyList_Append(list, number);
	}
	return list;
}

PyObject * vectorString_toPyList(const vector<string> & v) {
	PyObject * list = PyList_New(0);
	for (const auto & i : v) {
		PyObject * s = PyUnicode_FromString(i.c_str());
		PyList_Append(list, s);
	}
	return list;
}

extern "C" MYDLL PyObject * createSuffixTreePy(PyObject *list, bool persistString) {
	vector<string> strs = listString_toVector(list);
	void *p = (void *)createSuffixTreePointer(strs, persistString);
	return PyCapsule_New(p, "SuffixTree", deconstructSuffixTree);
}


/*
	take list of strings
*/
extern "C" MYDLL PyObject * createSuffixQueryTreePy(
							PyObject *list,
							bool persistString) {

	vector<string> strs = listString_toVector(list);
	auto *p = createSuffixTreePointer(strs, persistString);
	auto *p_qtree = createSuffixQueryTreePointer(p);
	freeSuffixTreePointer(p);
	return PyCapsule_New((void *)p_qtree, "SuffixQueryTree", deconstructSuffixQueryTree);
}


/*
	take list of strings
*/
extern "C" MYDLL PyObject * createSuffixQueryTreePyWithCache(
								PyObject *list, 
								bool persistString,
								double budgetRatio,
								double sampleRate) {

	vector<string> strs = listString_toVector(list);
	auto *p = createSuffixTreePointer(strs, persistString);
	auto *p_qtree = createSuffixQueryTreePointer(p);
	freeSuffixTreePointer(p);
	cacheIntermediateNode(p_qtree,budgetRatio, sampleRate);
	return PyCapsule_New((void *)p_qtree, "SuffixQueryTree", deconstructSuffixQueryTree);
}

/*
	return list of int
*/
extern "C" MYDLL PyObject * findStringIdxPy(PyObject * tree_capsule, PyObject *pys) {
	auto *tree = PyCapsule_GetPointer(tree_capsule, "SuffixTree");

	string && s = pyString_toString(pys);
	auto idx = findStringIdx(tree, s);
	return vectorInt_toPyList(idx);
}

/*
	return list of string
*/
extern "C" MYDLL PyObject * findStringPy(PyObject * tree_capsule, PyObject *pys) {
	auto *tree = PyCapsule_GetPointer(tree_capsule, "SuffixTree");
	string s = pyString_toString(pys);
	try {
		auto strs = findString(tree, s);
		return vectorString_toPyList(strs);
	}
	catch (std::exception e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		return NULL;
	}
}

extern "C" MYDLL void suffixTreeAddStringsPy(PyObject * tree_capsule, PyObject *list) {
	auto *tree = PyCapsule_GetPointer(tree_capsule, "SuffixTree");
	vector<string> strs = listString_toVector(list);
	suffixTreeAddStrings(tree, strs);
}

extern "C" MYDLL void suffixTreeAddStringPy(PyObject * tree_capsule, PyObject *pys) {
	auto *tree = PyCapsule_GetPointer(tree_capsule, "SuffixTree");
	string s = pyString_toString(pys);
	suffixTreeAddString(tree, s);
}

/*
	return list of int
*/
extern "C" MYDLL PyObject * findStringIdx_qtreePy(PyObject * qtree_capsule, PyObject *pys) {
	auto *tree = PyCapsule_GetPointer(qtree_capsule, "SuffixQueryTree");

	string && s = pyString_toString(pys);
	auto idx = findStringIdx_QTree(tree, s);
	return vectorInt_toPyList(idx);
}

/*
	return list of string
*/
extern "C" MYDLL PyObject * findString_qtreePy(PyObject * qtree_capsule, PyObject *pys) {
	auto *tree = PyCapsule_GetPointer(qtree_capsule, "SuffixQueryTree");
	string s = pyString_toString(pys);
	auto state = PyGILState_Ensure();
	try {
		auto strs = findString_QTree(tree, s);
		PyGILState_Release(state);
		return vectorString_toPyList(strs);
	}
	catch (std::exception e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		PyGILState_Release(state);
		return NULL;
	}
}

/*
	take suffix tree
	return capsule of qtree,
*/
extern "C" MYDLL PyObject * SuffixTree_toQueryTree(PyObject * tree_capsule) {
	auto *p = PyCapsule_GetPointer(tree_capsule, "SuffixTree");
	auto *p_qtree = createSuffixQueryTreePointer(p);
	return PyCapsule_New(p_qtree, "SuffixQueryTree", deconstructSuffixQueryTree);
}

/*
	return statistics report string
*/
extern "C" MYDLL PyObject * cacheIntermediateNodePy(PyObject * qtree_capsule, double budgetRatio, double sampleRate) {
	auto *p = PyCapsule_GetPointer(qtree_capsule, "SuffixQueryTree");
	cacheIntermediateNode(p, budgetRatio, sampleRate);
	string s = showStatistics(p);
	clearStatistics(p);

	return PyUnicode_FromString(s.c_str());
}

extern "C" MYDLL void saveSuffixQueryTreeToFilePy(PyObject * qtree_capsule, const char *path) {
	auto *p = PyCapsule_GetPointer(qtree_capsule, "SuffixQueryTree");
	saveSuffixQueryTreeToFile(p, path);
}

extern "C" MYDLL PyObject * saveSuffixQueryTreePy(PyObject * qtree_capsule) {
	auto *p = PyCapsule_GetPointer(qtree_capsule, "SuffixQueryTree");
	ostringstream ss(ostream::binary | ostream::out) ;
	saveSuffixQueryTree(p, ss);
	auto  s = ss.str();
	return PyBytes_FromStringAndSize(s.c_str(), s.length());
}

extern "C" MYDLL PyObject * readSuffixQueryTreeFromFilePy(const char *path) {
	auto *p_qtree = emptySuffixQueryTreePointer();
	readSuffixQueryTreeFromFile(p_qtree, path);
	return PyCapsule_New((void *)p_qtree, "SuffixQueryTree", deconstructSuffixQueryTree);
}

extern "C" MYDLL PyObject * readSuffixQueryTreePy(PyObject* pybytes) {
	auto *p_qtree = emptySuffixQueryTreePointer();
	char *c_str ;
	Py_ssize_t c_str_len;
	PyBytes_AsStringAndSize(pybytes, &c_str, &c_str_len);
	int len = (int)c_str_len;
	string s(c_str, len);

	istringstream ss(s, istream::binary | istream::in);
	readSuffixQueryTree(p_qtree, ss);
	return PyCapsule_New((void *)p_qtree, "SuffixQueryTree", deconstructSuffixQueryTree);
}

vector<Charset> translateList(PyObject *list_charset) {
	vector<Charset> chars;
	auto lLen = PySequence_Length(list_charset);
	PyGILState_STATE state = PyGILState_Ensure();

	chars.reserve((int)lLen);
	for (int i = 0; i < (int)lLen; i++) {
		auto *obj = PySequence_GetItem(list_charset, i);
		// turn obj to string

		if (PyNumber_Check(obj) == 1) { // is a number
			auto && c = Charset();
			int number = (int)PyLong_AsLong(obj);
			//Did not check error
			if (number == (int)'^')
				c.specialChar = CHAR_STRING_START;
			else if (number == (int)'$')
				c.specialChar = CHAR_STRING_END;
			else if (number == (int)'.')
				c.specialChar = CHAR_ANY;
			else if (number == (int)'w')
				c.specialChar = CHAR_WORD;
			else {
				//TODO add number in string
				PyErr_SetString(PyExc_RuntimeError, "unknown number ");
			}
			chars.push_back(c);
		}
		else {
			string s = pyString_toString(obj);
			vector<int> ints;
			for (auto & ch: s) {
				ints.push_back((int)ch);
			}
			auto && c = Charset(set<int>(ints.begin(), ints.end()));
			chars.push_back(c);
		}
		Py_DECREF(obj);
	}
	PyGILState_Release(state);

	return chars;
}

//list_charset is a list of string or int, int is considered to be special char
extern "C" MYDLL PyObject * findString_QTree_wildcardPy(PyObject * tree_capsule, PyObject *list_charset) {
	auto *qtree = PyCapsule_GetPointer(tree_capsule, "SuffixQueryTree");
	vector<Charset> chars;
	chars = translateList(list_charset);
	PyGILState_STATE state = PyGILState_Ensure();
	try {
		auto strs = findString_QTree_wildcard(qtree, chars);
		PyGILState_Release(state);
		return vectorString_toPyList(strs);
	}
	catch (std::exception e) {
		PyErr_SetString(PyExc_RuntimeError, e.what());
		PyGILState_Release(state);
		return NULL;
	}
}

//list_charset is a list of list_string, list_string is used for representing charset
extern "C" MYDLL PyObject * findStringIdx_QTree_wildcardPy(PyObject * tree_capsule, PyObject *list_charset) {
	auto *qtree = PyCapsule_GetPointer(tree_capsule, "SuffixQueryTree");
	vector<Charset> chars;
	chars = translateList(list_charset);
	auto idx = findStringIdx_QTree_wildcard(qtree, chars);
	return vectorInt_toPyList(idx);
}

extern "C" MYDLL PyObject * allString_SuffixTreePy(PyObject * tree_capsule) {
	auto *tree = PyCapsule_GetPointer(tree_capsule, "SuffixTree");
	auto strs = allString_SuffixTree(tree);
	return vectorString_toPyList(strs);
}

extern "C" MYDLL PyObject * allString_SuffixQueryTreePy(PyObject * qtree_capsule) {
	auto *qtree = PyCapsule_GetPointer(qtree_capsule, "SuffixQueryTree");
	auto strs = allString_SuffixQueryTree(qtree);
	return vectorString_toPyList(strs);
}