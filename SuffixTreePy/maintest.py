from suffixtree import *

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
"a2@",
"wwwr"
]

def main():
    tree = SuffixQueryTree(False)
    tree.initStringsWithCache(strs)
    allstr = tree.getStrings()
    print(allstr)
    r = RegularExpSearch("(a\\d.|21)^")
    t = r.searchStringIdx(tree)
    result = list(map(lambda x:allstr[x],t))
    print(result)
    try:
        t = tree.findStringIdx_wildCard(["1","12",SuffixQueryTree.CHAR_STRING_START])
        print(t)
        t = tree.findString_wildCard(["1","12",SuffixQueryTree.CHAR_STRING_END])
        print(t)
    except Exception as e:
        print(e)

main()



