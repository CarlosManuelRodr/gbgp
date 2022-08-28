#pragma once
#include "genetic_operators.h"

/// The environment optimizes the fitness of a population via genetic optimization.
class Environment
{
private:
    int _populationSize;
    int _survivorsPerGeneration;
    int _eliteIndividuals;
    double _mutationProbability;
    Population _population;

public:
    Environment(const Grammar& grammar, const FitnessFunction& fitnessFunction,
                int populationSize, int survivorsPerGeneration, int eliteIndividuals,
                double mutationProbability)
                : _population(grammar, fitnessFunction)
    {
        _populationSize = populationSize;
        _survivorsPerGeneration = survivorsPerGeneration;
        _eliteIndividuals = eliteIndividuals;
        _mutationProbability = mutationProbability;
    }

    void Optimize(int generations = 1)
    {
        _population.Initialize(_populationSize);

        for (int i = 0; i < generations; i++)
        {
            GeneticOperators::Selection(_population, _survivorsPerGeneration, _eliteIndividuals);
            GeneticOperators::Crossover(_population);
            GeneticOperators::Mutation(_population, _mutationProbability);
        }
    }
};