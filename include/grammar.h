#pragma once
#include <utility>
#include "evaluation.h"
#include "vector_ops.h"

//*****************************
//*         Terminal          *
//****************************/

/// Terminal term.
struct Terminal
{
    int id;
    std::string label;
    std::vector<std::string> values;

    Terminal()
    {
        id = -1;
        label = '\0';
    }
    Terminal(int pid, const std::string& plabel)
    {
        id = pid;
        label = plabel;
        values.push_back(plabel);
    }
    Terminal(int pid, const std::string& plabel, const std::vector<std::string>& pvalues)
    {
        id = pid;
        label = plabel;
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

    [[nodiscard]]
    std::string GetValue() const
    {
        if (values.size() == 1)
            return values.front();
        else
        {
            // Select a terminal value randomly.
            std::vector<size_t> applicableTerminals = range<size_t>(values.size());
            const size_t r = *random_choice(applicableTerminals.begin(), applicableTerminals.end());
            return values[r];
        }
    }
};

//*****************************
//*        NonTerminal        *
//****************************/

/// Non-Terminal term.
struct NonTerminal
{
    int id;
    std::string label;

    NonTerminal()
    {
        id = -1;
        label = '\0';
    }
    NonTerminal(int pid, const std::string& plabel)
    {
        id = pid;
        label = plabel;
    }

    bool operator==(const NonTerminal& other) const
    {
        return id == other.id;
    }
    bool operator!=(const NonTerminal& other) const
    {
        return id != other.id;
    }
};

//*********************************
//* Production element definition *
//********************************/

enum class ProductionElementType
{
    Unassigned, NonTerminal, Terminal
};

/// Single target element of a production rule. Used as a container for building a ProductionRule.
/// May contain a Terminal or a Non-Terminal.
struct ProductionElement
{
    ProductionElementType type;
    NonTerminal nonterm;
    Terminal term;

    ProductionElement()
    {
        type = ProductionElementType::Unassigned;
        nonterm = NonTerminal();
        term = Terminal();
    }
    explicit ProductionElement(const Terminal& pterm)
    {
        type = ProductionElementType::Terminal;
        term = pterm;
        nonterm = NonTerminal();
    }
    ProductionElement(int pid, const std::string& psymbol)
    {
        type = ProductionElementType::Terminal;
        term = Terminal(pid, psymbol);
        nonterm = NonTerminal();
    }
    ProductionElement(int pid, const std::string& psymbol, const std::vector<std::string>& pvalue)
    {
        type = ProductionElementType::Terminal;
        term = Terminal(pid, psymbol, pvalue);
        nonterm = NonTerminal();
    }
    explicit ProductionElement(const NonTerminal& pnonterm)
    {
        type = ProductionElementType::NonTerminal;
        term = Terminal();
        nonterm = pnonterm;
    }
};

//*********************************
//*  Semantic element definition  *
//********************************/

enum class SemanticElementType
{
    NonTerminal, Terminal, String
};

/// Single target element of a semantic rule. It is used for describing how an expression will be synthesized.
struct SemanticElement
{
    SemanticElementType type;
    NonTerminal nonterm;
    Terminal term;
    std::string string;

    explicit SemanticElement(const Terminal& pterm)
    {
        type = SemanticElementType::Terminal;
        term = pterm;
        nonterm = NonTerminal();
        string = "";
    }
    explicit SemanticElement(const NonTerminal& pnonterm)
    {
        type = SemanticElementType::NonTerminal;
        term = Terminal();
        nonterm = pnonterm;
        string = "";
    }
    explicit SemanticElement(const std::string& pstring)
    {
        type = SemanticElementType::String;
        term = Terminal();
        nonterm = NonTerminal();
        string = pstring;
    }

    [[nodiscard]]
    std::string ToString() const
    {
        switch (type)
        {
            case SemanticElementType::NonTerminal:
                return nonterm.label;
            case SemanticElementType::Terminal:
                return term.label;
            case SemanticElementType::String:
            default:
                return string;
        }
    }
};

//*********************************
//*   Production rule definition  *
//********************************/

/// A rule that describes how to build the next sub-nodes from a given Non-Term. It can also contain
/// a set of semantic rules for synthesizing an expression from the rule and a semantic action, which is
/// a function that evaluates a node using its children values.
struct ProductionRule
{
    NonTerminal from;
    std::vector<ProductionElement> to;
    std::vector<SemanticElement> semanticRules;
    std::function<void(EvaluationContext*)> semanticAction;

    ProductionRule()
    {
        from = NonTerminal();
        semanticAction = nullptr;
    }
    ProductionRule(
        const NonTerminal& pfrom,
        const std::vector<ProductionElement>& pto,
        const std::vector<SemanticElement>& psemrules,
        std::function<void(EvaluationContext*)> pSemanticAction = nullptr)
    {
        from = pfrom;
        to = pto;
        semanticRules = psemrules;
        semanticAction = std::move(pSemanticAction);
    }

    [[nodiscard]] int NumberOfRules() const
    {
        return static_cast<int>(to.size());
    }

    [[nodiscard]]
    std::string ToString() const
    {
        std::string output = from.label;
        output += " -> ";

        int index = 0;
        for (const auto& prodElement: semanticRules)
        {
            output += prodElement.ToString() + ((index == semanticRules.size() - 1) ? "" : " ");
            index++;
        }
        return output;
    }
};


//*********************************
//*   Formal grammar container    *
//********************************/

/// Defines a formal grammar that contains the rules needed to build, synthesize and evaluate syntax trees.
class Grammar
{
private:
    std::vector<ProductionRule> grammarRules;

public:
    Grammar() = default;
    Grammar(std::initializer_list<ProductionRule> productionRuleList) : grammarRules(productionRuleList) {}
    Grammar(const Grammar& other) : grammarRules(other.grammarRules) {}
    explicit Grammar(std::vector<ProductionRule>  productionRuleList) : grammarRules(std::move(productionRuleList)) {}

    /// Get the root rule of the grammar.
    /// \return The root rule of the grammar.
    [[nodiscard]]
    ProductionRule GetRootRule() const
    {
        return grammarRules.front();
    }

    /// Finds all the rules compatible with the specified Non-Terminal type.
    /// \param fromNonTermType The type of the Non-Terminal to find an appropriate rule.
    /// \return A vector containing the compatible rules.
    [[nodiscard]]
    std::vector<ProductionRule> GetCompatibleRules(int fromNonTermType) const
    {
        std::vector<ProductionRule> compatibleRules;
        for (const ProductionRule& rule : grammarRules)
        {
            if (rule.from.id == fromNonTermType)
                compatibleRules.push_back(rule);
        }

        return compatibleRules;
    }

    /// Gets a random rule that is compatible with the specified Non-Terminal type.
    /// \param fromNonTermType The type of the Non-Terminal to find an appropriate rule.
    /// \return The selected random rule.
    [[nodiscard]]
    ProductionRule GetRandomCompatibleRule(int fromNonTermType) const
    {
        std::vector<ProductionRule> compatibleRules = this->GetCompatibleRules(fromNonTermType);
        return *random_choice(compatibleRules.begin(), compatibleRules.end());
    }

    /// Returns the number of production rules of this grammar.
    [[nodiscard]]
    unsigned Size() const
    {
        return static_cast<unsigned>(grammarRules.size());
    }
};