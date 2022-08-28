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

    [[nodiscard]]
    Population SelectIndividualsByIndex(const std::vector<size_t>& indexes) const
    {
        std::vector<Individual> selectedIndividuals = extract_elements_at_indexes(_individuals, indexes);
        return { _generatingGrammar, _fitnessFunction, selectedIndividuals };
    }
};