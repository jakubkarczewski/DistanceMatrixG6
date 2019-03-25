from distutils.core import setup, Extension

module1 = Extension('simple_graphs',
                    sources = ['simple_graphsmodule.c'])

setup (name = 'Simple_graphs',
       version = '1.0',
       description = 'This is a custom, graph package.',
       ext_modules = [module1])
