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

    static void MutateIndividualNonTerm(Individual& individual, const Grammar& grammar)
    {
        SyntaxTree tree = individual.GetTree();

        // Select random non-term.
        std::vector<TreeNode*> nonTerminals = tree.GetTermsOfType(TreeNodeType::NonTerminal);
        TreeNode* randomNonTerm = *random_choice(nonTerminals.begin(), nonTerminals.end());

        // Remove branch and create subtree.
        tree.RemoveSubtree(randomNonTerm);
        SyntaxTree replacement;
        grammar.CreateRandomTree(replacement, 50, randomNonTerm->generatorPR);
        tree.InsertSubtree(randomNonTerm, replacement);
    }
};