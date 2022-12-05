from pybind11.setup_helpers import Pybind11Extension, build_ext
from setuptools import setup

__version__ = "0.0.1"


ext_modules = [
    Pybind11Extension("gbgp",
                      ["bindings.cpp"],
                      define_macros=[('VERSION_INFO', __version__)],
                      ),
]

setup(
    name="gbgp",
    version=__version__,
    author="Carlos Manuel Rodriguez Martinez",
    author_email="fis.carlosmanuel@gmail.com",
    url="https://github.com/CarlosManuelRodr/gbgp",
    description="Simple header-only C++ library to perform grammar-based genetic programming optimization that "
                "includes Python bindings.",
    long_description="",
    ext_modules=ext_modules,
    extras_require={"test": "pytest"},
    # Currently, build_ext only provides an optional "highest supported C++
    # level" feature, but in the future it may provide more features.
    cmdclass={"build_ext": build_ext},
    zip_safe=False,
    python_requires=">=3.7",
)
