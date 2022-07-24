#pragma once
#include "population.h"

class GeneticOperators
{
public:
    static Population Selection(const Population& population, int size)
    {
        std::vector<double> fitnessScores = population.GetFitness();
        const int populationSize = static_cast<int>(fitnessScores.size());
        sort(fitnessScores.begin(), fitnessScores.end());

        // Weighted sample
        std::vector<int> weights = range(populationSize, 0, -1);
        std::vector<size_t> sampledIndexes = random_weighted_sample_indexes(weights, size);

        return population.SelectIndividuals(sampledIndexes);
    }
};