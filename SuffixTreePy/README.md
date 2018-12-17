# Introduction
This is a Suffix Tree data structure lib implemented in C++, wrapped with python.

Expect for contructing the tree, this lib also provides to construct the query tree for speeding up querying strings.

# Install and Build
### Install from Pypi

run `pip install csuffixtree`

### Build
You can build this either on linux or windows.


### Install Python Package

1. Build SuffixTreePyBinding solution in Windows
2. On linux you should go to `linux` directory, and run `make python` 
3. under SuffixTreePy directory. run `python setup.py install`

# How to use

You can construct a suffix tree in the following way.
```
from suffixtree import *
tree = SuffixTree(True,["abc","123","321"])
tree.addStrings(["xyz","abcd"])
print(tree.findString("1"))
print(tree.findString("a"))

```

If you finished inserting strings to a suffix tree, and you want the querying be very fast
you can do:

```
# convert tree to query tree, this release a part of memory
qtree = tree.createQueryTree()
qtree.cacheNodes() # take some time to cache intermediate nodes
qtree.findString("a")

```

For simplicity you can also write

```
# do not preserve string
qtree = SuffixQueryTree(False,["abc","abcd","123","321"])
idx = qtree.findStringIdx("abc") # you cannot use findString now
print(idx)
```

### Serialization 
SuffixQueryTree can be serialized and deserialized to/from a file a bytes object.
For example:

 ```
# do not preserve string
qtree = SuffixQueryTree(False,["abc","abcd","123","321"])
idx = qtree.findStringIdx("abc") # you cannot use findString now
print(idx)
```

