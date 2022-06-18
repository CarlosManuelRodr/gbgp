#pragma once
#include <limits>
#include <functional>
#include "cst_tree.h"

template <typename TerminalType, typename NonTerminalType, typename ScoreType> class Individual
{
private:
    ConcreteSyntaxTree<TerminalType, NonTerminalType>* tree;

public:
    std::string expression;
    std::function<ScoreType(std::string)> evaluator;

    static Individual* NewRandomIndividual(Grammar<TerminalType, NonTerminalType> grammar,
                                           const std::function<ScoreType(std::string)>& pevaluator = nullptr,
                                           int maxDepth = 100)
    {
        auto* newTree = new ConcreteSyntaxTree<TerminalType, NonTerminalType>(grammar);
        newTree->CreateRandomTreeSafe(maxDepth);
        return new Individual(newTree, pevaluator);
    }

    Individual()
    {
        tree = nullptr;
        evaluator = nullptr;
    }
    Individual(const Individual<TerminalType, NonTerminalType, ScoreType>& other)
    {
        tree = other.tree;
        evaluator = other.evaluator;
        expression = other.expression;
    }
    explicit Individual(ConcreteSyntaxTree<TerminalType, NonTerminalType>& cst,
                        const std::function<ScoreType(std::string)>& pevaluator = nullptr)
    {
        tree = cst;
        expression = cst.SynthesizeExpression();
        evaluator = pevaluator;
    }
    explicit Individual(ConcreteSyntaxTree<TerminalType, NonTerminalType>* cst,
                        const std::function<ScoreType(std::string)>& pevaluator = nullptr)
    {
        tree = cst;
        expression = cst->SynthesizeExpression();
        evaluator = pevaluator;
    }
    ~Individual()
    {
        delete tree;
    }

    void SetEvaluator(const std::function<ScoreType(std::string)>& pevaluator)
    {
        evaluator = pevaluator;
    }

    ScoreType Evaluate()
    {
        return evaluator(expression);
    }

    ConcreteSyntaxTree<TerminalType, NonTerminalType> GetTree()
    {
        return *tree;
    }

    std::string GetExpression()
    {
        return expression;
    }
};