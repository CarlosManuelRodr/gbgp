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

    void AddIndividual(const Individual& individual)
    {
        _individuals.push_back(individual);
    }

    Individual& GetIndividual(size_t index)
    {
        return _individuals[index];
    }

    Individual& GetFittestByRank(int rank)
    {
        std::sort(_individuals.begin(), _individuals.end(), [](Individual a, Individual b) -> bool
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

    void Evaluate()
    {
        for (auto& ind : _individuals)
            ind.Evaluate();
    }

    std::vector<double> GetFitness()
    {
        std::vector<double> fitnessValues;
        fitnessValues.reserve(_individuals.size());

        for (auto& ind : _individuals)
            fitnessValues.push_back(ind.GetFitness());

        return fitnessValues;
    }

    [[nodiscard]]
    std::vector<Individual> GetIndividuals() const
    {
        return _individuals;
    }

    [[nodiscard]]
    size_t Size() const
    {
        return _individuals.size();
    }

    [[nodiscard]]
    Grammar GetGeneratingGrammar() const
    {
        return _generatingGrammar;
    }

    [[nodiscard]]
    std::function<double(SyntaxTree&)> GetFitnessFunction() const
    {
        return _fitnessFunction;
    }
};