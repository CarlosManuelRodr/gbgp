#pragma once
#include "genetic_operators.h"

/// The environment optimizes the fitness of a population via genetic optimization.
class Environment
{
private:
    int _populationSize;
    int _survivorsPerGeneration;
    int _childrenByPair;
    int _eliteIndividuals;
    double _mutationProbability;
    Population _population;

public:
    /// Environment constructor.
    /// \param grammar The grammar used for creating and mutating individuals.
    /// \param fitnessFunction The fitness function for evaluating individuals.
    /// \param populationSize The size of the population.
    /// \param survivorsPerGeneration The number of individuals that survive the selection operator.
    /// \param eliteIndividuals The number of individuals that automatically survive to the next generation.
    /// \param mutationProbability The probability that an individual mutates over a generation.
    Environment(const Grammar& grammar, const std::function<double(SyntaxTree&)>& fitnessFunction,
                int populationSize, int survivorsPerGeneration, int eliteIndividuals,
                double mutationProbability)
                : _population(grammar, fitnessFunction)
    {
        _populationSize = populationSize;
        _survivorsPerGeneration = survivorsPerGeneration;
        _eliteIndividuals = eliteIndividuals;
        _mutationProbability = mutationProbability;

        _childrenByPair = _populationSize / _survivorsPerGeneration;

        _population.Initialize(_populationSize);
        _population.Evaluate();
    }

    /// Population getter.
    [[nodiscard]]
    Population& GetPopulation()
    {
        return _population;
    }

    /// Optimizes a population via genetic optimization.
    /// \param generations The number of generations to optimize.
    void Optimize(int generations = 1)
    {
        for (int i = 0; i < generations; i++)
        {
            std::cout << "Generation " << i << std::endl;
            GeneticOperators::Selection(_population, _survivorsPerGeneration);
            GeneticOperators::Crossover(_population, _childrenByPair);
            GeneticOperators::Mutation(_population, _mutationProbability);
            _population.Evaluate();
        }
    }
};