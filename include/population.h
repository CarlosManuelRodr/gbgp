#pragma once
#include "individual.h"
#include "thread_pool.h"

namespace gbgp
{
    enum class RuntimeMode
    {
        SingleThread, MultiThread
    };

    /// A container for a collection of individuals. Handles their initialization and provides an interface for
    /// manipulating them.
    class Population
    {
    private:
        /// The grammar that contains rules for generating an individual.
        Grammar _generatingGrammar;

        /// The fitness function for individuals of this population.
        std::function<double(SyntaxTree&)> _fitnessFunction;

        /// The collection of individuals.
        std::vector<Individual> _individuals;

        /// Is the population evaluated?
        bool _isEvaluated;

        void SingleThreadEvaluate()
        {
            for (auto& ind : _individuals)
                ind.Evaluate();
        }

        void MultiThreadEvaluate()
        {
            BS::thread_pool pool;

            for (auto& ind : _individuals)
                pool.push_task(&Individual::Evaluate, &ind);

            pool.wait_for_tasks();
        }

        void SortPopulation()
        {
            // Sort individuals by descending fitness.
            std::sort(_individuals.begin(), _individuals.end(), [](const Individual& a, const Individual& b) -> bool
            {
                return a.GetFitness() > b.GetFitness();
            });
        }

    public:
        /// Population constructor.
        /// \param grammar The grammar used to initialize the individuals from the population.
        /// \param fitnessFunction The fitness function for evaluating individuals.
        Population(const Grammar& grammar, const std::function<double(SyntaxTree&)>& fitnessFunction)
        {
            _generatingGrammar = grammar;
            _fitnessFunction = fitnessFunction;
            _isEvaluated = false;
        }

        /// Initializes a population of randomly generated individuals.
        /// \param populationSize The size of the population.
        void Initialize(unsigned populationSize)
        {
            _individuals.reserve(populationSize);

            for (unsigned i = 0; i < populationSize; i++)
            {
                Individual newIndividual(_fitnessFunction);
                newIndividual.CreateRandom(_generatingGrammar);
                _individuals.push_back(newIndividual);
            }
            _isEvaluated = false;
        }

        /// Add an individual to the population.
        /// \param individual The individual to add.
        void AddIndividual(const Individual& individual)
        {
            _individuals.push_back(individual);
            _isEvaluated &= individual.IsEvaluated();

            if (_isEvaluated)
                SortPopulation();
        }

        /// Add a collection of individuals to the population.
        /// \param newIndividuals The collection of individuals to add.
        void AddIndividuals(const std::vector<Individual>& newIndividuals)
        {
            _individuals.insert(_individuals.end(), newIndividuals.begin(), newIndividuals.end());
            std::for_each(_individuals.begin(), _individuals.end(), [this](const Individual& ind) { _isEvaluated &= ind.IsEvaluated(); } );

            if (_isEvaluated)
                SortPopulation();
        }

        /// Get the individual at the n-th index.
        /// \param n The index.
        /// \return A reference to the individual.
        Individual& GetIndividual(size_t n)
        {
            return _individuals[n];
        }

        /// Get the fittest individual by rank.
        Individual& GetFittestByRank(int rank)
        {
            if (!_isEvaluated)
                throw std::runtime_error("Tried to access ranked individuals of an unevaluated population.");

            return _individuals[rank];
        }

        /// Get the fittest individuals up to rank maxRank.
        std::vector<Individual> GetNthFittestByRank(int maxRank)
        {
            if (!_isEvaluated)
                throw std::runtime_error("Tried to access ranked individuals of an unevaluated population.");

            auto from = _individuals.begin();
            auto to = _individuals.begin() + maxRank;
            return {from, to};
        }

        /// Reduce the population to the selected indexes.
        /// \param keepIndexes The indexes of the individuals selected to survive on the next generation.
        void ReducePopulation(const std::vector<size_t>& keepIndexes)
        {
            _individuals = extract_elements_at_indexes(_individuals, keepIndexes);
        }

        /// Removes the n-th worst individuals by fitness of the population.
        /// \param maxRank The maximum rank of the worst individuals to remove.
        void RemoveWorst(int maxRank)
        {
            if (!_isEvaluated)
                throw std::runtime_error("Tried to access ranked individuals of an unevaluated population.");

            auto from = _individuals.end() - maxRank;
            auto to= _individuals.end();
            _individuals.erase(from, to);
        }

        /// Prune all the individuals of the population.
        void Prune()
        {
            for (auto& ind : _individuals)
                ind.Prune(_generatingGrammar);
        }

        /// Evaluate all the individuals of the population.
        void Evaluate()
        {
            Evaluate(RuntimeMode::SingleThread);
        }

        /// Evaluate all the individuals of the population.
        /// \param runtimeMode
        void Evaluate(RuntimeMode runtimeMode)
        {
            // Evaluate individuals.
            switch (runtimeMode) {
                case RuntimeMode::SingleThread:
                    SingleThreadEvaluate();
                    break;
                case RuntimeMode::MultiThread:
                    MultiThreadEvaluate();
                    break;
            }

            SortPopulation();

            _isEvaluated = true;
        }

        /// Get the fitness values of all the individuals in the population.
        /// \return A vector with the fitness values.
        [[nodiscard]]
        std::vector<double> GetFitness() const
        {
            if (!_isEvaluated)
                throw std::runtime_error("Tried to access fitness of individuals on an unevaluated population.");

            std::vector<double> fitnessValues;
            fitnessValues.reserve(_individuals.size());

            for (auto& ind : _individuals)
                fitnessValues.push_back(ind.GetFitness());

            return fitnessValues;
        }

        /// Get a reference to the individuals of the population.
        [[nodiscard]]
        std::vector<Individual>& GetIndividuals()
        {
            return _individuals;
        }

        /// Get the size of the population.
        [[nodiscard]]
        size_t Size() const
        {
            return _individuals.size();
        }

        /// Grammar getter.
        [[nodiscard]]
        Grammar GetGeneratingGrammar() const
        {
            return _generatingGrammar;
        }

        /// Fitness function getter.
        [[nodiscard]]
        std::function<double(SyntaxTree&)> GetFitnessFunction() const
        {
            return _fitnessFunction;
        }
    };
}