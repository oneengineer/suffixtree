#pragma once
#include "pch.h"
#include <Python.h>
#include <vector>
#include <string>

using namespace std;

inline string pyString_toString(PyObject* s) {
	auto* sobj = PyUnicode_AsUTF8String(s);  //automatically deref when return
	const char* c = PyBytes_AsString(sobj);
	string str(c);
	Py_DECREF(sobj);
	return str;
}

inline vector<string> listString_toVector(PyObject* list) {
	PyGILState_STATE state = PyGILState_Ensure();
	auto x = PySequence_Length(list);
	vector<string> strs;
	strs.reserve((int)x);
	for (int i = 0; i < (int)x; i++) {
		auto* obj = PySequence_GetItem(list, i);
		// turn obj to string

		string s = pyString_toString(obj);
		strs.push_back(s);

		Py_DECREF(obj);
	}
	PyGILState_Release(state);
	return strs;
}

inline PyObject* vectorInt_toPyList(const vector<int>& v) {
	PyGILState_STATE state = PyGILState_Ensure();

	PyObject* list = PyList_New(0);
	for (const auto& i : v) {
		PyObject* number = PyLong_FromLong(i);
		PyList_Append(list, number);
		Py_DECREF(number);
	}
	PyGILState_Release(state);
	return list;
}

inline PyObject* vectorString_toPyList(const vector<string>& v) {
	PyGILState_STATE state = PyGILState_Ensure();

	PyObject* list = PyList_New(0);
	for (const auto& i : v) {
		PyObject* s = PyUnicode_FromString(i.c_str());
		PyList_Append(list, s);
		Py_DECREF(s);

	}
	PyGILState_Release(state);

	return list;
}
