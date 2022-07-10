#pragma once
#include <limits>
#include "syntax_tree.h"

template <typename EvaluationContextType = EvaluationContext> class Individual
{
private:
    SyntaxTree* tree;

public:
    std::string expression;
    std::function<std::string(std::string)> evaluator;

    static Individual<EvaluationContextType>*
    NewRandomIndividual(const Grammar& grammar, const std::function<std::string(std::string)>& pevaluator = nullptr,
                        int maxDepth = 100)
    {
        auto* newTree = new SyntaxTree();
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
    explicit Individual(SyntaxTree& syntaxTree, const std::function<std::string(std::string)>& pevaluator = nullptr)
    {
        tree = &syntaxTree;
        expression = syntaxTree.SynthesizeExpression();
        evaluator = pevaluator;
    }
    explicit Individual(SyntaxTree* syntaxTree, const std::function<std::string(std::string)>& pevaluator = nullptr)
    {
        tree = syntaxTree;
        expression = syntaxTree->SynthesizeExpression();
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
    SyntaxTree GetTree() const
    {
        return *tree;
    }

    [[nodiscard]]
    std::string GetExpression() const
    {
        return expression;
    }
};