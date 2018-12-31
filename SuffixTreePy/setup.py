
from setuptools import *
from setuptools.command.build_py import build_py

import shutil
import os
import sys

with open("README.md", "r") as fh:
    long_description = fh.read()


setupPath = os.path.dirname(os.path.abspath(__file__))
dllpath = setupPath + "/../x64/Release" + "/SuffixTreePyBinding.dll"
sopath = setupPath + "/../linux" + "/libSuffixTreePyBinding.so"
packagePath = setupPath + "/suffixtree"

class MyBuild(build_py):

    def run(self):
        try:
            shutil.copy(dllpath, packagePath)
        except Exception as e:
            print(e)
            print("failed to copy ",dllpath)
        try:
            shutil.copy(sopath, packagePath)
        except Exception as e:
            print(e)
            print("failed to copy ",sopath)
        super().run()

cmdclass={
    'build_py': MyBuild
}


setup(
    name="csuffixtree",
    version="0.2.0",
    author="oneengineer",
    author_email="oneengineer@gmail.com",
    description="A C implemented Suffix Tree package",
    long_description=long_description,
    long_description_content_type="text/markdown",
    install_requires=["automaton_tools"],
    url="https://github.com/oneengineer/suffixtree",
    packages=find_packages(),
    include_package_data = True,
    package_data={'suffixtree':['*.so','*.dll']},
    cmdclass=cmdclass
)
