from setuptools import setup
from Cython.Build import cythonize

setup(ext_modules = cythonize("cy_usporf.pyx"))

# For local Mac and Linux 
# in terminal$ python setup.py build_ext --inplace
