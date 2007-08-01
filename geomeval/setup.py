from distutils.core import setup, Extension
from numarray.numarrayext import NumarrayExtension
import sys

#module1 = Extension('geomeval',
#                    sources = ['geomeval.cpp', 'expression.cpp', octree.cpp', 'interval.cpp', 'space_interval.cpp'])

#setup (name = 'geomeval',
#       version = '0.1',
#       description = 'High-speed geometry evaluator for cad.py',
#       ext_modules = [module1])

setup (name = 'geomeval',
       version = '0.21',
       description = 'High-speed geometry evaluator for cad.py',
       ext_modules = [NumarrayExtension("geomeval", ['pyint.cpp', 'expression.cpp', 'octree.cpp', 'interval.cpp', 'space_interval.cpp'],include_dirs=["./"], library_dirs=["./"], libraries=['m'])])
