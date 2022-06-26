#pragma once
#include <utility>
#include "evaluation.h"
#include "vector_ops.h"

//*****************************
//*         Terminal          *
//****************************/

/// Terminal term.
/// \tparam TerminalType enum class with the Terminal IDs specified by the user.
template <typename TerminalType> struct Terminal
{
    TerminalType id;
    std::string label;
    std::vector<std::string> values;

    Terminal()
    {
        label = '\0';
    }
    Terminal(TerminalType pid, const std::string& plabel)
    {
        id = pid;
        label = plabel;
        values.push_back(plabel);
    }
    Terminal(TerminalType pid, const std::string& plabel, const std::vector<std::string>& pvalues)
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
};

//*****************************
//*        NonTerminal        *
//****************************/

/// Non-Terminal term.
/// \tparam NonTerminalType enum class with the NonTerminal IDs specified by the user.
template <typename NonTerminalType> struct NonTerminal
{
    NonTerminalType id;
    std::string label;

    NonTerminal()
    {
        label = '\0';
    }
    NonTerminal(NonTerminalType pid, const std::string& plabel)
    {
        id = pid;
        label = plabel;
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

//*********************************
//* Production element definition *
//********************************/

enum class ProductionElementType
{
    Unassigned, NonTerminal, Terminal
};

/// Single target element of a production rule. Used as a container for building a ProductionRule.
/// May contain a Terminal or a Non-Terminal.
/// \tparam TerminalType enum class with the Terminal IDs specified by the user.
/// \tparam NonTerminalType enum class with the NonTerminal IDs specified by the user.
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
    ProductionElement(TerminalType pid, const std::string& psymbol)
    {
        type = ProductionElementType::Terminal;
        term = Terminal<TerminalType>(pid, psymbol);
        nonterm = NonTerminal<NonTerminalType>();
    }
    ProductionElement(TerminalType pid, const std::string& psymbol, const std::vector<std::string>& pvalue)
    {
        type = ProductionElementType::Terminal;
        term = Terminal<TerminalType>(pid, psymbol, pvalue);
        nonterm = NonTerminal<NonTerminalType>();
    }
    explicit ProductionElement(const NonTerminal<NonTerminalType>& pnonterm)
    {
        type = ProductionElementType::NonTerminal;
        term = Terminal<TerminalType>();
        nonterm = pnonterm;
    }
    ProductionElement(NonTerminalType pid, const std::string& psymbol)
    {
        type = ProductionElementType::NonTerminal;
        term = Terminal<TerminalType>();
        nonterm = NonTerminal<NonTerminalType>(pid, psymbol);
    }

    bool operator==(const ProductionElement<TerminalType, NonTerminalType>& other) const
    {
        return type == other.type && nonterm == other.nonterm && term == other.term;
    }
    bool operator!=(const ProductionElement<TerminalType, NonTerminalType>& other) const
    {
        return type != other.type || nonterm != other.nonterm || term != other.term;
    }

    [[nodiscard]] std::string ToString() const
    {
        switch (type)
        {
            case ProductionElementType::NonTerminal:
                return nonterm.label;
            case ProductionElementType::Terminal:
                return term.label;
            case ProductionElementType::Unassigned:
            default:
                return {};
        }
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
/// \tparam TerminalType enum class with the Terminal IDs specified by the user.
/// \tparam NonTerminalType enum class with the NonTerminal IDs specified by the user.
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

//*********************************
//*   Production rule definition  *
//********************************/

/// A rule that describes how to build the next sub-nodes from a given Non-Term. It can also contain
/// a set of semantic rules for synthesizing an expression from the rule and a semantic action, which is
/// a function that evaluates a node using its children values.
/// \tparam TerminalType enum class with the Terminal IDs specified by the user.
/// \tparam NonTerminalType enum class with the NonTerminal IDs specified by the user.
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
        semanticAction = std::move(pSemanticAction);
    }

    [[nodiscard]] int NumberOfRules() const
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

    [[nodiscard]] std::string ToString() const
    {
        std::string output = from.label;
        output += " -> ";
        for (auto prodElement: to)
            output += prodElement.ToString() + ((prodElement == to.back()) ? "" : " ");
        return output;
    }
};


//*********************************
//*   Formal grammar container    *
//********************************/

/// Defines a formal grammar that contains the rules needed to build, synthesize and evaluate syntax trees.
/// \tparam TerminalType enum class with the Terminal IDs specified by the user.
/// \tparam NonTerminalType enum class with the NonTerminal IDs specified by the user.
template <typename TerminalType, typename NonTerminalType> class Grammar
{
private:
    std::vector<ProductionRule<TerminalType, NonTerminalType>> grammarRules;

public:
    Grammar() = default;
    Grammar(std::initializer_list<ProductionRule<TerminalType, NonTerminalType>> productionRuleList) : grammarRules(productionRuleList) {}
    Grammar(const Grammar<TerminalType, NonTerminalType>& other) : grammarRules(other.grammarRules) {}

    explicit Grammar(const std::vector<ProductionRule<TerminalType, NonTerminalType>>& productionRuleList) : grammarRules(productionRuleList) {}

    /// Get the root Non-Terminal of the grammar.
    /// \return The root Non-Terminal of the grammar.
    [[nodiscard]]
    NonTerminal<NonTerminalType> GetRoot() const
    {
        return grammarRules.front().from;
    }

    /// Finds the index of a production rule contained in the grammar.
    /// \param pr The production rule.
    /// \return The index of a rule inside the current grammar.
    [[nodiscard]]
    unsigned IndexOfRule(const ProductionRule<TerminalType, NonTerminalType>& pr) const
    {
        return find_index_of(grammarRules,pr);
    }

    /// Finds all the rules compatible with the specified Non-Terminal type.
    /// \param fromNonTermType The type of the Non-Terminal to find an appropriate rule.
    /// \return A vector containing the compatible rules.
    [[nodiscard]]
    std::vector<ProductionRule<TerminalType, NonTerminalType>> GetCompatibleRules(NonTerminalType fromNonTermType) const
    {
        std::vector<ProductionRule<TerminalType, NonTerminalType>> compatibleRules;
        for (ProductionRule<TerminalType, NonTerminalType> rule : grammarRules)
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
    ProductionRule<TerminalType, NonTerminalType> GetRandomCompatibleRule(NonTerminalType fromNonTermType) const
    {
        std::vector<ProductionRule<TerminalType, NonTerminalType>> compatibleRules = this->GetCompatibleRules(fromNonTermType);
        return *select_randomly(compatibleRules.begin(), compatibleRules.end());
    }

    /// Returns a random root rule.
    [[nodiscard]]
    ProductionRule<TerminalType, NonTerminalType> GetRandomRootRule() const
    {
        return GetRandomCompatibleRule(this->GetRoot().id);
    }

    /// Returns the number of production rules of this grammar.
    [[nodiscard]]
    unsigned Size() const
    {
        return static_cast<unsigned>(grammarRules.size());
    }

    ProductionRule<TerminalType, NonTerminalType>& operator[](int index) const
    {
        return grammarRules[index];
    }

    Grammar<TerminalType, NonTerminalType>& operator=(const Grammar<TerminalType, NonTerminalType>& other)
    {
        grammarRules = other.grammarRules;
        return *this;
    }
};