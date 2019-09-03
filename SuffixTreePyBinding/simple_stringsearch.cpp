#include "pch.h"


#ifdef _WIN32
#define MYDLL __declspec(dllexport)
#else
#define MYDLL
#endif


#include <string>
#include <vector>

#include <Python.h>
#include "pycommons.h"
#include <iostream>
#include <sstream>
#include <algorithm>

using namespace std;

string str_to_lower(const string & str) {
	string s2 = str;
	std::transform(s2.begin(), s2.end(), s2.begin(),
		[](unsigned char c) { return std::tolower(c); });
	return s2;
}

class SimpleStringSearch {

public:
	vector<string> strs;
	vector<string> lower_strs;
	SimpleStringSearch() {}
	SimpleStringSearch(const vector<string>& strs) {
		this->strs = strs;
		lower_strs.reserve(strs.size());
		//lower_strs.reserve(strs.size());
		std::transform(strs.begin(), strs.end(), std::back_inserter(lower_strs),
			str_to_lower);
	}

	template < bool case_sensitive = true >
	vector<string> findString(string s) const {
		if (!case_sensitive)
			s = str_to_lower(s);
		vector<string> result;
		int c = 0;
		for (const auto& x : (case_sensitive ? strs : lower_strs)) {
			bool found = false;
			found = x.find(s) != string::npos;
			if (found && case_sensitive)  result.push_back(x);
			if (found && !case_sensitive) result.push_back(this->strs[c]);
			c++;
		}
		return result;
	}

	template < bool case_sensitive = true >
	vector<int> findStringIdx(string s) const {
		if (!case_sensitive)
			s = str_to_lower(s);
		vector<int> result;
		int i = 0;
		for (const auto& x :  ( case_sensitive? strs:lower_strs ) ) {
			bool found = false;
			found = x.find(s) != string::npos;
			if (found ) {
				result.push_back(i);
			}
			i++;
		}
		return result;
	}

	// find string contains all characters in s
	template < bool case_sensitive = true >
	vector<string> findString(const vector<string> & list_s) const {
		vector<string> list_s2(list_s);
		if (!case_sensitive)
			transform(list_s2.begin(), list_s2.end(), list_s2.begin(), str_to_lower);
		vector<string> result;
		int c = 0;
		for (const auto& x : (case_sensitive ? strs : lower_strs)) {
			bool flag = true;
			for (const auto& s : list_s2) {
				auto found = x.find(s);
				if (found == string::npos) {
					flag = false;
					break;
				}
			}
			if (flag && case_sensitive )  result.push_back(x);
			if (flag && !case_sensitive ) result.push_back( this->strs[c] );
			c++;
		}
		return result;
	}

	// find string contains all characters in s
	template < bool case_sensitive = true >
	vector<int> findStringIdx(const vector<string>& list_s) const {
		vector<string> list_s2(list_s);
		if (!case_sensitive)
			transform(list_s2.begin(), list_s2.end(), list_s2.begin(), str_to_lower);
		vector<int> result;
		int i = 0;
		for (const auto& x : (case_sensitive ? strs : lower_strs)) {
			bool flag = true;
			for (const auto& s : list_s2) {
				auto found = x.find(s);
				if (found == string::npos) {
					flag = false;
					break;
				}
			}
			if (flag) result.push_back(i);
			i++;
		}
		return result;
	}
};

void deconstructSimpleSearch(PyObject* capsule) {
	auto* x = PyCapsule_GetPointer(capsule, "SimpleStringSearch");
	delete (SimpleStringSearch*) x;
}


extern "C" MYDLL PyObject* createSimpleStringSearch(PyObject* list) {
	vector<string> strs = listString_toVector(list);
	PyGILState_STATE state = PyGILState_Ensure();
	auto* p = new SimpleStringSearch(strs);
	PyObject* result = PyCapsule_New(p, "SimpleStringSearch", deconstructSimpleSearch);
	PyGILState_Release(state);
	return result;
}

extern "C" MYDLL PyObject* simpleSearch_findStringPy(PyObject* search_capsule, PyObject* pys, bool case_sensitive) {
	auto* simpleSearch = PyCapsule_GetPointer(search_capsule, "SimpleStringSearch");
	PyGILState_STATE state = PyGILState_Ensure();
	string s = pyString_toString(pys);
	PyGILState_Release(state);
	SimpleStringSearch* p = (SimpleStringSearch*)simpleSearch;
	auto&& strs = case_sensitive ? p->findString<true>(s) : p->findString<false>(s);
	return vectorString_toPyList(strs);
}


extern "C" MYDLL PyObject* simpleSearch_findStringIdxPy(PyObject* search_capsule, PyObject* pys, bool case_sensitive) {
	auto* simpleSearch = PyCapsule_GetPointer(search_capsule, "SimpleStringSearch");
	PyGILState_STATE state = PyGILState_Ensure();
	string s = pyString_toString(pys);
	PyGILState_Release(state);
	SimpleStringSearch* p= (SimpleStringSearch*)simpleSearch;
	auto && idx = case_sensitive ? p->findStringIdx<true>(s) : p->findStringIdx<false>(s);
	return vectorInt_toPyList(idx);
}



extern "C" MYDLL PyObject* simpleSearch_findStringPy_list(PyObject* search_capsule, PyObject* list, bool case_sensitive) {
	auto* simpleSearch = PyCapsule_GetPointer(search_capsule, "SimpleStringSearch");
	PyGILState_STATE state = PyGILState_Ensure();
	vector<string> list_s = listString_toVector(list);
	PyGILState_Release(state);
	SimpleStringSearch* p = (SimpleStringSearch*)simpleSearch;
	auto&& strs = case_sensitive ? p->findString<true>(list_s) : p->findString<false>(list_s);
	return vectorString_toPyList(strs);
}


extern "C" MYDLL PyObject* simpleSearch_findStringIdxPy_list(PyObject* search_capsule, PyObject* list, bool case_sensitive) {
	auto* simpleSearch = PyCapsule_GetPointer(search_capsule, "SimpleStringSearch");
	PyGILState_STATE state = PyGILState_Ensure();
	vector<string> list_s = listString_toVector(list);
	PyGILState_Release(state);
	SimpleStringSearch* p = (SimpleStringSearch*)simpleSearch;
	auto&& idx = case_sensitive ? p->findStringIdx<true>(list_s) : p->findStringIdx<false>(list_s);
	return vectorInt_toPyList(idx);
}


