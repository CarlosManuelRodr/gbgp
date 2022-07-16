#pragma once
#include "individual.h"

template <typename EvaluationContextType = EvaluationContext> class Population
{
private:
    Grammar generatingGrammar;
    std::vector<Individual<EvaluationContextType>*> individuals;

    int populationSize;

public:
    explicit Population(const Grammar& grammar, int population)
    {
        generatingGrammar = grammar;
        populationSize = population;
        individuals.reserve(populationSize);
    }
    ~Population()
    {
        for (int i = 0; i < individuals.size(); i++)
            delete individuals[i];
    }

    void Initialize()
    {
        for (int i = 0; i < populationSize; i++)
        {
            auto* newIndividual = new Individual<EvaluationContextType>();
            newIndividual->CreateRandom(generatingGrammar);
            individuals.push_back(newIndividual);
        }
    }
};