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
	PyGILState_STATE state = PyGILState_Ensure();
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
	PyGILState_Release(state);
	return strs;
}

PyObject * vectorInt_toPyList(const vector<int> & v) {
	PyGILState_STATE state = PyGILState_Ensure();

	PyObject * list = PyList_New(0);
	for (const auto & i : v) {
		PyObject * number = PyLong_FromLong(i);
		PyList_Append(list, number);
	}
	PyGILState_Release(state);
	return list;
}

PyObject * vectorString_toPyList(const vector<string> & v) {
	PyGILState_STATE state = PyGILState_Ensure();

	PyObject * list = PyList_New(0);
	for (const auto & i : v) {
		PyObject * s = PyUnicode_FromString(i.c_str());
		PyList_Append(list, s);
	}
	PyGILState_Release(state);

	return list;
}


vector<Charset> translateList(PyObject *list_charset) {
	vector<Charset> chars;
	PyGILState_STATE state = PyGILState_Ensure();
	auto lLen = PySequence_Length(list_charset);

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
			for (auto & ch : s) {
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


extern "C" MYDLL PyObject * createSuffixTreePy(PyObject *list, bool persistString) {
	vector<string> strs = listString_toVector(list);
	void *p = (void *)createSuffixTreePointer(strs, persistString);
	PyGILState_STATE state = PyGILState_Ensure();
	PyObject *result = PyCapsule_New(p, "SuffixTree", deconstructSuffixTree);
	PyGILState_Release(state);
	return result;
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
	PyGILState_STATE state = PyGILState_Ensure();
	PyObject *result = PyCapsule_New((void *)p_qtree, "SuffixQueryTree", deconstructSuffixQueryTree);
	PyGILState_Release(state);
	return result;
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
	PyGILState_STATE state = PyGILState_Ensure();
	PyObject *result = PyCapsule_New((void *)p_qtree, "SuffixQueryTree", deconstructSuffixQueryTree);
	PyGILState_Release(state);
	return result;
}

/*
	return list of int
*/
extern "C" MYDLL PyObject * findStringIdxPy(PyObject * tree_capsule, PyObject *pys) {
	auto *tree = PyCapsule_GetPointer(tree_capsule, "SuffixTree");
	PyGILState_STATE state = PyGILState_Ensure();
	string s = pyString_toString(pys);
	PyGILState_Release(state);
	auto idx = findStringIdx(tree, s);
	return vectorInt_toPyList(idx);
}

/*
	return list of string
*/
extern "C" MYDLL PyObject * findStringPy(PyObject * tree_capsule, PyObject *pys) {
	auto *tree = PyCapsule_GetPointer(tree_capsule, "SuffixTree");
	PyGILState_STATE state = PyGILState_Ensure();
	string s = pyString_toString(pys);
	PyGILState_Release(state);
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
	PyGILState_STATE state = PyGILState_Ensure();
	string s = pyString_toString(pys);
	PyGILState_Release(state);
	suffixTreeAddString(tree, s);
}

/*
	return list of int
*/
extern "C" MYDLL PyObject * findStringIdx_qtreePy(PyObject * qtree_capsule, PyObject *pys, bool case_sensitive) {
	auto *tree = PyCapsule_GetPointer(qtree_capsule, "SuffixQueryTree");

	PyGILState_STATE state = PyGILState_Ensure();
	string s = pyString_toString(pys);
	PyGILState_Release(state);
	auto idx = findStringIdx_QTree(tree, s, case_sensitive);
	return vectorInt_toPyList(idx);
}

/*
	return list of string
*/
extern "C" MYDLL PyObject * findString_qtreePy(PyObject * qtree_capsule, PyObject *pys, bool case_sensitive) {
	auto *tree = PyCapsule_GetPointer(qtree_capsule, "SuffixQueryTree");
	PyGILState_STATE state = PyGILState_Ensure();
	string s = pyString_toString(pys);
	PyGILState_Release(state);
	try {
		auto strs = findString_QTree(tree, s, case_sensitive);
		return vectorString_toPyList(strs);
	}
	catch (std::exception e) {
		state = PyGILState_Ensure();
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
	PyGILState_STATE state = PyGILState_Ensure();
	PyObject *result = PyCapsule_New(p_qtree, "SuffixQueryTree", deconstructSuffixQueryTree);
	PyGILState_Release(state);
	return result;
}

/*
	return statistics report string
*/
extern "C" MYDLL PyObject * cacheIntermediateNodePy(PyObject * qtree_capsule, double budgetRatio, double sampleRate) {
	auto *p = PyCapsule_GetPointer(qtree_capsule, "SuffixQueryTree");
	cacheIntermediateNode(p, budgetRatio, sampleRate);
	string s = showStatistics(p);
	clearStatistics(p);
	PyGILState_STATE state = PyGILState_Ensure();
	PyObject * result = PyUnicode_FromString(s.c_str());
	PyGILState_Release(state);
	return result;
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
	PyGILState_STATE state = PyGILState_Ensure();
	PyObject * result = PyBytes_FromStringAndSize(s.c_str(), s.length());
	PyGILState_Release(state);
	return result;
}

extern "C" MYDLL PyObject * readSuffixQueryTreeFromFilePy(const char *path) {
	auto *p_qtree = emptySuffixQueryTreePointer();
	readSuffixQueryTreeFromFile(p_qtree, path);
	PyGILState_STATE state = PyGILState_Ensure();
	PyObject *result = PyCapsule_New((void *)p_qtree, "SuffixQueryTree", deconstructSuffixQueryTree);
	PyGILState_Release(state);
	return result;
}

extern "C" MYDLL PyObject * readSuffixQueryTreePy(PyObject* pybytes) {
	auto *p_qtree = emptySuffixQueryTreePointer();
	char *c_str ;
	Py_ssize_t c_str_len;
	PyGILState_STATE state = PyGILState_Ensure();
	PyBytes_AsStringAndSize(pybytes, &c_str, &c_str_len);
	PyGILState_Release(state);
	int len = (int)c_str_len;
	string s(c_str, len);

	istringstream ss(s, istream::binary | istream::in);
	readSuffixQueryTree(p_qtree, ss);
	state = PyGILState_Ensure();
	PyObject *result = PyCapsule_New((void *)p_qtree, "SuffixQueryTree", deconstructSuffixQueryTree);
	PyGILState_Release(state);
	return result;
}

//list_charset is a list of string or int, int is considered to be special char
extern "C" MYDLL PyObject * findString_QTree_wildcardPy(PyObject * tree_capsule, PyObject *list_charset, bool case_sensitive) {
	auto *qtree = PyCapsule_GetPointer(tree_capsule, "SuffixQueryTree");
	vector<Charset> chars;
	chars = translateList(list_charset);
	try {
		auto strs = findString_QTree_wildcard(qtree, chars, case_sensitive);
		return vectorString_toPyList(strs);
	}
	catch (std::exception e) {
		PyGILState_STATE state = PyGILState_Ensure();
		PyErr_SetString(PyExc_RuntimeError, e.what());
		PyGILState_Release(state);
		return NULL;
	}
}

//list_charset is a list of list_string, list_string is used for representing charset
extern "C" MYDLL PyObject * findStringIdx_QTree_wildcardPy(PyObject * tree_capsule, PyObject *list_charset, bool case_sensitive) {
	auto *qtree = PyCapsule_GetPointer(tree_capsule, "SuffixQueryTree");
	vector<Charset> chars;
	chars = translateList(list_charset);
	auto idx = findStringIdx_QTree_wildcard(qtree, chars, case_sensitive);
	auto result = vectorInt_toPyList(idx);
	return result;
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