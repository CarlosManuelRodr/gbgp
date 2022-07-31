#pragma once
#include "population.h"

class GeneticOperators
{
private:
    static bool RandomBool(const double p = 0.5)
    {
        static auto dev = std::random_device();
        static auto gen = std::mt19937{dev()};
        static auto dist = std::uniform_real_distribution<double>(0,1);
        return (dist(gen) < p);
    }

    static std::vector<TreeNode*> GetMutableTerminalNodes(const std::vector<TreeNode*>& terminalNodes)
    {
        std::vector<TreeNode*> mutableNodes;
        for (auto node : terminalNodes)
            if (node->termInstance.IsMutable())
                mutableNodes.push_back(node);

        return mutableNodes;
    }

    static std::vector<TreeNode*> GetMutableNonTerminalNodes(const std::vector<TreeNode*>& nonTerminalNodes)
    {
        std::vector<TreeNode*> mutableNodes;
        for (auto node : nonTerminalNodes)
            if (node->parent != nullptr)
                mutableNodes.push_back(node);

        return mutableNodes;
    }

    static void MutateIndividualTerminal(Individual& individual)
    {
        SyntaxTree& tree = individual.GetTree();

        // Select random term.
        std::vector<TreeNode*> terminals = tree.GetTermsOfType(TreeNodeType::Terminal);
        std::vector<TreeNode*> mutableTerminals = GetMutableTerminalNodes(terminals);
        TreeNode* randomTerminalNode = *random_choice(mutableTerminals.begin(), mutableTerminals.end());
        Terminal randomTerminal = randomTerminalNode->termInstance;

        randomTerminalNode->termValue = randomTerminal.GetRandomValue();
    }

    static void MutateIndividualNonTerminal(Individual& individual, const Grammar& grammar)
    {
        SyntaxTree& tree = individual.GetTree();

        // Select random non-term.
        std::vector<TreeNode*> nonTerminals = tree.GetTermsOfType(TreeNodeType::NonTerminal);
        std::vector<TreeNode*> mutableNonTerminals = GetMutableNonTerminalNodes(nonTerminals);
        TreeNode* randomNonTerm = *random_choice(mutableNonTerminals.begin(), mutableNonTerminals.end());

        // Remove branch and create subtree.
        tree.RemoveSubtree(randomNonTerm);
        SyntaxTree replacement;
        grammar.CreateRandomTree(replacement, 50, randomNonTerm->generatorPR);
        tree.InsertSubtree(randomNonTerm, replacement);
    }

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

    static void MutateIndividual(Individual& individual, const Grammar& grammar, double nonTermMutationProb = 0.5)
    {
        if (RandomBool(nonTermMutationProb))
            MutateIndividualNonTerminal(individual, grammar);
        else
            MutateIndividualTerminal(individual);
    }
};