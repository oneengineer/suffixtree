from suffixtree import *
import re

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

test1()
#main()



