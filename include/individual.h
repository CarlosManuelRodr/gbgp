#pragma once
#include "grammar.h"

/// An individual is a container for a syntax tree whose fitness can be evaluated through a fitness function.
class Individual
{
private:
    /// The tree that contains the genome of the individual.
    SyntaxTree _tree;

    /// The fitness function for evaluating the individual.
    std::function<double(SyntaxTree&)> _fitnessFunction;

    /// The evaluated fitness value.
    std::optional<double> _fitnessValue = std::nullopt;

public:
    Individual() = default;
    explicit Individual(const std::function<double(SyntaxTree&)>& fitnessFunction, const SyntaxTree& syntaxTree)
    {
        _fitnessFunction = fitnessFunction;
        _tree = syntaxTree;
    }
    explicit Individual(const std::function<double(SyntaxTree&)>& fitnessFunction)
    {
        _fitnessFunction = fitnessFunction;
    }
    Individual(const Individual& other)
    {
        _tree = other._tree;
        _fitnessFunction = other._fitnessFunction;
        _fitnessValue = other._fitnessValue;
    }

    /// Fitness function setter.
    void SetFitnessFunction(const std::function<double(SyntaxTree&)>& fitnessFunction)
    {
        _fitnessFunction = fitnessFunction;
    }

    /// Fitness function getter.
    [[nodiscard]]
    std::function<double(SyntaxTree&)> GetFitnessFunction() const
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

    /// Return the fitness value.
    [[nodiscard]]
    double GetFitness() const
    {
        if (_fitnessValue == std::nullopt)
            throw std::runtime_error("Tried to get unevaluated fitness value.");
        else
            return _fitnessValue.value();
    }

    /// Evaluates the fitness function and assign the fitness value.
    void Evaluate()
    {
        _fitnessValue = _fitnessFunction(_tree);
    }

    /// Prunes the tree.
    /// \param grammar The grammar that contains the prune rules.
    void Prune(const Grammar& grammar)
    {
        grammar.PruneTree(_tree);
    }

    /// Generates a random individual using the production rules and prune rules of the grammar.
    /// \param grammar The generating grammar.
    void CreateRandom(const Grammar& grammar)
    {
        grammar.CreateRandomTree(_tree);
        Prune(grammar);
    }
};