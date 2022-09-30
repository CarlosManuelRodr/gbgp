#pragma once
#include "term.h"
#include "evaluation.h"

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

    [[nodiscard]]
    std::string GetTypeStr() const
    {
        switch (type)
        {
            case ProductionElementType::NonTerminal:
                return "NonTerminal";
            case ProductionElementType::Terminal:
                return "Terminal";
            case ProductionElementType::Unassigned:
            default:
                return "Unassigned";
        }
    }

    [[nodiscard]]
    std::string GetValue() const
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
    std::string GetTypeStr() const
    {
        switch (type)
        {
            case SemanticElementType::NonTerminal:
                return "NonTerminal";
            case SemanticElementType::Terminal:
                return "Terminal";
            case SemanticElementType::String:
            default:
                return "String";
        }
    }

    [[nodiscard]]
    std::string GetValue() const
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
    std::function<void(EvaluationContext&)> semanticAction {};

    ProductionRule()
    {
        from = NonTerminal();
        semanticAction = nullptr;
    }
    ProductionRule(
            const NonTerminal& pfrom,
            const std::vector<ProductionElement>& pto,
            const std::vector<SemanticElement>& psemrules,
            std::function<void(EvaluationContext&)> pSemanticAction = nullptr)
    {
        from = pfrom;
        to = pto;
        semanticRules = psemrules;
        semanticAction = std::move(pSemanticAction);
    }

    [[nodiscard]]
    int NumberOfRules() const
    {
        return static_cast<int>(to.size());
    }

    [[nodiscard]]
    NonTerminal GetFrom() const
    {
        return from;
    }

    [[nodiscard]]
    std::vector<ProductionElement> GetTo() const
    {
        return to;
    }

    [[nodiscard]]
    std::vector<SemanticElement> GetSemanticRules() const
    {
        return semanticRules;
    }

    [[nodiscard]]
    std::function<void(EvaluationContext&)> GetSemanticAction() const
    {
        return semanticAction;
    }

    [[nodiscard]]
    std::string ToString() const
    {
        std::string output = from.label;
        output += " -> ";

        int index = 0;
        for (const auto& prodElement: semanticRules)
        {
            output += prodElement.GetValue() + ((index == semanticRules.size() - 1) ? "" : " ");
            index++;
        }
        return output;
    }
};