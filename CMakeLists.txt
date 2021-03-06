cmake_minimum_required(VERSION 3.22)
project(gbgp)

set(CMAKE_CXX_STANDARD 17)

add_executable(gbgp
        include/syntax_tree.h
        include/fitness_function.h
        include/grammar.h
        include/individual.h
        include/vector_ops.h
        include/evaluation.h
        include/tree_node.h
        include/prune_rule.h
        include/production_rule.h
        include/terms.h
        include/population.h
        include/genetic_operators.h
        include/gbgp.h

        tests/doctest.h
        tests/testing.cpp
        tests/test_tree.cpp
        tests/test_trading_grammar.cpp
        tests/test_individual_generation.cpp
        tests/test_memory.cpp
        tests/test_grammar.cpp
        tests/test_simple_evaluation.cpp
        tests/test_semantic_evaluation.cpp
        tests/test_population.cpp

        util/peglib.h
        util/arithmetic_parser.h
        util/arithmetic_parser.cpp)
