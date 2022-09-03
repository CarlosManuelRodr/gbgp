#pragma once
#include "individual.h"

/// A container for a collection of individuals. Handles their initialization and provides an interface for
/// manipulating them.
class Population
{
private:
    /// The grammar that contains rules for generating an individual.
    Grammar _generatingGrammar;

    /// The fitness function for individuals of this population.
    std::function<double(SyntaxTree&)> _fitnessFunction;

    /// The collection of individuals.
    std::vector<Individual> _individuals;

public:
    /// Population constructor.
    /// \param grammar The grammar used to initialize the individuals from the population.
    /// \param fitnessFunction The fitness function for evaluating individuals.
    Population(const Grammar& grammar, const std::function<double(SyntaxTree&)>& fitnessFunction)
    {
        _generatingGrammar = grammar;
        _fitnessFunction = fitnessFunction;
    }

    /// Initializes a population of randomly generated individuals.
    /// \param populationSize The size of the population.
    void Initialize(int populationSize)
    {
        _individuals.reserve(populationSize);

        for (int i = 0; i < populationSize; i++)
        {
            Individual newIndividual(_fitnessFunction);
            newIndividual.CreateRandom(_generatingGrammar);
            _individuals.push_back(newIndividual);
        }
    }

    /// Add an individual to the population.
    /// \param individual The individual to add.
    void AddIndividual(const Individual& individual)
    {
        _individuals.push_back(individual);
    }

    /// Get the individual at the n-th index.
    /// \param n The index.
    /// \return A reference to the individual.
    Individual& GetIndividual(size_t n)
    {
        return _individuals[n];
    }

    Individual& GetFittestByRank(int rank)
    {
        std::sort(_individuals.begin(), _individuals.end(), [](const Individual& a, const Individual& b) -> bool
        {
            return a.GetFitness() > b.GetFitness();
        });
        return _individuals[rank];
    }

    /// Reduce the population to the selected indexes.
    /// \param keepIndexes The indexes of the individuals selected to survive on the next generation.
    void ReducePopulation(const std::vector<size_t>& keepIndexes)
    {
        _individuals = extract_elements_at_indexes(_individuals, keepIndexes);
    }

    /// Evaluate all the individuals of the population.
    void Evaluate()
    {
        for (auto& ind : _individuals)
            ind.Evaluate();
    }

    /// Get the fitness values of all the individuals in the population.
    /// \return A vector with the fitness values.
    std::vector<double> GetFitness()
    {
        std::vector<double> fitnessValues;
        fitnessValues.reserve(_individuals.size());

        for (auto& ind : _individuals)
            fitnessValues.push_back(ind.GetFitness());

        return fitnessValues;
    }

    /// Get a reference to the individuals of the population.
    [[nodiscard]]
    std::vector<Individual>& GetIndividuals()
    {
        return _individuals;
    }

    /// Get the size of the population.
    [[nodiscard]]
    size_t Size() const
    {
        return _individuals.size();
    }

    /// Grammar getter.
    [[nodiscard]]
    Grammar GetGeneratingGrammar() const
    {
        return _generatingGrammar;
    }

    /// Fitness function getter.
    [[nodiscard]]
    std::function<double(SyntaxTree&)> GetFitnessFunction() const
    {
        return _fitnessFunction;
    }
};