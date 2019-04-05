from suffixtree import *
import re
from threading import Thread
import threading

strs = [
"abc",
"abcde",
"aabbcdeabc",
"123",
"1234",
"321",
"123321",
"hhhzzzzww",
"ttbb",
"ab1",
"12b",
"12",
"11",
"a1a",
"b",
"a2@",
"wwwr"
]
tree = SuffixQueryTree(False)
tree.initStringsWithCache(strs)
allstr = tree.getStrings()
r = RegularExpSearch(tree)

def test1():
    
    p = "[^bc23]+([b-c]+|2|3){2,}$"
    t = r.searchString(p)
    print("match strings: ",t)
    t2 = [ i for i in strs if re.match(p,i) ]
    print(t2)
    assert set(t) == set(t2)



def test3():
    print("Thread #: " + str( threading.get_ident() ))
    p = "[^bc23]+([b-c]+|2|3){2,}$"
    temp = 0
    for i in range(1000):
        s = r.st.getStrings()
        t = r.searchPossibleStringIdx(p)
        import time
        time.sleep(0.01)
        temp += len(t)
        print(threading.get_ident(),i,len(t),len(s))
    return temp

def main():

    t = r.searchPossibleStringIdx("(a\\d.|21)^")
    result = list(map(lambda x:allstr[x],t))
    print("possible: ",result)
    
    t = r.searchString("^(a\\d.|21)")
    print(t)
    
    try:
        t = tree.findStringIdx_wildCard(["1","12",SuffixQueryTree.CHAR_STRING_START])
        print(t)
        t = tree.findString_wildCard(["1","12",SuffixQueryTree.CHAR_STRING_END])
        print(t)
    except Exception as e:
        print(e)

#test1()
#main()

def test2():
    ts = [Thread(target = test3) for _ in range (5)]
    for i in ts:
        i.start()

    for i in ts:
        i.join()

#test2()

from suffixtree.TestClass import *
import unittest
unittest.main()

