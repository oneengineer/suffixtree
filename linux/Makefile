CFLAGS += -fPIC -O3
CXXFLAGS = $(CFLAGS) -std=c++17
CC = g++

INCLUDES = \
	-I../SuffixTree \
	-I../SuffixTreePyBinding \
	-I${PYTHONHOME}/include/python3.7m/ \

LIBPATH = \
	-L.\
	-L${PYTHONHOME}/lib

LIBFLAGS += \
	-l:libSuffixTree.a \
	-lpython3.7m

.PHONY: default
default: testSuffixTree

libSuffixTree.a: ../SuffixTree/*.h ../SuffixTree/*.cpp
	$(CC) -c $(INCLUDES) $(CXXFLAGS) ../SuffixTree/interface.cpp -o libSuffixTree.a

testSuffixTree: libSuffixTree.a  ../TestSuffixTree/TestSuffixTree.cpp
	cp ../TestSuffixTree/*.txt ./
	$(CC) $(INCLUDES) $(CXXFLAGS) $(LIBPATH) $(LIBFLAGS) ../TestSuffixTree/TestSuffixTree.cpp -o testSuffixTree

libSuffixTreePyBinding.so: libSuffixTree.a ../SuffixTreePyBinding/interface_py.cpp
	$(CC) -shared $(INCLUDES) $(CXXFLAGS) -DPy_LIMITED_API $(LIBPATH) $(LIBFLAGS) ../SuffixTreePyBinding/interface_py.cpp -o libSuffixTreePyBinding.so


python: libSuffixTreePyBinding.so


clean:
	rm -f *.a
	rm -f *.so
	rm -f testSuffixTree
