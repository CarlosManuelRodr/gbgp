#pragma once
#include <limits>
#include "cst_tree.h"

template <typename TerminalType, typename NonTerminalType> class Individual
{
private:
    ConcreteSyntaxTree<TerminalType, NonTerminalType>* tree;

public:
    std::string expression;
    double score;

    Individual()
    {
        tree = nullptr;
        score = std::numeric_limits<double>::quiet_NaN();
    }
    Individual(const Individual<TerminalType, NonTerminalType>& other)
    {
        tree = other.tree;
        expression = other.expression;
        score = other.score;
    }
    explicit Individual(ConcreteSyntaxTree<TerminalType, NonTerminalType>& cst)
    {
        tree = cst;
        expression = cst.SynthesizeExpression();
        score = std::numeric_limits<double>::quiet_NaN();
    }
    explicit Individual(ConcreteSyntaxTree<TerminalType, NonTerminalType>* cst)
    {
        tree = cst;
        expression = cst->SynthesizeExpression();
        score = std::numeric_limits<double>::quiet_NaN();
    }

    static Individual* NewRandomIndividual(Grammar<TerminalType, NonTerminalType> grammar, int maxDepth = -1)
    {
        auto* newTree = new ConcreteSyntaxTree<TerminalType, NonTerminalType>(grammar);
        newTree->CreateRandomTreeSafe(maxDepth);
        return new Individual(newTree);
    }

    bool IsEvaluated()
    {
        return score != std::numeric_limits<double>::quiet_NaN();
    }

    ConcreteSyntaxTree<TerminalType, NonTerminalType> GetTree()
    {
        return *tree;
    }
};