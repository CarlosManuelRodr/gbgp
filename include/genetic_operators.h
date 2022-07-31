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

    static std::vector<TreeNode*> GetMutableTermsOfType(const SyntaxTree& tree, TreeNodeType type)
    {
        std::vector<TreeNode*> terms = tree.GetTermsOfType(type);
        std::vector<TreeNode*> mutableTerms = type == TreeNodeType::NonTerminal ?
                GetMutableNonTerminalNodes(terms) : GetMutableTerminalNodes(terms);
        return mutableTerms;
    }

    static void MutateIndividualTerminal(Individual& individual)
    {
        // Select random terminal.
        std::vector<TreeNode*> mutableTerminals = GetMutableTermsOfType(individual.GetTree(), TreeNodeType::Terminal);
        TreeNode* randomTerminalNode = *random_choice(mutableTerminals.begin(), mutableTerminals.end());
        Terminal randomTerminal = randomTerminalNode->termInstance;

        randomTerminalNode->termValue = randomTerminal.GetRandomValue();
    }

    static void MutateIndividualNonTerminal(Individual& individual, const Grammar& grammar)
    {
        SyntaxTree& tree = individual.GetTree();

        // Select random non-terminal.
        std::vector<TreeNode*> mutableNonTerminals = GetMutableTermsOfType(tree, TreeNodeType::NonTerminal);
        TreeNode* randomNonTerm = *random_choice(mutableNonTerminals.begin(), mutableNonTerminals.end());

        // Remove branch and create subtree.
        tree.RemoveSubtree(randomNonTerm);
        SyntaxTree replacement;
        grammar.CreateRandomTree(replacement, 50, randomNonTerm->generatorPR);
        tree.InsertSubtree(randomNonTerm, replacement);
    }

public:
    /// Implemented as proportionate ranked selection. TODO: Implement more selection operators.
    /// \param population
    /// \param size
    /// \return
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

    static Individual Crossover(Individual& parent1, Individual& parent2)
    {
        SyntaxTree treeParent1 = parent1.GetTree();
        SyntaxTree treeParent2 = parent2.GetTree();

        std::vector<TreeNode*> mutableNonTerminalsParent1 = GetMutableTermsOfType(treeParent1, TreeNodeType::NonTerminal);
        std::vector<TreeNode*> mutableNonTerminalsParent2 = GetMutableTermsOfType(treeParent2, TreeNodeType::NonTerminal);

        TreeNode* randomNonTermParent1 = *random_choice(mutableNonTerminalsParent1.begin(), mutableNonTerminalsParent1.end());

        // TODO: Get compatible nonterm
        TreeNode* randomNonTermParent2;

        treeParent1.RemoveSubtree(randomNonTermParent1);
        treeParent1.InsertSubtree(randomNonTermParent1, randomNonTermParent2);

        return Individual(treeParent1, parent1.GetFitnessFunction());
    }
};