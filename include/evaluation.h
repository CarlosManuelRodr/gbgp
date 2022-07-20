#pragma once
#include <string>
#include <vector>

//*********************************
//*      Evaluation context       *
//********************************/

class EvaluationContext
{
private:
    std::string _result;
    std::vector<std::string> _semanticValues;

public:
    auto result()                     -> std::string&       { return _result; }
    [[nodiscard]] auto result() const -> const std::string& { return _result; }

    /// Get semantic value of the associated ProductionRule at the specified index.
    /// \param index Index of the ProductionRule.
    /// \return The evaluated value of the associated ProductionRule.
    [[nodiscard]]
    std::string SemanticValue(unsigned index) const
    {
        return _semanticValues.at(index);
    }

    /// Push the evaluation result of a ProductionRule for later use inside a semantic action.
    /// \param value Value to push.
    void PushSemanticValue(const std::string& value)
    {
        _semanticValues.push_back(value);
    }

    [[nodiscard]]
    unsigned NumberOfSemanticValues() const
    {
        return _semanticValues.size();
    }

    /// Virtual function that is executed before each evaluation of a semantic action.
    virtual void Prepare()
    {
        _semanticValues.clear();
        _result.clear();
    }
};