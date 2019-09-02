from suffixtree import *
import re
from threading import Thread
import threading
from memory_profiler import profile

import random
import time
import sys

charset = ['a','b','c','d','e','f','g']

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
"wwwr",
"AB1",
"aBC",
"cdD",
"CD",
"Cd"
]
tree = SuffixQueryTree(False)
tree.initStringsWithCache(strs)
allstr = tree.getStrings()
r = RegularExpSearch(tree)

charset = ['a','b','c','d','e','f','g']
def random_string(lens:int):
     a = random.choices(charset,k=lens)
     return "".join(a)


def create_dataset(n:int,l:int):
    data =[
        random_string(random.randint(0,5) + l)
        for _ in range(n)
    ]
    return data

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

def test_simplesearch():
    a = SimpleStringSearch(strs)
    print(a.findString("a"))
    print(a.findString("12"))
    print(a.findString(["a", "b"]))
    print(a.findString(["abc", "de"]))
    print(a.findString(["aabbc", "de"]))
    print(a.findString(["aabbc", "dde"]))
    print(a.findString(["cd"]))
    print(a.findString(["cd", "CD"]))
    print(a.findString(["CD"]))

    print("----------------------------------")

    print(a.findString(["aabbc", "dde"],False))
    print(a.findString(["cd"],False))
    print(a.findString(["cd", "CD"],False))
    print(a.findString(["CD"],False))
    print(a.findString(["aB"],False))


    print("----------------------------------")

    print(a.findStringIdx("12"))
    print(a.findStringIdx(["a", "b"]))
    print(a.findStringIdx(["abc", "de"]))
    print(a.findStringIdx(["aabbc", "de"]))
    print(a.findStringIdx(["aabbc", "dde"]))

@profile
def benchmark_simplesearch():
    import gc
    n = 100000 * 50
    test_n = 1000
    data = create_dataset(n,20)
    
    tl0 = time.time()
    a = SimpleStringSearch(data)
    tl1 = time.time()
    print("finished dataset creation")
    testdata = create_dataset(test_n, 3)
    num = 0
    t0 = time.time()

    print(gc.get_stats())
    for i in testdata:
        #print(i)
        temp = a.findStringIdx(i)
        num += len(temp)
    t1 = time.time()
    print(num)
    ctime = t1 - t0
    print("creation time cost", tl1 - tl0,"total time cost", ctime, " avg time ", ctime / test_n)
    print(gc.get_stats())

    sys.stdin.readline()



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

test_simplesearch()
#benchmark_simplesearch()

#from suffixtree.TestClass import *
#from suffixtree.TestClass import *
#import unittest
#unittest.main()

