#pragma once
#include <limits>
#include "grammar.h"

template <typename EvaluationContextType = EvaluationContext> class Individual
{
private:
    Grammar grammar;
    SyntaxTree tree;

    std::string expression;
    std::function<std::string(std::string)> evaluator;

public:
    explicit Individual(const Grammar& pgrammar)
    {
        grammar = pgrammar;
        evaluator = nullptr;
    }
    explicit Individual(const Grammar& pgrammar, const SyntaxTree& syntaxTree,
                        const std::function<std::string(std::string)>& pevaluator = nullptr)
    {
        grammar = pgrammar;
        tree = syntaxTree;
        expression = syntaxTree.SynthesizeExpression();
        evaluator = pevaluator;
    }
    Individual(const Individual<EvaluationContextType>& other)
    {
        grammar = other.grammar;
        tree = other.tree;
        evaluator = other.evaluator;
        expression = other.expression;
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
            tree.Evaluate(&evaluationContext);
            return evaluationContext.result();
        }
        else
            return evaluator(expression);
    }

    [[nodiscard]]
    SyntaxTree& GetTree()
    {
        return tree;
    }

    [[nodiscard]]
    std::string GetExpression() const
    {
        return expression;
    }

    void CreateRandom()
    {
        grammar.CreateRandomTree(tree);
        expression = tree.SynthesizeExpression();
    }
};