#pragma once
#include <string>
#include <vector>

//*********************************
//*      Evaluation context       *
//********************************/

/// An evaluation context is used to evaluate the tree using the production rules of the grammar. To extend
/// its functionality, inherit from this base class.
class EvaluationContext
{
private:
    /// The result value of each semantic action.
    std::string _result;

    /// The semantic values used at each semantic action.
    std::vector<std::string> _semanticValues;

public:

    // Shorthand getter/setter
    auto result()                     -> std::string&       { return _result; }
    [[nodiscard]] auto result() const -> const std::string& { return _result; }

    std::string GetResult() { return result(); }
    void SetResult(const std::string& s) { result() = s; }

    /// Get semantic value of the associated ProductionRule at the specified index.
    /// \param index Index of the ProductionRule.
    /// \return The evaluated value of the associated ProductionRule.
    [[nodiscard]]
    std::string SemanticValue(unsigned index) const
    {
        return _semanticValues.at(index);
    }

    /// Get the list of semantic values.
    [[nodiscard]]
    std::vector<std::string> GetSemanticValues() const
    {
        return _semanticValues;
    }

    /// Push the evaluation result of a ProductionRule for later use inside a semantic action.
    /// \param value Value to push.
    void PushSemanticValue(const std::string& value)
    {
        _semanticValues.push_back(value);
    }

    /// Get the total number of semantic values.
    [[nodiscard]]
    unsigned NumberOfSemanticValues() const
    {
        return _semanticValues.size();
    }

    /// Transfer the semantic value at the specified index to the result.
    /// \param index The semantic value index.
    void TransferSemanticValueToResult(unsigned index = 0)
    {
        result() = SemanticValue(index);
    }

    /// Virtual function that is executed before each evaluation of a semantic action.
    virtual void Prepare()
    {
        _semanticValues.clear();
        _result.clear();
    }

    /// Get the string representation.
    [[nodiscard]]
    std::string ToString() const
    {
        return "result='" + result() + "', semanticValues='" + vector_to_string(GetSemanticValues());
    }
};