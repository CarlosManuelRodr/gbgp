#pragma once
#include <limits>
#include "cst_tree.h"

template <typename TerminalType, typename NonTerminalType, typename ValueType = std::string,
          typename EvaluationContextType = EvaluationContext<ValueType>>
class Individual
{
private:
    ConcreteSyntaxTree<TerminalType, NonTerminalType, ValueType>* tree;

public:
    std::string expression;
    std::function<ValueType(std::string)> evaluator;

    static Individual* NewRandomIndividual(const Grammar<TerminalType, NonTerminalType, ValueType>& grammar,
                                           const std::function<ValueType(std::string)>& pevaluator = nullptr,
                                           int maxDepth = 100)
    {
        auto* newTree = new ConcreteSyntaxTree<TerminalType, NonTerminalType, ValueType>();
        newTree->CreateRandomTree(grammar, maxDepth);
        return new Individual(newTree, pevaluator);
    }

    Individual()
    {
        tree = nullptr;
        evaluator = nullptr;
    }
    Individual(const Individual<TerminalType, NonTerminalType, ValueType, EvaluationContextType>& other)
    {
        tree = other.tree;
        evaluator = other.evaluator;
        expression = other.expression;
    }
    explicit Individual(ConcreteSyntaxTree<TerminalType, NonTerminalType, ValueType>& cst,
                        const std::function<ValueType(std::string)>& pevaluator = nullptr)
    {
        tree = cst;
        expression = cst.SynthesizeExpression();
        evaluator = pevaluator;
    }
    explicit Individual(ConcreteSyntaxTree<TerminalType, NonTerminalType, ValueType>* cst,
                        const std::function<ValueType(std::string)>& pevaluator = nullptr)
    {
        tree = cst;
        expression = cst->SynthesizeExpression();
        evaluator = pevaluator;
    }
    ~Individual()
    {
        delete tree;
    }

    void SetEvaluator(const std::function<ValueType(std::string)>& pevaluator)
    {
        evaluator = pevaluator;
    }

    ValueType Evaluate()
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
    ConcreteSyntaxTree<TerminalType, NonTerminalType, ValueType> GetTree() const
    {
        return *tree;
    }

    [[nodiscard]]
    std::string GetExpression() const
    {
        return expression;
    }
};