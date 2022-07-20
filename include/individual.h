#pragma once
#include <limits>
#include "grammar.h"

/// An individual is a container for a syntax tree that can be evaluated. It also provides an interface for
/// generating random syntax tree and it is the target of genetic operators.
/// \tparam EvaluationContextType
class Individual
{
private:
    SyntaxTree tree;

    /// External evaluator. Used when there are no semantic actions on the syntax tree to perform an evaluation.
    std::function<std::string(std::string)> evaluator;

public:
    Individual()
    {
        evaluator = nullptr;
    }
    explicit Individual(const SyntaxTree& syntaxTree, const std::function<std::string(std::string)>& pevaluator = nullptr)
    {
        tree = syntaxTree;
        evaluator = pevaluator;
    }
    Individual(const Individual& other)
    {
        tree = other.tree;
        evaluator = other.evaluator;
    }

    /// Set-up an external evaluator. Used when there are no semantic actions on the syntax tree to perform an evaluation.
    /// \param pevaluator Function pointer to the external evaluator.
    void SetEvaluator(const std::function<std::string(std::string)>& pevaluator)
    {
        evaluator = pevaluator;
    }

    /// Returns a reference to the syntax tree.
    [[nodiscard]]
    SyntaxTree& GetTree()
    {
        return tree;
    }

    /// Synthesizes the tree expression.
    [[nodiscard]]
    std::string GetExpression() const
    {
        return tree.SynthesizeExpression();
    }

    /// Generates a random individual using the production rules and prune rules of the grammar.
    /// \param grammar The generating grammar.
    void CreateRandom(const Grammar& grammar)
    {
        grammar.CreateRandomTree(tree);
        grammar.PruneTree(tree);
    }
};