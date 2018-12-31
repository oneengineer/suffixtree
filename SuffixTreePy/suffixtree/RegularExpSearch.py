from .SuffixTree import SuffixQueryTree
from antlr4 import *
from automaton_tools import *

LEN_MATCH_ANY = -1

class RegularExpSearch(object):
    """description of class"""

    def __init__(self,suffixQueryTree:SuffixQueryTree,andClause_searchNum = 4):
        self.andClause_searchNum = andClause_searchNum
        self.st = suffixQueryTree
        self.allStrings = None

    def _combine(self,tree):
        t = type(tree)
        if t is CharsetSeq:
            return tree
        new_operands = []
        for op in tree.operands:
            temp = self._combine(op)
            if t == type(temp):
                new_operands += op.operands
            else :
                new_operands += [temp]
        tree.operands = new_operands
        return tree

    def _resolveConcat(self,ast):
        if type(ast) is not Concatclause:
            return ast
        self.q = []
        def _dfs(ast):
            if type(ast) is not Concatclause:
                self.q += [ast]
                return
            for i in ast.seq:
                _dfs(i)

        _dfs(ast)
        # self.q should have no concatAST
        seq = []
        current = []
        for i in self.q:
            if type(i) is CharsetSeq:
                current += i.seq
            else:
                if len(current) > 0:
                    c = CharsetSeq(current)
                    current = []
                    seq += [ c ]

                if type(i) is not Break:
                    seq += [ i ] #ignore break

        if len(current) > 0:
            c = CharsetSeq(current)
            seq += [c]
        if len(seq) < 2:
            return seq[0]
        return ANDclause(seq)


    def _translateToClause(self,ast):
        t = type(ast)
        if t is ConcatAST:
            a = self._translateToClause(ast.a)
            b = self._translateToClause(ast.b)
            return Concatclause([a,b])
        elif t is OrAST:
            a = self._translateToClause(ast.a)
            b = self._translateToClause(ast.b)
            a = self._resolveConcat(a)
            b = self._resolveConcat(b)
            return ORclause([a,b])
        elif t is PlusAST: # get a single one
            a = self._translateToClause(ast.content)
            a = self._resolveConcat(a)
            return Concatclause([a,Break()])
        elif t is RepeatAST: # get a single one unless it is charset
            num = ast.min
            a = self._translateToClause(ast.content)
            a = self._resolveConcat(a)
            if type(a) is CharsetSeq:
                #repeat charset
                import copy
                seq = []
                for i in range(num):
                    seq += copy.deepcopy(a.seq)
                a = CharsetSeq(seq)
            return Concatclause([a, Break()])
        elif t is CharsetAST :
            return CharsetSeq([ast.charset]) # charset is the type in automaton_tools
        elif t is StarAST:
            return Break()
        else :
            raise Exception(f"Unknown type {t}  ast: {ast}")

    def _getWildCard(self,clause):
        m = {
            CharAny: ord('.'),
            CharWord: ord('w'),
            CharStart: ord('^'),
            CharEnd: ord('$')
        }
        if type(clause) is CharsetSeq:
            pattern = []
            for i in clause.seq:
                t = type(i)
                #print(isinstance(i,CharRange))
                #print(isinstance(i,CharSet))
                if t in m:
                    pattern += [ m[t] ]
                elif t is CharRange:
                    pattern += [ "".join(i.charset) ]
                elif t is CharSet:
                    pattern += [ str(i.char) ]
                else: 
                    raise Exception(f"Unknown type {t} {i}")
            result = set( self.st.findStringIdx_wildCard(pattern) )
            return result
        elif type(clause) is ORclause:
            result = set()
            ops_and_len = sorted(clause.operands, key = lambda x :x.len)
            for i in ops_and_len:
                result = result.union(self._getWildCard(i))
                if i.len == LEN_MATCH_ANY:
                    # search this kind of char equals to fetch all strings
                    break;
            return result
        elif type(clause) is ANDclause:
            result = None
            ops_and_len = sorted(clause.operands, key = lambda x :x.len,reverse = True)
            for i in clause.operands[:self.andClause_searchNum]:
                if result is None:
                    result = self._getWildCard(i)
                else:
                    if i.len == LEN_MATCH_ANY:
                        break;
                    temp = self._getWildCard(i)
                    result = result.intersection(temp)
            return result
        else:
            raise Exception("Unknown clause " + str(type(clause)))

    def _parseRegex(self,regex):
        input = InputStream(regex)
        lexer = RegexLexer(input)
        stream = CommonTokenStream(lexer)
        parser = RegexParser(stream)
        parser.addErrorListener(AutomatonErrorListener)
        root = parser.root()
        visitor = ASTVisitor()
        ast = visitor.visit(root)
        self.ast = ast
        return ast

    def searchPossibleStringIdx(self,regex):
        ast = self._parseRegex(regex)
        clause = self._translateToClause(ast)
        clause = self._resolveConcat(clause)
        minClause = self._combine(clause)
        minClause.setLen()
        return self._getWildCard(minClause)
        
    def searchString(self,regex):
        import re
        idx = self.searchPossibleStringIdx(regex)
        if self.allStrings is None:
            self.allStrings = self.st.getStrings()
        selected = [ self.allStrings[i] for i in idx ]
        # check if selected string matches the pattern
        prog = re.compile(regex)
        result = [ i
            for i in selected
            if prog.match(i)
        ]
        return result
        

class ORclause:
    def __init__(self,operands):
        self.operands = operands

    def __repr__(self):
        temp = " OR ".join(map(str,self.operands))
        return "(" + temp + ")"

    def setLen(self):
        self.len = sum([ i.setLen() for i in self.operands ])
        return self.len


class ANDclause:
    def __init__(self,operands):
        self.operands = operands

    def __repr__(self):
        temp = " AND ".join(map(str,self.operands))
        return "(" + temp + ")"

    def setLen(self):
        self.len = min([ i.setLen() for i in self.operands ])
        return self.len

class CharsetSeq:
    def __init__(self,seq):
        self.seq = seq

    def __repr__(self):
        return "".join(map(str,self.seq))

    def setLen(self):
        #make start,end and . infinity long
        if len(self.seq) == 1 and type(self.seq[0]) in {CharAny,CharStart,CharEnd}:
            self.len = LEN_MATCH_ANY
        else:
            self.len = len(self.seq)
        return self.len

class Concatclause:
    def __init__(self,seq):
        self.seq = seq

    def __repr__(self):
        return "[" + ", ".join(map(str,self.seq)) + "]"

class Break:
    def __init__(self):
        pass

    def __repr__(self):
        return "BREAK"


