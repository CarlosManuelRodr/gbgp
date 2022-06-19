#pragma once
#include "vector_ops.h"

/****************************
*         Terminal          *
****************************/

template <typename TerminalType> struct Terminal
{
    TerminalType id;
    std::string label;
    std::vector<std::string> values;

    Terminal()
    {
        label = '\0';
    }
    Terminal(TerminalType pid, const std::string& psymbol)
    {
        id = pid;
        label = psymbol;
        values.push_back(psymbol);
    }
    Terminal(TerminalType pid, const std::string& psymbol, const std::vector<std::string>& pvalues)
    {
        id = pid;
        label = psymbol;
        values = pvalues;
    }

    bool operator==(const Terminal& other) const
    {
        return id == other.id;
    }
    bool operator!=(const Terminal& other) const
    {
        return id != other.id;
    }
};

/****************************
*        NonTerminal        *
****************************/

template <typename NonTerminalType> struct NonTerminal
{
    NonTerminalType id;
    std::string label;

    NonTerminal()
    {
        label = '\0';
    }
    NonTerminal(NonTerminalType pid, const std::string& psymbol)
    {
        id = pid;
        label = psymbol;
    }

    bool operator==(const NonTerminal<NonTerminalType>& other) const
    {
        return id == other.id;
    }
    bool operator!=(const NonTerminal<NonTerminalType>& other) const
    {
        return id != other.id;
    }
};

/********************************
* Production element definition *
********************************/

enum class ProductionElementType
{
    Unassigned, NonTerminal, Terminal
};

template <typename TerminalType, typename NonTerminalType> struct ProductionElement
{
    ProductionElementType type;
    NonTerminal<NonTerminalType> nonterm;
    Terminal<TerminalType> term;

    ProductionElement()
    {
        type = ProductionElementType::Unassigned;
        nonterm = NonTerminal<NonTerminalType>();
        term = Terminal<TerminalType>();
    }
    explicit ProductionElement(const Terminal<TerminalType>& pterm)
    {
        type = ProductionElementType::Terminal;
        term = pterm;
        nonterm = NonTerminal<NonTerminalType>();
    }
    explicit ProductionElement(const NonTerminal<NonTerminalType>& pnonterm)
    {
        type = ProductionElementType::NonTerminal;
        term = Terminal<TerminalType>();
        nonterm = pnonterm;
    }

    bool operator==(const ProductionElement<TerminalType, NonTerminalType>& other) const
    {
        return type == other.type && nonterm == other.nonterm && term == other.term;
    }
    bool operator!=(const ProductionElement<TerminalType, NonTerminalType>& other) const
    {
        return type != other.type || nonterm != other.nonterm || term != other.term;
    }
};

/********************************
*  Semantic element definition  *
********************************/

enum class SemanticElementType
{
    NonTerminal, Terminal, String
};

template <typename TerminalType, typename NonTerminalType> struct SemanticElement
{
    SemanticElementType type;
    NonTerminal<NonTerminalType> nonterm;
    Terminal<TerminalType> term;
    std::string string;

    explicit SemanticElement(const Terminal<TerminalType>& pterm)
    {
        type = SemanticElementType::Terminal;
        term = pterm;
        nonterm = NonTerminal<NonTerminalType>();
        string = "";
    }
    explicit SemanticElement(const NonTerminal<NonTerminalType>& pnonterm)
    {
        type = SemanticElementType::NonTerminal;
        term = Terminal<TerminalType>();
        nonterm = pnonterm;
        string = "";
    }
    explicit SemanticElement(const std::string& pstring)
    {
        type = SemanticElementType::String;
        term = Terminal<TerminalType>();
        nonterm = NonTerminal<NonTerminalType>();
        string = pstring;
    }

    bool operator==(const SemanticElement<TerminalType, NonTerminalType>& other) const
    {
        return type == other.type && nonterm == other.nonterm && term == other.term && string == other.string;
    }
    bool operator!=(const SemanticElement<TerminalType, NonTerminalType>& other) const
    {
        return type != other.type || nonterm != other.nonterm || term != other.term || string == other.string;
    }
};

/********************************
*      Evaluation context       *
********************************/

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

/********************************
*   Production rule definition  *
********************************/

template <typename TerminalType, typename NonTerminalType> struct ProductionRule
{
    NonTerminal<NonTerminalType> from;
    std::vector<ProductionElement<TerminalType, NonTerminalType>> to;
    std::vector<SemanticElement<TerminalType, NonTerminalType>> semanticRules;
    std::function<void(EvaluationContext*)> semanticAction;

    ProductionRule()
    {
        from = NonTerminal<NonTerminalType>();
        semanticAction = nullptr;
    }
    ProductionRule(
        const NonTerminal<NonTerminalType>& pfrom,
        const std::vector<ProductionElement<TerminalType, NonTerminalType>>& pto,
        const std::vector<SemanticElement<TerminalType, NonTerminalType>>& psemrules,
        std::function<void(EvaluationContext*)> pSemanticAction = nullptr)
    {
        from = pfrom;
        to = pto;
        semanticRules = psemrules;
        semanticAction = pSemanticAction;
    }

    int ElementsToSynthesize()
    {
        return static_cast<int>(to.size());
    }

    bool operator==(const ProductionRule<TerminalType, NonTerminalType>& other) const
    {
        return from == other.from && to == other.to && semanticRules == other.semanticRules;
    }
    bool operator!=(const ProductionRule<TerminalType, NonTerminalType>& other) const
    {
        return from != other.from || to != other.to || semanticRules != other.semanticRules;
    }
};


/********************************
*   Formal grammar container    *
********************************/

template <typename TerminalType, typename NonTerminalType> class Grammar
{
private:
    std::vector<ProductionRule<TerminalType, NonTerminalType>> grammarRules;

public:
    Grammar() = default;
    Grammar(std::initializer_list<ProductionRule<TerminalType, NonTerminalType>> productionRuleList) : grammarRules(productionRuleList) {}
    Grammar(const Grammar<TerminalType, NonTerminalType>& other) : grammarRules(other.grammarRules) {}

    [[maybe_unused]] explicit Grammar(const std::vector<ProductionRule<TerminalType, NonTerminalType>>& productionRuleList) : grammarRules(productionRuleList) {}

    NonTerminal<NonTerminalType> GetRoot()
    {
        return grammarRules.front().from;
    }

    [[maybe_unused]] int GetProductionRuleIndex(ProductionRule<TerminalType, NonTerminalType> pr)
    {
        return find_index_of(grammarRules, pr);
    }

    std::vector<ProductionRule<TerminalType, NonTerminalType>> GetCompatibleRules(NonTerminalType fromNonTermType)
    {
        std::vector<ProductionRule<TerminalType, NonTerminalType>> compatibleRules;
        for (ProductionRule<TerminalType, NonTerminalType> rule : grammarRules)
        {
            if (rule.from.id == fromNonTermType)
                compatibleRules.push_back(rule);
        }

        return compatibleRules;
    }

    ProductionRule<TerminalType, NonTerminalType> GetRandomCompatibleRule(NonTerminalType fromNonTermType)
    {
        std::vector<ProductionRule<TerminalType, NonTerminalType>> compatibleRules = this->GetCompatibleRules(fromNonTermType);
        return *select_randomly(compatibleRules.begin(), compatibleRules.end());
    }

    ProductionRule<TerminalType, NonTerminalType> GetRandomRootRule()
    {
        return GetRandomCompatibleRule(this->GetRoot().id);
    }

    [[maybe_unused]] unsigned Size()
    {
        return static_cast<unsigned>(grammarRules.size());
    }

    unsigned IndexOfRule(const ProductionRule<TerminalType, NonTerminalType>& rule)
    {
        return find_index_of(grammarRules,rule);
    }

    ProductionRule<TerminalType, NonTerminalType>& operator[](int index)
    {
        return grammarRules[index];
    }

    Grammar<TerminalType, NonTerminalType>& operator=(const Grammar<TerminalType, NonTerminalType>& other)
    {
        grammarRules = other.grammarRules;
        return *this;
    }
};