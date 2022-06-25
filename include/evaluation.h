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
    std::vector<std::string> semanticValues;

public:
    auto result()                     -> std::string&       { return _result; }
    [[nodiscard]] auto result() const -> const std::string& { return _result; }

    std::string SemanticValue(unsigned index)
    {
        return semanticValues.at(index);
    }

    void PushSemanticValue(const std::string& value)
    {
        semanticValues.push_back(value);
    }

    virtual void Prepare()
    {
        semanticValues.clear();
        _result.clear();
    }
};