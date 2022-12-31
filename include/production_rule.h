#pragma once
#include "term.h"
#include "evaluation.h"

//*********************************
//* Production element definition *
//********************************/

/// Enumeration of the type of element that the production element contains.
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

    /// Empty constructor.
    ProductionElement()
    {
        type = ProductionElementType::Unassigned;
        term = Terminal();
        nonterm = NonTerminal();
    }

    /// Terminal element constructor.
    /// \param pterm The Terminal element.
    explicit ProductionElement(const Terminal& pterm)
    {
        type = ProductionElementType::Terminal;
        term = pterm;
        nonterm = NonTerminal();
    }

    /// Terminal constructor with unique value.
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

    /// Set all members constructor.
    /// \param ptype The production element type.
    /// \param pterm The Terminal.
    /// \param pnonterm The NonTerminal.
    ProductionElement(const ProductionElementType& ptype, const Terminal& pterm, const NonTerminal& pnonterm)
    {
        type = ptype;
        term = pterm;
        nonterm = pnonterm;
    }

    /// Get the production element type.
    [[nodiscard]]
    ProductionElementType GetType() const
    {
        return type;
    }

    /// Get production element type as a string.
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

    /// Get the label value of the production element.
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

    /// Get a string representation.
    [[nodiscard]]
    std::string ToString() const
    {
        return "type='" + GetTypeStr() + "', value='" + GetValue();
    }

    bool operator==(const ProductionElement& other) const
    {
        return type == other.type && nonterm == other.nonterm && term == other.term;
    }

    bool operator!=(const ProductionElement& other) const
    {
        return type != other.type || nonterm != other.nonterm || term != other.term;
    }

    /// Serialization hook.
    template<class Archive> void serialize(Archive& ar)
    {
        ar(type, nonterm, term);
    }
};

//*********************************
//*   Production rule definition  *
//********************************/

/// A rule that describes how to build the next sub-nodes from a given Non-Terminal. It can also contain
/// a set of semantic rules for synthesizing an expression and evaluating it using a semantic action, which is
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

    /// Production rule with default semantic action.
    /// \param pfrom The From non-terminal.
    /// \param pto The To Terms.
    ProductionRule(
            const NonTerminal& pfrom,
            const std::vector<ProductionElement>& pto)
            : ProductionRule(pfrom, pto, 0) {}

    /// Production rule with default semantic action.
    /// \param pfrom The From non-terminal.
    /// \param pto The To Terms.
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

    /// Production rule with custom semantic action.
    /// \param pfrom The From non-terminal.
    /// \param pto The To Terms.
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

    /// Returns the number of production elements.
    [[nodiscard]]
    int NumberOfProductionElements() const
    {
        return static_cast<int>(to.size());
    }

    /// Getter for the from list.
    [[nodiscard]]
    NonTerminal GetFrom() const
    {
        return from;
    }

    /// Getter for the to list.
    [[nodiscard]]
    std::vector<ProductionElement> GetTo() const
    {
        return to;
    }

    /// Getter for the semantic action.
    [[nodiscard]]
    std::function<void(EvaluationContext&)> GetSemanticAction() const
    {
        return semanticAction;
    }

    /// Get a string representation.
    [[nodiscard]]
    std::string ToString() const
    {
        std::string output = from.label;
        output += " -> ";

        size_t index = 0;
        for (const auto& prodElement: to)
        {
            output += prodElement.GetValue() + ((index == to.size() - 1) ? "" : " ");
            index++;
        }
        return output;
    }

    [[nodiscard]]
    bool SameRule(const ProductionRule& other) const
    {
        return from == other.from && to == other.to;
    }

    bool operator==(const ProductionRule& other) const
    {
        return SameRule(other);
    }

    bool operator!=(const ProductionRule& other) const
    {
        return !SameRule(other);
    }

    /// Serialization hook.
    template<class Archive> void serialize(Archive& ar)
    {
        ar(from, to);
    }
};