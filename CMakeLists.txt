cmake_minimum_required(VERSION 3.22)
project(gbgp)

set(CMAKE_CXX_STANDARD 17)

add_executable(gbgp
        include/cst_tree.h
        include/grammar.h
        include/individual.h
        include/vector_ops.h
        include/evaluation.h
        include/gbgp.h

        tests/doctest.h
        tests/testing.cpp
        tests/test_tree_manipulation.cpp
        tests/test_trading_grammar.cpp
        tests/test_individual_generation.cpp
        tests/test_memory.cpp
        tests/test_grammar.cpp

        util/peglib.h
        util/arithmetic_parser.h
        util/arithmetic_parser.cpp)
