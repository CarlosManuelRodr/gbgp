cmake_minimum_required(VERSION 3.22)
project(gbgp)

set(CMAKE_CXX_STANDARD 20)

add_executable(gbgp
        include/cst_tree.h
        include/grammar.h
        include/individual.h
        include/vector_ops.h
        include/gbgp.h

        tests/doctest.h
        tests/testing.cpp
        tests/test_subtree_insertion.cpp

        )
