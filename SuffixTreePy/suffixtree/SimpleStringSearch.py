from ctypes import *
import os
import pathlib

if os.name == 'nt':
    dllpath = os.path.dirname(os.path.abspath(__file__)) + "/SuffixTreePyBinding.dll"
    #dllpath = os.path.dirname(os.path.abspath(__file__)) + r"\..\..\x64\Release\SuffixTreePyBinding.dll"
    
else:
    dllpath = os.path.dirname(os.path.abspath(__file__)) + "/libSuffixTreePyBinding.so"


class SimpleStringSearch(object):
    """description of class"""

    def __init__(self,strs:list):
        
        lib = cdll.LoadLibrary(dllpath)

        lib.simpleSearch_findStringPy.argtypes = [ py_object,py_object, c_bool] # string_capsule string, string
        lib.simpleSearch_findStringPy.restype = py_object # list string

        lib.simpleSearch_findStringIdxPy.argtypes = [ py_object,py_object, c_bool] # string_capsule string, string
        lib.simpleSearch_findStringIdxPy.restype = py_object # list int

        lib.simpleSearch_findStringPy_list.argtypes = [ py_object,py_object, c_bool] # string_capsule string, list string
        lib.simpleSearch_findStringPy_list.restype = py_object # list string

        lib.simpleSearch_findStringIdxPy_list.argtypes = [ py_object,py_object, c_bool] # string_capsule string, list string
        lib.simpleSearch_findStringIdxPy_list.restype = py_object # list int

        # ---- init ---
        lib.createSimpleStringSearch.argtypes = [ py_object] # list string
        lib.createSimpleStringSearch.restype = py_object # pointer

        

        self.lib = lib
        self.search_obj = self.lib.createSimpleStringSearch(strs)

    def findString(self,s,case_sensitive = True):
        if type(s) is list:
            return self.lib.simpleSearch_findStringPy_list(self.search_obj, s, case_sensitive)
        else:
            return self.lib.simpleSearch_findStringPy(self.search_obj, s, case_sensitive)


    def findStringIdx(self,s,case_sensitive = True):
        if type(s) is list:
            return self.lib.simpleSearch_findStringIdxPy_list(self.search_obj, s, case_sensitive)
        else:
            return self.lib.simpleSearch_findStringIdxPy(self.search_obj, s, case_sensitive)


