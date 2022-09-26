cmake_minimum_required(VERSION 3.22)
project(gbgp)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_CONFIGURATION_TYPES "Test;Python" CACHE STRING "" FORCE)
message("Available build types are: ${CMAKE_CONFIGURATION_TYPES}")

if (CMAKE_BUILD_TYPE MATCHES Test)
    message("Compiling unit tests" )

    include_directories(third-party/doctest/doctest)
    include_directories(third-party/cpp-peglib)

    add_executable(gbgp
            include/thread_pool.h
            include/syntax_tree.h
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

            util/arithmetic_parser.h
            util/arithmetic_parser.cpp)

elseif(CMAKE_BUILD_TYPE MATCHES Python)
    message("Compiling Python bindings" )
    add_subdirectory(third-party/pybind11)

    pybind11_add_module(gbgp
            include/thread_pool.h
            include/syntax_tree.h
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
            include/environment.h
            include/gbgp.h

            python/bindings.cpp
            )

    add_custom_command(TARGET gbgp POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy
            "$<TARGET_FILE:gbgp>"
            "${CMAKE_CURRENT_SOURCE_DIR}/python/$<TARGET_FILE_NAME:gbgp>"
            COMMENT "Copying to output directory")
endif()

