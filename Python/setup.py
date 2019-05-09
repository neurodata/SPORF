from setuptools import setup, Extension, find_packages
from setuptools.command.build_ext import build_ext
import sys
from distutils.errors import CompileError
import subprocess

__version__ = "0.0.1"

PACKAGE_NAME = "rerf"
DESCRIPTION = "Randomer Forest (RerF) Python Package"
with open("../README.md", "r") as f:
    LONG_DESCRIPTION = f.read()
URL = "https://github.com/neurodata/RerF"
AUTHOR_EMAIL = "falk.ben@jhu.edu"
MINIMUM_PYTHON_VERSION = 3, 6  # Minimum of Python 3.6


def check_python_version():
    """Exit when the Python version is too low."""
    if sys.version_info < MINIMUM_PYTHON_VERSION:
        sys.exit("Python {}.{}+ is required.".format(*MINIMUM_PYTHON_VERSION))


class get_pybind_include(object):
    """Helper class to determine the pybind11 include path

    The purpose of this class is to postpone importing pybind11
    until it is actually installed, so that the ``get_include()``
    method can be invoked. """

    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        import pybind11

        return pybind11.get_include(self.user)


ext_modules = [
    Extension(
        "pyfp",
        ["packedForest.cpp"],
        include_dirs=[
            # Path to pybind11 headers
            get_pybind_include(),
            get_pybind_include(user=True),
        ],
        language="c++",
    )
]


# As of Python 3.6, CCompiler has a `has_flag` method.
# cf http://bugs.python.org/issue26689
def has_flag(compiler, flagname):
    """Return a boolean indicating whether a flag name is supported on
    the specified compiler.
    """
    import tempfile

    with tempfile.NamedTemporaryFile("w", suffix=".cpp") as f:
        f.write("int main (int argc, char **argv) { return 0; }")
        try:
            compiler.compile([f.name], extra_postargs=[flagname])
        except CompileError:
            return False
    return True


def cpp_flag(compiler):
    """Return the -std=c++[11/14] compiler flag.

    The c++14 is prefered over c++11 (when it is available).
    """
    if has_flag(compiler, "-std=c++14"):
        return "-std=c++14"
    elif has_flag(compiler, "-std=c++11"):
        return "-std=c++11"
    else:
        raise RuntimeError(
            "Unsupported compiler -- at least C++11 support " "is needed!"
        )


class BuildExt(build_ext):
    """A custom build extension for adding compiler-specific options."""

    c_opts = {
        "msvc": ["/EHsc", "/openmp"],
        "unix": [
            "-Xpreprocessor",
            "-fopenmp",
            "-Wall",
            "-O3",
            "-DNDEBUG",
            "-ffast-math",
        ],
    }

    if sys.platform == "darwin":
        ompbase = subprocess.run(["brew", "--prefix", "libomp"], stdout=subprocess.PIPE)
        omploc = ompbase.stdout.decode("utf-8").strip()

        c_opts["unix"] += [
            "-lomp",
            "-I{}/include".format(omploc),
            "-L{}/lib".format(omploc),
        ]

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])
        if ct == "unix":
            opts.append('-DVERSION_INFO="%s"' % self.distribution.get_version())
            opts.append(cpp_flag(self.compiler))
            if has_flag(self.compiler, "-fvisibility=hidden"):
                opts.append("-fvisibility=hidden")
        elif ct == "msvc":
            opts.append('/DVERSION_INFO=\\"%s\\"' % self.distribution.get_version())
        for ext in self.extensions:
            ext.extra_compile_args = opts
            if ct == "unix":
                ext.extra_link_args = ["-lgomp"]
        build_ext.build_extensions(self)


check_python_version()

with open("requirements.txt") as f:
    required = f.read().splitlines()

setup(
    name=PACKAGE_NAME,
    version=__version__,
    description=DESCRIPTION,
    long_description=LONG_DESCRIPTION,
    author_email=AUTHOR_EMAIL,
    ext_modules=ext_modules,
    install_requires=required,
    cmdclass={"build_ext": BuildExt},
    zip_safe=False,
    url=URL,
    license="Apache License 2.0",
    packages=find_packages(),
)
