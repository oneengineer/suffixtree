import unittest
from suffixtree import *
from threading import *
import time

class TestClass(unittest.TestCase):
    
    def generateRandomString(self,charBase:list,length:int,num:int = 1):
        import random
        def a():
            return "".join([ random.choice(charBase) for i in range(length)])
        if num == 1:
            return a()
        return [a() for i in range(num)]
        

    def existIn(self, pattern:str, l:list):
        return [i for i in l if pattern in i]

    def test_forest(self):
        "test SuffixQueryForest's finding"
        sf = SuffixQueryForest(3,True,3)
        strs1 = self.generateRandomString("abcde",4,1000)
        strs2 = self.generateRandomString("123456",3,1000)
        strs3 = self.generateRandomString("abc123",6,1000)

        strs4 = self.generateRandomString("ABCD",3,1000)
        strs5 = self.generateRandomString("xyz",2,10)

        strs6 = self.generateRandomString("xyz" + "ABCD" + "123456" + "abcde",6,10000)

        all = strs1 + strs2 + strs3 + strs4 + strs5 + strs6

        sf.initStringsWithCache(strs5)

        self.assertEqual(sorted(sf.getStrings()), sorted(strs5))

        print(strs5)

        self.assertEqual(sorted(sf.findString("x")), sorted(self.existIn("x",strs5)))
        self.assertEqual(sorted(sf.findString("y")), sorted(self.existIn("y",strs5)))

        sf = SuffixQueryForest(3,True,1)
        sf.initStringsWithCache(strs3)

        self.assertEqual(sorted(sf.findString("ab")), sorted(self.existIn("ab",strs3)))
        self.assertEqual(sorted(sf.findString("12")), sorted(self.existIn("12",strs3)))

        print(sf.findString("a1c"))
        self.assertEqual(sorted(sf.findString("a1c")), sorted(self.existIn("a1c",strs3)))


        sf = SuffixQueryForest(3,True,1)
        sf.initStringsWithCache(strs6)
        self.assertEqual(sorted(sf.findString("aB")), sorted(self.existIn("aB",strs6)))
        print(sf.findString("Dx"))
        self.assertEqual(sorted(sf.findString("Dx")), sorted(self.existIn("Dx",strs6)))

        sf = SuffixQueryForest(4,True,2)
        sf.initStringsWithCache(all)
        self.assertEqual(sorted(sf.findString("aBz")), sorted(self.existIn("aBz",all)))
        print(sf.findString("1Dx"))
        self.assertEqual(sorted(sf.findString("1Dx")), sorted(self.existIn("1Dx",all)))

    def test_forest_speed(self):
        "test SuffixQueryForest's speed of construction"
        sf = SuffixQueryForest(3,True,3)
        strs1 = self.generateRandomString("abcde",6,10000)
        strs2 = self.generateRandomString("123456",6,10000)
        strs3 = self.generateRandomString("abc123",6,10000)

        strs4 = self.generateRandomString("ABCD",6,10000)
        strs5 = self.generateRandomString("xyz",6,10000)

        n = 100000
        strs6 = self.generateRandomString("xyz" + "ABCD" + "123456" + "abcde",16,n)

        all = strs1 + strs2 + strs3 + strs4 + strs5 + strs6
        t0 = time.time()
        sf = SuffixQueryForest(4,True,4)
        sf.initStringsWithCache(all)
        t1 = time.time()
        print("4 trees 4 thread time cost ", t1 - t0)

        sf2 = SuffixQueryForest(1,True,1)
        sf2.initStringsWithCache(all)
        t2 = time.time()
        print("1 tree 1 thread time cost ", t2 - t1)

        t0 = time.time()
        sf.zippedSerialize("cpu4.idx")
        t1 = time.time()
        sf2.zippedSerialize("cpu1.idx")
        t2 = time.time()

        print("4 trees 4 thread serilization time cost ", t1 - t0)
        print("1 tree 1 thread serilization time cost ", t2 - t1)





class TestClass2(unittest.TestCase):
    
    def generateRandomString(self,charBase:list,length:int,num:int = 1):
        import random
        def a():
            return "".join([ random.choice(charBase) for i in range(length)])
        if num == 1:
            return a()
        return [a() for i in range(num)]
        

    def existIn(self, pattern:str, l:list):
        return [i for i in l if pattern in i]

    def test_case_sensitive(self):
        "test case sensitive"
        sf = SuffixQueryForest(3,True,3)
        strs1 = self.generateRandomString("abcde",6,10000)
        strs2 = self.generateRandomString("123456",6,10000)
        strs3 = self.generateRandomString("abc123",6,10000)

        strs4 = self.generateRandomString("ABCD",6,10000)
        strs5 = self.generateRandomString("ABCD123",6,10000)

        sf.initStringsWithCache(strs2)
        self.assertEqual(sf.findString("25",True), sf.findString("25",True))
        sf.initStringsWithCache(strs3)
        self.assertEqual(len(sf.findString("A",True)), 0)
        self.assertEqual(len(sf.findString("a1",True)), len( self.existIn("a1",strs3) ))

        self.assertEqual(len(sf.findString("A1",False)), len( self.existIn("a1",strs3) ))


        sf.initStringsWithCache(strs4)
        self.assertEqual(sorted(sf.findString("AD",True)), sorted(sf.findString("AD",False)))

        sf.initStringsWithCache(strs5)
        self.assertEqual(sorted(sf.findString("a1",False)), sorted(sf.findString("A1",False)))


    def test_case_sensitive_wildcard(self):
        "test case sensitive in regular expression"
        strs3 = self.generateRandomString("abc123",10,100)
        sf = SuffixQueryForest(3,True,3)
        sf.initStringsWithCache(strs3)
        r = RegularExpSearch(sf)

        tree = SuffixQueryTree(True)
        tree.initStringsWithCache(strs3)
        r2 = RegularExpSearch(tree)


        p = "b23"

        p2 = "bc"
        p3 = "Bc"

        #print([ (i,strs3[i]) for i in r.searchPossibleStringIdx(p)])
        #print([ (i,strs3[i]) for i in r2.searchPossibleStringIdx(p)])

        self.assertEqual(sorted(r.searchPossibleStringIdx(p)), sorted(r2.searchPossibleStringIdx(p)))
        self.assertEqual(sorted(r.searchPossibleStringIdx(p2)), sorted(r2.searchPossibleStringIdx(p2)))

        self.assertEqual(sorted(r.searchString(".*abc.*")), sorted(self.existIn("abc",strs3) ))
        
        r.case_sensitive = False
        self.assertEqual(sorted(r.searchString(".*a1C.*")), sorted(self.existIn("a1c",strs3) ))

        print([ (i,strs3[i]) for i in r.searchPossibleStringIdx(p3)])
        self.assertEqual(sorted(r.searchPossibleStringIdx(p3)), sorted(r2.searchPossibleStringIdx(p2)))

        p4 = "[^bc23]+([b-c]+|2|3){2,}$"
        self.assertEqual(sorted(r.searchPossibleStringIdx(p4)), sorted(r2.searchPossibleStringIdx(p4)))



class TestClass3(unittest.TestCase):
    
    def generateRandomString(self,charBase:list,length:int,num:int = 1):
        import random
        def a():
            return "".join([ random.choice(charBase) for i in range(length)])
        if num == 1:
            return a()
        return [a() for i in range(num)]
        

    def test_num_str(self):
        "test get number of string"
        strs1 = self.generateRandomString("abcde",6,10000)

        tree = SuffixQueryTree(True,strs1)
        self.assertEqual(tree.getStrNum(), len(strs1) )
        
        # test deserialize length
        tree.zippedSerialize("temp.idx")
        tree2 = SuffixQueryTree(True)
        tree2.zippedDeserialize("temp.idx")

        print(tree2.getStrNum())
        self.assertEqual(tree2.getStrNum(), len(strs1) )

        sf = SuffixQueryForest(3,True,3)

        sf.initStringsWithCache(strs1)
        self.assertEqual(sf.getStrNum() , len(strs1) )
        print(sf._subNumTree)

        sf.zippedSerialize("temp.idx") # same name is ok

        sf2 = SuffixQueryForest(0,True,3)
        sf2.zippedDeserialize("temp.idx")
        self.assertEqual(sf2.getStrNum() , len(strs1) )
        self.assertEqual(len(sf2.trees) , 3 )

    def test_num_string(self):
        "test get number of string"
        strs1 = self.generateRandomString("abcde",6,10000)

        tree = SuffixQueryTree(True,strs1)
        self.assertEqual(tree.getStrNum(), len(strs1) )
        
        # test deserialize length
        tree.zippedSerialize("temp.idx")
        tree2 = SuffixQueryTree(True)
        tree2.zippedDeserialize("temp.idx")

        print(tree2.getStrNum())
        self.assertEqual(tree2.getStrNum(), len(strs1) )

        sf = SuffixQueryForest(3,True,3)

        sf.initStringsWithCache(strs1)
        self.assertEqual(sf.getStrNum() , len(strs1) )
        print(sf._subNumTree)

        sf.zippedSerialize("temp.idx") # same name is ok

        sf2 = SuffixQueryForest(0,True,3)
        sf2.zippedDeserialize("temp.idx")
        self.assertEqual(sf2.getStrNum() , len(strs1) )
        self.assertEqual(len(sf2.trees) , 3 ) 
