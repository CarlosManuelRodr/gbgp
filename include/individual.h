#pragma once
#include "grammar.h"

/// An individual is a container for a syntax tree whose fitness can be evaluated through a fitness function.
class Individual
{
private:
    /// The tree that contains the genome of the individual.
    SyntaxTree _tree;

    /// The fitness function for evaluating the individual.
    FitnessFunction _fitnessFunction;

public:
    Individual() = default;
    explicit Individual(const SyntaxTree& syntaxTree, const FitnessFunction& fitnessFunction)
    {
        _tree = syntaxTree;
        _fitnessFunction = fitnessFunction;
    }
    Individual(const Individual& other)
    {
        _tree = other._tree;
        _fitnessFunction = other._fitnessFunction;
    }

    void SetFitnessFunction(const FitnessFunction& fitnessFunction)
    {
        _fitnessFunction = fitnessFunction;
    }

    [[nodiscard]]
    FitnessFunction GetFitnessFunction() const
    {
        return _fitnessFunction;
    }

    /// Returns a reference to the syntax tree.
    [[nodiscard]]
    SyntaxTree& GetTree()
    {
        return _tree;
    }

    /// Synthesizes the tree expression.
    [[nodiscard]]
    std::string GetExpression() const
    {
        return _tree.SynthesizeExpression();
    }

    double GetFitness()
    {
        return _fitnessFunction.Evaluate(_tree);
    }

    /// Generates a random individual using the production rules and prune rules of the grammar.
    /// \param grammar The generating grammar.
    void CreateRandom(const Grammar& grammar)
    {
        grammar.CreateRandomTree(_tree);
        grammar.PruneTree(_tree);
    }
};