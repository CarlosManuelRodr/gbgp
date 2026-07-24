# Using and building gbgp

gbgp is a header-only C++17 library with optional Python bindings. The C++
tests use vendored dependencies stored in Git submodules, while Python builds
obtain their build dependencies through `pip`.

## Get the source

Clone the repository and its submodules:

```console
git clone --recurse-submodules https://github.com/CarlosManuelRodr/gbgp.git
cd gbgp
```

If the repository was cloned without `--recurse-submodules`, initialize the
dependencies before building the C++ tests:

```console
git submodule update --init --recursive
```

## Use the C++ library

gbgp does not need to be compiled before use. Add the repository's `include`
directory to a C++17 target and include the umbrella header:

```cpp
#include <gbgp.h>
```

For example, from a CMake project that keeps gbgp under `third-party/gbgp`:

```cmake
add_executable(my_program main.cpp)
target_compile_features(my_program PRIVATE cxx_std_17)
target_include_directories(
    my_program
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/third-party/gbgp/include"
)
```

The complete grammar and optimization example in [Readme.md](Readme.md)
demonstrates the C++ API.

## Build and run the C++ tests

Requirements:

- CMake 3.22 or newer
- A C++17 compiler
- The initialized Git submodules described above

Configure and build the test executable from the repository root:

```console
cmake -S . -B build/tests -DBUILD_TYPE=Tests
cmake --build build/tests --config Release
```

With a single-configuration generator such as Make or Ninja, run:

```console
./build/tests/gbgp
```

With a multi-configuration generator such as Visual Studio, run:

```powershell
.\build\tests\Release\gbgp.exe
```

## Use and build the Python bindings

Requirements:

- Python 3.7 or newer, including its development headers
- CMake 3.22 or newer
- A C++17 compiler compatible with the selected Python interpreter

Create and activate a virtual environment:

```console
python -m venv .venv
```

On Windows PowerShell:

```powershell
.\.venv\Scripts\Activate.ps1
```

On Linux or macOS:

```console
source .venv/bin/activate
```

Install the library in editable mode together with its test dependency:

```console
python -m pip install -U pip
python -m pip install -e ".[test]"
```

The installation invokes CMake through scikit-build-core and builds the native
extension for the Python interpreter in the active environment. Confirm which
extension was installed and run the Python tests with:

```console
python -c "import gbgp; print(gbgp.__file__)"
python -m pytest python/tests
```

After changing C++ sources or headers, rerun the editable installation to
rebuild incrementally:

```console
python -m pip install -e ".[test]"
```

To build a distributable wheel instead:

```console
python -m pip wheel . --wheel-dir dist
```

The build backend stores its per-ABI CMake build trees under
`build/{wheel_tag}/native`. On Windows, the default Visual Studio generator
locates MSVC from an ordinary PowerShell session. Ninja is also available as
an opt-in from a Visual Studio Developer PowerShell; use a separate build
directory to avoid generator conflicts:

```powershell
$env:CMAKE_GENERATOR = "Ninja"
python -m pip install -e ".[test]" -Cbuild-dir=build/ninja-dev
```

Compiled extensions cannot be reliably unloaded from a running Python
process. After rebuilding, start a new Python process or restart the Jupyter
kernel before importing `gbgp` again.
