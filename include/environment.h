#pragma once
#include "genetic_operators.h"

namespace gbgp
{
    /// The environment optimizes the fitness of a population via genetic optimization.
    class Environment
    {
    private:
        int _populationSize;
        int _survivorsPerGeneration;
        int _childrenByPair;
        int _eliteIndividuals;
        int _immigrationIndividuals;
        double _mutationProbability;
        RuntimeMode _runtimeMode;
        Population _population;

        /// Generates new immigrant individuals.
        /// \param n The number of individuals.
        std::vector<Individual> GenerateImmigrationIndividuals(unsigned n)
        {
            Population immigrants(_population.GetGeneratingGrammar(), _population.GetFitnessFunction());
            immigrants.Initialize(n);
            immigrants.Evaluate(_runtimeMode);
            return immigrants.GetIndividuals();
        }

    public:
        /// Environment constructor.
        /// \param grammar The grammar used for creating and mutating individuals.
        /// \param fitnessFunction The fitness function for evaluating individuals.
        /// \param populationSize The size of the population.
        /// \param survivorsPerGeneration The number of individuals that survive the selection operator.
        /// \param eliteIndividuals The number of individuals that automatically survive to the next generation.
        /// \param immigrationIndividuals The number of new individuals to be inserted on the population on each generation.
        /// \param mutationProbability The probability that an individual mutates over a generation.
        Environment(const Grammar& grammar, const std::function<double(SyntaxTree&)>& fitnessFunction,
                    int populationSize, int survivorsPerGeneration, int eliteIndividuals,
                    int immigrationIndividuals, double mutationProbability) :
                Environment(grammar, fitnessFunction, populationSize, survivorsPerGeneration, eliteIndividuals,
                            immigrationIndividuals, mutationProbability, RuntimeMode::SingleThread) {}

        /// Environment constructor.
        /// \param grammar The grammar used for creating and mutating individuals.
        /// \param fitnessFunction The fitness function for evaluating individuals.
        /// \param populationSize The size of the population.
        /// \param survivorsPerGeneration The number of individuals that survive the selection operator.
        /// \param eliteIndividuals The number of individuals that automatically survive to the next generation.
        /// \param immigrationIndividuals The number of new individuals to be inserted on the population on each generation.
        /// \param mutationProbability The probability that an individual mutates over a generation.
        /// \param runtimeMode Whether to evaluate the population on a single thread or in a thread pool.
        Environment(const Grammar& grammar, const std::function<double(SyntaxTree&)>& fitnessFunction,
                    int populationSize, int survivorsPerGeneration, int eliteIndividuals,
                    int immigrationIndividuals, double mutationProbability,
                    RuntimeMode runtimeMode)
                    : _population(grammar, fitnessFunction)
        {
            _populationSize = populationSize;
            _survivorsPerGeneration = survivorsPerGeneration;
            _eliteIndividuals = eliteIndividuals;
            _immigrationIndividuals = immigrationIndividuals;
            _mutationProbability = mutationProbability;
            _runtimeMode = runtimeMode;

            _childrenByPair = _populationSize / _survivorsPerGeneration;

            _population.Initialize(_populationSize);
            _population.Evaluate(_runtimeMode);
        }

        /// Population getter.
        [[nodiscard]]
        Population& GetPopulation()
        {
            return _population;
        }

        /// Optimizes a population via genetic optimization.
        void Optimize()
        {
            Optimize(1);
        }

        /// Optimizes a population via genetic optimization.
        /// \param generations The number of generations to optimize.
        void Optimize(unsigned generations)
        {
            for (unsigned i = 0; i < generations; i++)
            {
                // Store the fittest individuals.
                std::vector<Individual> elite = _population.GetNthFittestByRank(_eliteIndividuals);

                // Apply the genetic operators and evaluate generation-
                GeneticOperators::Selection(_population, _survivorsPerGeneration);
                GeneticOperators::Crossover(_population, _childrenByPair);
                GeneticOperators::Mutation(_population, _mutationProbability);
                _population.Evaluate(_runtimeMode);

                // Replace worst of generation by elite individuals.
                _population.RemoveWorst(_eliteIndividuals + _immigrationIndividuals);
                _population.AddIndividuals(elite);
                _population.AddIndividuals(GenerateImmigrationIndividuals(_immigrationIndividuals));

                // Prune generation.
                _population.Prune();
            }
        }

        static std::string RuntimeModeToString(RuntimeMode runtimeMode)
        {
            return runtimeMode == RuntimeMode::MultiThread ? "MultiThread" : "SingleThread";
        }


        /// Get a string representation of this object.
        /// \return The string representation.
        [[nodiscard]]
        std::string ToString() const
        {
            std::string output = "Environment(";

            output += "populationSize='" + std::to_string(_populationSize) + "'\n";
            output += "survivorsPerGeneration='" + std::to_string(_survivorsPerGeneration) + "'\n";
            output += "childrenByPair='" + std::to_string(_childrenByPair) + "'\n";
            output += "eliteIndividuals='" + std::to_string(_eliteIndividuals) + "'\n";
            output += "immigrationIndividuals='" + std::to_string(_immigrationIndividuals) + "'\n";
            output += "mutationProbability='" + std::to_string(_mutationProbability) + "'\n";
            output += "runtimeMode='" + RuntimeModeToString(_runtimeMode) + "'\n";
            output += "population=[\n";
            output += _population.ToString();
            output += "\n])";

            return output;
        }
    };
}