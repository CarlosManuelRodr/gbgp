#pragma once
#include "population.h"

class GeneticOperators
{
private:
    static std::vector<TreeNode*> GetMutableTerminalNodes(const std::vector<TreeNode*>& terminalNodes)
    {
        std::vector<TreeNode*> mutableNodes;
        for (auto node : terminalNodes)
            if (node->termInstance.IsMutable())
                mutableNodes.push_back(node);

        return mutableNodes;
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

    static void MutateIndividualTerminal(Individual& individual)
    {
        SyntaxTree& tree = individual.GetTree();

        // Select random term.
        std::vector<TreeNode*> terminals = tree.GetTermsOfType(TreeNodeType::Terminal);
        std::vector<TreeNode*> mutableTerminals = GetMutableTerminalNodes(terminals);
        TreeNode* randomTerminalNode = *random_choice(mutableTerminals.begin(), mutableTerminals.end());
        Terminal randomTerminal = randomTerminalNode->termInstance;

        randomTerminalNode->termValue = *random_choice(randomTerminal.values.begin(), randomTerminal.values.end());
    }

    static void MutateIndividualNonTerminal(Individual& individual, const Grammar& grammar)
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