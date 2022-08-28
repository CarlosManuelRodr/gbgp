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

    static std::vector<NonTerminal> GetSharedNonTerminals(const std::vector<TreeNode*>& nodesParent1,
                                                          const std::vector<TreeNode*>& nodesParent2)
    {
        std::vector<NonTerminal> shared;
        for (auto n1 : nodesParent1)
        {
            if (!vector_contains_q(shared, n1->nonTermInstance))
            {
                for (auto n2 : nodesParent2)
                {
                    if (n1->nonTermInstance == n2->nonTermInstance)
                    {
                        shared.push_back(n1->nonTermInstance);
                        break;
                    }
                }
            }
        }
        return shared;
    }

    static TreeNode* GetRandomNodeOfType(const std::vector<TreeNode*>& nodes, const NonTerminal& type)
    {
        std::vector<size_t> indexes = find_indexes_if(nodes,[type](TreeNode* node){ return  type == node->nonTermInstance; });
        size_t randomSelection = *random_choice(indexes.begin(), indexes.end());
        return nodes[randomSelection];
    }

    static TreeNode* GetRandomNodeOfType(const std::vector<TreeNode*>& nodes, const std::vector<NonTerminal>& types)
    {
        std::vector<size_t> indexes = find_indexes_if(nodes,[types](TreeNode* node){ return vector_contains_q(types, node->nonTermInstance); });
        size_t randomSelection = *random_choice(indexes.begin(), indexes.end());
        return nodes[randomSelection];
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

        return population.SelectIndividualsByIndex(sampledIndexes);
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

        std::vector<NonTerminal> sharedNonTerminals = GetSharedNonTerminals(mutableNonTerminalsParent1, mutableNonTerminalsParent2);
        TreeNode* randomNonTermParent1 = GetRandomNodeOfType(mutableNonTerminalsParent1, sharedNonTerminals);
        TreeNode* randomNonTermParent2 = GetRandomNodeOfType(mutableNonTerminalsParent2, randomNonTermParent1->nonTermInstance);

        treeParent1.RemoveSubtree(randomNonTermParent1);
        treeParent1.InsertSubtree(randomNonTermParent1, randomNonTermParent2);

        return Individual(treeParent1, parent1.GetFitnessFunction());
    }
};