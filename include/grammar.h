#pragma once
#include <utility>
#include "vector_ops.h"
#include "production_rule.h"

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