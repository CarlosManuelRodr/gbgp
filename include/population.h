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
    FitnessFunction _fitnessFunction;

    /// The collection of individuals.
    std::vector<Individual> _individuals;

public:
    Population(const Grammar& grammar, const FitnessFunction& fitnessFunction)
    {
        _generatingGrammar = grammar;
        _fitnessFunction = fitnessFunction;
    }
    Population(const Grammar& grammar, const FitnessFunction& fitnessFunction, const std::vector<Individual>& individuals)
    {
        _generatingGrammar = grammar;
        _fitnessFunction = fitnessFunction;
        _individuals = individuals;
    }

    /// Initializes a population of randomly generated individuals.
    /// \param populationSize The size of the population.
    void Initialize(int populationSize)
    {
        _individuals.reserve(populationSize);

        for (int i = 0; i < populationSize; i++)
        {
            Individual newIndividual;
            newIndividual.CreateRandom(_generatingGrammar);
            newIndividual.SetFitnessFunction(_fitnessFunction);
            _individuals.push_back(newIndividual);
        }
    }

    /// Reduce the population to the selected indexes.
    /// \param indexes The indexes of the individuals selected to survive on the next generation.
    void SelectIndividuals(const std::vector<size_t>& selectIndexes)
    {
        _individuals = extract_elements_at_indexes(_individuals, selectIndexes);
    }

    [[nodiscard]]
    std::vector<double> GetFitness() const
    {
        std::vector<double> fitnessValues;
        fitnessValues.reserve(_individuals.size());

        for (auto ind : _individuals)
            fitnessValues.push_back(ind.GetFitness());

        return fitnessValues;
    }

    [[nodiscard]]
    std::vector<Individual> GetIndividuals() const
    {
        return _individuals;
    }

};