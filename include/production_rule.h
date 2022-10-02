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

    /// Terminal element constructor.
    /// \param pterm The Terminal element.
    explicit ProductionElement(const Terminal& pterm)
    {
        type = ProductionElementType::Terminal;
        term = pterm;
        nonterm = NonTerminal();
    }

    /// Terminal constructor with unique values.
    /// \param value The value.
    explicit ProductionElement(const std::string& value)
    {
        type = ProductionElementType::Terminal;
        term = Terminal(value);
        nonterm = NonTerminal();
    }

    /// Terminal constructor with various possible values.
    /// \param id The Terminal id.
    /// \param label The label of the Terminal.
    /// \param values List of possible values for this Terminal.
    ProductionElement(int id, const std::string& label, const std::vector<std::string>& values)
    {
        type = ProductionElementType::Terminal;
        term = Terminal(id, label, values);
        nonterm = NonTerminal();
    }

    /// NonTerminal element constructor.
    /// \param pnonterm The NonTerminal element.
    explicit ProductionElement(const NonTerminal& pnonterm)
    {
        type = ProductionElementType::NonTerminal;
        term = Terminal();
        nonterm = pnonterm;
    }

    [[nodiscard]]
    ProductionElementType GetType() const
    {
        return type;
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
//*   Production rule definition  *
//********************************/

/// A rule that describes how to build the next sub-nodes from a given Non-Term. It can also contain
/// a set of semantic rules for synthesizing an expression from the rule and a semantic action, which is
/// a function that evaluates a node using its children values.
struct ProductionRule
{
    NonTerminal from;
    std::vector<ProductionElement> to;
    std::function<void(EvaluationContext&)> semanticAction {};

    /// Empty ProductionRule constructor.
    ProductionRule()
    {
        from = NonTerminal();
        semanticAction = nullptr;
    }

    /// Production rule with custom semantic action.
    /// \param pfrom The From non-terminal
    /// \param pto The To Terms
    /// \param pSemanticAction A function to evaluate this production rule.
    ProductionRule(
            const NonTerminal& pfrom,
            const std::vector<ProductionElement>& pto,
            std::function<void(EvaluationContext&)> pSemanticAction)
    {
        from = pfrom;
        to = pto;
        semanticAction = std::move(pSemanticAction);
    }

    ProductionRule(
            const NonTerminal& pfrom,
            const std::vector<ProductionElement>& pto)
            : ProductionRule(pfrom, pto, 0) {}

    /// Production rule with default semantic action.
    /// \param pfrom The From non-terminal
    /// \param pto The To Terms
    /// \param semanticTransferIndex The index of the ProductionElement whose value will be used on evaluation.
    ProductionRule(
            const NonTerminal& pfrom,
            const std::vector<ProductionElement>& pto,
            int semanticTransferIndex)
    {
        from = pfrom;
        to = pto;
        semanticAction = [semanticTransferIndex](EvaluationContext& ctx) {
            ctx.TransferSemanticValueToResult(semanticTransferIndex);
        };
    }

    [[nodiscard]]
    int NumberOfProductionElements() const
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
        for (const auto& prodElement: to)
        {
            output += prodElement.GetValue() + ((index == to.size() - 1) ? "" : " ");
            index++;
        }
        return output;
    }
};