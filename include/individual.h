#pragma once
#include <limits>
#include "cst_tree.h"

template <typename EvaluationContextType = EvaluationContext> class Individual
{
private:
    ConcreteSyntaxTree* tree;

public:
    std::string expression;
    std::function<std::string(std::string)> evaluator;

    static Individual<EvaluationContextType>*
    NewRandomIndividual(const Grammar& grammar, const std::function<std::string(std::string)>& pevaluator = nullptr,
                        int maxDepth = 100)
    {
        auto* newTree = new ConcreteSyntaxTree();
        newTree->CreateRandomTree(grammar, maxDepth);
        return new Individual(newTree, pevaluator);
    }

    Individual()
    {
        tree = nullptr;
        evaluator = nullptr;
    }
    Individual(const Individual<EvaluationContextType>& other)
    {
        tree = other.tree;
        evaluator = other.evaluator;
        expression = other.expression;
    }
    explicit Individual(ConcreteSyntaxTree& cst, const std::function<std::string(std::string)>& pevaluator = nullptr)
    {
        tree = &cst;
        expression = cst.SynthesizeExpression();
        evaluator = pevaluator;
    }
    explicit Individual(ConcreteSyntaxTree* cst, const std::function<std::string(std::string)>& pevaluator = nullptr)
    {
        tree = cst;
        expression = cst->SynthesizeExpression();
        evaluator = pevaluator;
    }
    ~Individual()
    {
        delete tree;
    }

    void SetEvaluator(const std::function<std::string(std::string)>& pevaluator)
    {
        evaluator = pevaluator;
    }

    std::string Evaluate()
    {
        if (evaluator == nullptr)
        {
            EvaluationContextType evaluationContext;
            tree->Evaluate(&evaluationContext);
            return evaluationContext.result();
        }
        else
            return evaluator(expression);
    }

    [[nodiscard]]
    ConcreteSyntaxTree GetTree() const
    {
        return *tree;
    }

    [[nodiscard]]
    std::string GetExpression() const
    {
        return expression;
    }
};