#pragma once
#include "population.h"

/// Genetic operators are applied over a population to modify its state according to the rules
/// of genetic optimization.
class GeneticOperators
{
private:
    /// Get a random bool with probability p of being true.
    /// \param p The probability of returning true.
    /// \return The random bool.
    static bool RandomBool(const double p = 0.5)
    {
        static auto dev = std::random_device();
        static auto gen = std::mt19937{dev()};
        static auto dist = std::uniform_real_distribution<double>(0,1);
        return (dist(gen) < p);
    }

    /// Get a list of the mutable terminal nodes. For a terminal node to be mutable it needs to have more than one
    /// possible value.
    /// \param terminalNodes A list of terminals.
    /// \return The subset of terminalNodes that are mutable.
    static std::vector<TreeNode*> GetMutableTerminalNodes(const std::vector<TreeNode*>& terminalNodes)
    {
        std::vector<TreeNode*> mutableNodes;
        for (auto node : terminalNodes)
            if (node->termInstance.IsMutable())
                mutableNodes.push_back(node);

        return mutableNodes;
    }

    /// Get a list of the mutable non-terminal nodes. For a non-terminal node to be mutable it needs to have a parent.
    /// \param nonTerminalNodes A list of non-terminals.
    /// \return The subset of nonTerminalNodes that are mutable.
    static std::vector<TreeNode*> GetMutableNonTerminalNodes(const std::vector<TreeNode*>& nonTerminalNodes)
    {
        std::vector<TreeNode*> mutableNodes;
        for (auto node : nonTerminalNodes)
            if (node->parent != nullptr)
                mutableNodes.push_back(node);

        return mutableNodes;
    }

    /// Get mutable nodes of the specified type.
    /// \param tree The tree.
    /// \param type Can be Terminal or NonTerminal.
    /// \return A list of mutable nodes.
    static std::vector<TreeNode*> GetMutableTermsOfType(const SyntaxTree& tree, TreeNodeType type)
    {
        std::vector<TreeNode*> terms = tree.GetTermsOfType(type);
        std::vector<TreeNode*> mutableTerms = type == TreeNodeType::NonTerminal ?
                GetMutableNonTerminalNodes(terms) : GetMutableTerminalNodes(terms);
        return mutableTerms;
    }

    /// Operator to mutate a random terminal from an individual.
    /// \param individual The target individual.
    static void MutateIndividualTerminal(Individual& individual)
    {
        // Select random terminal.
        std::vector<TreeNode*> mutableTerminals = GetMutableTermsOfType(individual.GetTree(), TreeNodeType::Terminal);
        TreeNode* randomTerminalNode = *random_choice(mutableTerminals.begin(), mutableTerminals.end());
        Terminal randomTerminal = randomTerminalNode->termInstance;

        randomTerminalNode->termValue = randomTerminal.GetRandomValue();
    }

    /// Operator to mutate a random non-terminal from an individual.
    /// \param individual The target individual.
    /// \param grammar The grammar rules to build the mutated branch.
    static void MutateIndividualNonTerminal(Individual& individual, const Grammar& grammar)
    {
        SyntaxTree& tree = individual.GetTree();

        // Select random non-terminal.
        std::vector<TreeNode*> mutableNonTerminals = GetMutableTermsOfType(tree, TreeNodeType::NonTerminal);
        TreeNode* randomNonTerm = *random_choice(mutableNonTerminals.begin(), mutableNonTerminals.end());

        // Remove branch and create subtree.
        tree.DeleteSubtree(randomNonTerm);
        SyntaxTree replacement;
        grammar.CreateRandomTree(replacement, 50, randomNonTerm->generatorPR);
        tree.InsertSubtree(randomNonTerm, replacement);
    }

    /// Returns the common type non-terminals from both parents.
    /// \param nodesParent1 Traversal of parent 1.
    /// \param nodesParent2 Traversal of parent 2.
    /// \return A list of non-terminals that are shared by both parents.
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

    /// Returns a random node of the specified type.
    /// \param nodes The list of nodes.
    /// \param type The node type (Terminal/NonTerminal).
    /// \return A randomly selected node of the specified type.
    static TreeNode* GetRandomNodeOfType(const std::vector<TreeNode*>& nodes, const NonTerminal& type)
    {
        std::vector<size_t> indexes = find_indexes_if(nodes,[type](TreeNode* node){ return  type == node->nonTermInstance; });
        size_t randomSelection = *random_choice(indexes.begin(), indexes.end());
        return nodes[randomSelection];
    }

    /// Returns a random node from any of the specified type.
    /// \param nodes The list of nodes.
    /// \param types The node type (Terminal/NonTerminal).
    /// \return A randomly selected node with any of the specified types.
    static TreeNode* GetRandomNodeOfType(const std::vector<TreeNode*>& nodes, const std::vector<NonTerminal>& types)
    {
        std::vector<size_t> indexes = find_indexes_if(nodes,[types](TreeNode* node){ return vector_contains_q(types, node->nonTermInstance); });
        size_t randomSelection = *random_choice(indexes.begin(), indexes.end());
        return nodes[randomSelection];
    }

public:

    /// Generates a new offspring individual that combines genome features from both parents.
    /// \param parent1 The first parent.
    /// \param parent2 The second parent.
    /// \return A new offspring individual.
    static Individual IndividualsCrossover(Individual& parent1, Individual& parent2)
    {
        SyntaxTree treeParent1 = parent1.GetTree();
        SyntaxTree treeParent2 = parent2.GetTree();

        std::vector<TreeNode*> mutableNonTerminalsParent1 = GetMutableTermsOfType(treeParent1, TreeNodeType::NonTerminal);
        std::vector<TreeNode*> mutableNonTerminalsParent2 = GetMutableTermsOfType(treeParent2, TreeNodeType::NonTerminal);

        std::vector<NonTerminal> sharedNonTerminals = GetSharedNonTerminals(mutableNonTerminalsParent1, mutableNonTerminalsParent2);
        TreeNode* randomNonTermParent1 = GetRandomNodeOfType(mutableNonTerminalsParent1, sharedNonTerminals);
        TreeNode* randomNonTermParent2 = GetRandomNodeOfType(mutableNonTerminalsParent2, randomNonTermParent1->nonTermInstance);

        treeParent1.DeleteSubtree(randomNonTermParent1);
        treeParent1.InsertSubtree(randomNonTermParent1, randomNonTermParent2);

        return Individual(parent1.GetFitnessFunction(), treeParent1);
    }

    /// The selection operator. Reduces the population to its fittest individuals.
    /// Implemented as proportionate ranked selection.
    /// \param population The target population.
    /// \param size The number of individuals that will survive.
    static void Selection(Population& population, int size)
    {
        std::vector<double> fitnessScores = population.GetFitness();
        const int populationSize = static_cast<int>(fitnessScores.size());
        sort(fitnessScores.begin(), fitnessScores.end());

        // Weighted sample
        std::vector<int> weights = range(populationSize, 0, -1);
        std::vector<size_t> sampledIndexes = random_weighted_sample_indexes(weights, size);

        population.ReducePopulation(sampledIndexes);
    }

    /// The crossover operator. Creates a new generation by reproduction of the individuals.
    /// \param population The target population.
    static void Crossover(Population& population)
    {
        Crossover(population, 1);
    }

    /// The crossover operator. Creates a new generation by reproduction of the individuals.
    /// \param population The target population.
    /// \param offspringSize The number of children individuals produced by each parent pair.
    static void Crossover(Population& population, unsigned offspringSize)
    {
        Population newGeneration(population.GetGeneratingGrammar(), population.GetFitnessFunction());

        std::vector<size_t> randomPairings = range(population.Size());
        shuffle(randomPairings);

        for (size_t i = 0; i < randomPairings.size(); i += 2)
        {
            Individual& parent1 = population.GetIndividual(i);
            Individual& parent2 = population.GetIndividual(i+1);

            // Generate offspring.
            for (unsigned j = 0; j < offspringSize; j++)
                newGeneration.AddIndividual(IndividualsCrossover(parent1, parent2));
        }

        population = newGeneration;
    }

    /// Mutation operator that acts over an individual. Mutation probability is 50%.
    /// \param individual The target individual.
    /// \param grammar The generating grammar used for NonTerminal mutation.
    static void MutateIndividual(Individual& individual, const Grammar& grammar)
    {
        MutateIndividual(individual, grammar, 0.5);
    }

    /// Mutation operator that acts over an individual.
    /// \param individual The target individual.
    /// \param grammar The generating grammar used for NonTerminal mutation.
    /// \param nonTermMutationProb The probability that the mutation is applied over a NonTerminal.
    static void MutateIndividual(Individual& individual, const Grammar& grammar, double nonTermMutationProb)
    {
        if (RandomBool(nonTermMutationProb))
            MutateIndividualNonTerminal(individual, grammar);
        else
            MutateIndividualTerminal(individual);
    }

    /// Mutation operator that acts over a population.
    /// \param population The target population.
    /// \param mutationProbability The probability that a mutation is applied over an individual.
    static void Mutation(Population& population, double mutationProbability)
    {
        Mutation(population, mutationProbability, 0.5);
    }

    /// Mutation operator that acts over a population.
    /// \param population The target population.
    /// \param mutationProbability The probability that a mutation is applied over an individual.
    /// \param nonTermMutationProbability The probability that the mutation is applied over a NonTerminal.
    static void Mutation(Population& population, double mutationProbability, double nonTermMutationProbability)
    {
        for (size_t i = 0; i < population.Size(); i++)
        {
            if (RandomBool(mutationProbability))
                MutateIndividual(population.GetIndividual(i), population.GetGeneratingGrammar(), nonTermMutationProbability);
        }
    }
};