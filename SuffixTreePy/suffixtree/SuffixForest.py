from suffixtree.SuffixTree import SuffixQueryTree
from concurrent.futures import *
import threading

class SuffixQueryForest(SuffixQueryTree):

    def _chunks(self, l, n):
        """Yield successive n-sized chunks from l."""
        gsize = (len(l) + n -1) // n 
        for i in range(0, len(l), gsize):
            yield l[i:i + gsize]

    def _createOneTree(self, strs:list, withCache:bool):
        #print("create one tree begin",threading.get_ident())
        t = SuffixQueryTree(self.preserveString)
        if withCache:
            t.initStringsWithCache(strs)
        else:
            t.initStrings(strs)
        return t

    def _createTrees(self, strs:list, withCache:bool):
        from itertools import accumulate 
        self.numTree = min(len(strs), self.numTree)
        
        if self.parallelThread == 1:
            self._subNumTree = [0]
            self.trees = [self._createOneTree(strs, withCache)]
        else:
            strsParts = list(self._chunks(strs, self.numTree))
            self._subNumTree = [0] + list(accumulate(map(len,strsParts)))
            futures = [ self.executor.submit( self._createOneTree, i, withCache ) for i in strsParts]
            self.trees = [i.result() for i in futures]

    def __init__(self, numTree:int ,preserveString:bool, parallelThread:int = 1, strs:list = None):
        self.numTree = numTree
        self.preserveString = preserveString
        self.parallelThread = parallelThread
        
        self.trees = None
        if self.parallelThread == 1:
            self.executor = None
        else:
            self.executor = ThreadPoolExecutor(max_workers=parallelThread)
        if strs is not None:
            self._createTrees(strs,True)
        
    def serialize(self,path = None):
        if self.parallelThread == 1:
            temp = [ i.serialize(path) for i in self.trees]
        else:
            futures = [ self.executor.submit( i.serialize, path + ".part_" + str(idx) ) for (idx,i) in enumerate(self.trees)]
            temp = [i.result() for i in futures]
        if path is None:
            return temp

    def zippedSerialize(self,path):
        if self.parallelThread == 1:
            temp = [ i.zippedSerialize(path) for i in self.trees]
        else:
            futures = [ self.executor.submit( i.zippedSerialize, path + ".part_" + str(idx) ) for (idx,i) in enumerate(self.trees)]
            temp = [i.result() for i in futures]

    def zippedDeserialize(self,path):
        if self.parallelThread == 1:
            temp = [ i.zippedDeserialize(path) for i in self.trees]
        else:
            futures = [ self.executor.submit( i.zippedDeserialize, path + ".part_" + str(idx) ) for (idx,i) in enumerate(self.trees)]
            temp = [i.result() for i in futures]

    def deserialize(self,content = None):
        raise Exception("Not support in SuffixTree Forest")

    def findStringIdx(self,s:str,case_sensitive:bool = True): 
        l = []
        for num,i in zip(self._subNumTree, self.trees):
            temp = i.findStringIdx(s,case_sensitive)
            l.extend(map(lambda x:x + num,temp))
        return l

    def findString(self,s:str,case_sensitive:bool = True): 
        l = []
        for i in self.trees:
            l.extend(i.findString(s,case_sensitive))
        return l

    def initStrings(self,strs:list):
        self.numStr = len(strs)
        self._createTrees(strs, False)

    def initStringsWithCache(self,strs:list):
        self.numStr = len(strs)
        self._createTrees(strs, True)

    def findStringIdx_wildCard(self,s:list,case_sensitive:bool = True): 
        l = []
        for num,i in zip(self._subNumTree, self.trees):
            temp = i.findStringIdx_wildCard(s,case_sensitive)
            l.extend(map(lambda x:x + num,temp))
        return l

    def findString_wildCard(self,s:list,case_sensitive:bool = True): 
        l = []
        for i in self.trees:
            l.extend(i.findString_wildCard(s,case_sensitive))
        return l

    def cacheNodes(self,budgetRatio:float = 0.5,sampleRate:float = 0.01):
        raise Exception("Not support in SuffixTree Forest")

    def getStrings(self):
        l = []
        for i in self.trees:
            l.extend(i.getStrings())
        return l
