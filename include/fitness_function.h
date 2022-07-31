#pragma once
#include "syntax_tree.h"

enum class FitnessFunctionType
{
    None, Deterministic, NonDeterministic
};

class FitnessFunction
{
private:
    FitnessFunctionType _type;
    std::function<double(SyntaxTree&)> _functionPointer;
    std::optional<double> _fitnessValue = std::nullopt;
    std::string _expression;

public:
    FitnessFunction()
    {
        _type = FitnessFunctionType::None;
        _functionPointer = nullptr;
    }
    explicit FitnessFunction(const std::function<double(SyntaxTree&)>& functionPointer)
    {
        _type = FitnessFunctionType::Deterministic;
        _functionPointer = functionPointer;
    }
    FitnessFunction(const std::function<double(SyntaxTree&)>& functionPointer, FitnessFunctionType type)
    {
        _type = type;
        _functionPointer = functionPointer;
    }

    void SetFitnessFunction(const std::function<double(SyntaxTree&)>& functionPointer,
                            FitnessFunctionType type = FitnessFunctionType::Deterministic)
    {
        _type = type;
        _functionPointer = functionPointer;
    }

    [[nodiscard]]
    double Evaluate(SyntaxTree& tree)
    {
        if (_type == FitnessFunctionType::Deterministic)
        {
            std::string treeExpression = tree.SynthesizeExpression();

            if (treeExpression != _expression)
            {
                _expression = treeExpression;
                _fitnessValue = _functionPointer(tree);
            }
            else if (_fitnessValue == std::nullopt)
                _fitnessValue = _functionPointer(tree);

            return _fitnessValue.value();
        }
        else if (_type == FitnessFunctionType::NonDeterministic)
            return _functionPointer(tree);
        else
            throw std::runtime_error("Tried to evaluate an uninitialized FitnessFunction.");
    }
};