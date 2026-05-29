from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext
import sys
import pybind11

class BuildExt(build_ext):
    def build_extensions(self):
        opts = ['/O2', '/std:c++17'] if sys.platform == 'win32' else ['-O3', '-std=c++17']
        for ext in self.extensions:
            ext.extra_compile_args = opts
            ext.include_dirs.append(pybind11.get_include())
        super().build_extensions()

setup(
    name='moneyriver_core',
    version='1.0',
    ext_modules=[
        Extension(
            'moneyriver_core',
            sources=['main.cpp'],
            language='c++',
        ),
    ],
    cmdclass={'build_ext': BuildExt},
    zip_safe=False,
)