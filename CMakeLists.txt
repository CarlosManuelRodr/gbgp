cmake_minimum_required(VERSION 3.22)
project(gbgp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(
            /Zc:__cplusplus
            /utf-8)
endif()

if(NOT DEFINED BUILD_TYPE)
    set(BUILD_TYPE "Tests")
endif()

if (BUILD_TYPE MATCHES Tests)
    message("Compiling unit tests" )

    include_directories(third-party/doctest/doctest)
    include_directories(third-party/cpp-peglib)
    include_directories(third-party/cereal/include)

    add_executable(gbgp
            include/thread_pool.h
            include/syntax_tree.h
            include/graph.h
            include/grammar.h
            include/individual.h
            include/vector_ops.h
            include/evaluation.h
            include/tree_node.h
            include/prune_rule.h
            include/production_rule.h
            include/term.h
            include/population.h
            include/genetic_operators.h
            include/environment.h
            include/gbgp.h

            tests/testing.cpp
            tests/test_tree_manipulation.cpp
            tests/test_trading_grammar.cpp
            tests/test_individual_generation.cpp
            tests/test_memory.cpp
            tests/test_grammar.cpp
            tests/test_simple_evaluation.cpp
            tests/test_semantic_evaluation.cpp
            tests/test_population.cpp
            tests/test_genetic_operators.cpp
            tests/test_optimization.cpp
            tests/test_logic_gates.cpp
            tests/test_graph.cpp
            tests/test_serialization.cpp

            util/arithmetic_parser.h
            util/arithmetic_parser.cpp)

    target_compile_definitions(gbgp PRIVATE
            DOCTEST_CONFIG_USE_STD_HEADERS)

elseif(BUILD_TYPE MATCHES Python)
    message("Compiling Python bindings" )

    # scikit-build-core provides the Python selected by the invoking environment
    # and makes the pybind11 build dependency discoverable by CMake.
    set(PYBIND11_FINDPYTHON ON)
    find_package(Python COMPONENTS Interpreter Development.Module REQUIRED)
    find_package(pybind11 CONFIG REQUIRED)

    message(STATUS "Python extension interpreter: ${Python_EXECUTABLE}")

    pybind11_add_module(gbgp
            include/thread_pool.h
            include/syntax_tree.h
            include/graph.h
            include/grammar.h
            include/individual.h
            include/vector_ops.h
            include/evaluation.h
            include/tree_node.h
            include/prune_rule.h
            include/production_rule.h
            include/term.h
            include/population.h
            include/genetic_operators.h
            include/environment.h
            include/gbgp.h

            python/bindings.cpp
            )

    target_include_directories(gbgp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

    # scikit-build-core collects CMake's install tree for wheels and editable
    # installs. A top-level destination preserves the public `import gbgp`.
    install(TARGETS gbgp
            LIBRARY DESTINATION .
            RUNTIME DESTINATION .)
endif()

