#pragma once
#include <string>
#include <vector>

//*********************************
//*      Evaluation context       *
//********************************/

template <typename ValueType = std::string> class EvaluationContext
{
private:
    ValueType _result;
    std::vector<ValueType> semanticValues;

public:
    auto result()                     -> ValueType&       { return _result; }
    [[nodiscard]] auto result() const -> const ValueType& { return _result; }

    /// Get semantic value of the associated ProductionRule at the specified index.
    /// \param index Index of the ProductionRule.
    /// \return The evaluated value of the associated ProductionRule.
    [[nodiscard]]
    ValueType SemanticValue(unsigned index) const
    {
        return semanticValues.at(index);
    }

    /// Push the evaluation result of a ProductionRule for later use inside a semantic action.
    /// \param value Value to push.
    void PushSemanticValue(const ValueType& value)
    {
        semanticValues.push_back(value);
    }

    /// Virtual function that is executed before each evaluation of a semantic action.
    virtual void Prepare()
    {
        semanticValues.clear();
        _result = ValueType();
    }
};