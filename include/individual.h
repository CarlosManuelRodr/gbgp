#pragma once
#include <limits>
#include "grammar.h"

/// An individual is a container for a syntax _tree that can be evaluated. It also provides an interface for
/// generating random syntax _tree and it is the target of genetic operators.
class Individual
{
private:
    SyntaxTree _tree;
    std::function<double(SyntaxTree&)> _fitnessFunction;

public:
    Individual()
    {
        _fitnessFunction = nullptr;
    }
    explicit Individual(const SyntaxTree& syntaxTree, const std::function<double(SyntaxTree&)>& fitnessFunction = nullptr)
    {
        _tree = syntaxTree;
        _fitnessFunction = fitnessFunction;
    }
    Individual(const Individual& other)
    {
        _tree = other._tree;
        _fitnessFunction = other._fitnessFunction;
    }

    void SetEvaluator(const std::function<double(SyntaxTree&)>& fitnessFunction)
    {
        _fitnessFunction = fitnessFunction;
    }

    /// Returns a reference to the syntax _tree.
    [[nodiscard]]
    SyntaxTree& GetTree()
    {
        return _tree;
    }

    /// Synthesizes the _tree expression.
    [[nodiscard]]
    std::string GetExpression() const
    {
        return _tree.SynthesizeExpression();
    }

    /// Generates a random individual using the production rules and prune rules of the grammar.
    /// \param grammar The generating grammar.
    void CreateRandom(const Grammar& grammar)
    {
        grammar.CreateRandomTree(_tree);
        grammar.PruneTree(_tree);
    }
};