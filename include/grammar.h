#pragma once
#include <utility>
#include "prune_rule.h"

/// Defines a formal grammar that contains the rules needed to build, synthesize and evaluate syntax trees.
class Grammar
{
private:
    std::vector<ProductionRule> _grammarRules;
    std::vector<PruneRule> _pruneRules;

    /// Applies sequentially all the prune rules of the grammar.
    /// \param syntaxTree The target syntax tree that will be pruned.
    /// \return True if a prune rule could be applied, false otherwise.
    bool ApplyPruneRules(SyntaxTree& syntaxTree) const
    {
        bool wasPruned = false;
        for (auto pruneRule : _pruneRules)
        {
            if (pruneRule.CanBeApplied(syntaxTree))
            {
                pruneRule.Apply(syntaxTree);
                wasPruned = true;
            }
        }
        return wasPruned;
    }

    /// Finds all the rules compatible with the specified Non-Terminal type.
    /// \param fromNonTermType The type of the Non-Terminal to find an appropriate rule.
    /// \return A vector containing the compatible rules.
    [[nodiscard]]
    std::vector<ProductionRule> GetCompatibleRules(int fromNonTermType) const
    {
        std::vector<ProductionRule> compatibleRules;
        for (const ProductionRule& rule : _grammarRules)
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

public:
    Grammar() = default;
    Grammar(const Grammar& other) = default;

    Grammar(std::initializer_list<ProductionRule> productionRuleList) : _grammarRules(productionRuleList) {}

    Grammar(std::initializer_list<ProductionRule> productionRuleList,
            std::initializer_list<PruneRule> pruneRuleList)
            : _grammarRules(productionRuleList), _pruneRules(pruneRuleList) {}

    explicit Grammar(const std::vector<ProductionRule>& productionRuleList)
    {
        _grammarRules = productionRuleList;
    }
    Grammar(const std::vector<ProductionRule>& productionRuleList, const std::vector<PruneRule>& pruneRuleList)
    {
        _grammarRules = productionRuleList;
        _pruneRules = pruneRuleList;
    }

    /// Get the root rule of the grammar.
    /// \return The root rule of the grammar.
    [[nodiscard]]
    ProductionRule GetRootRule() const
    {
        return _grammarRules.front();
    }

    /// Returns the number of production rules of this grammar.
    [[nodiscard]]
    unsigned Size() const
    {
        return static_cast<unsigned>(_grammarRules.size());
    }

    //*******************************
    //*   Random tree generation    *
    //******************************/

    /// Recursive implementation. Create random tree based on the production rules described in _grammarRules.
    /// \param maxDepth Maximum allowed tree depth.
    /// \param depth Current depth. If while creating a random tree, the depth reaches the maxDepth value, it will fail and return false.
    /// \param node Node from where the random tree will be created.
    /// \return True if creation is successful, false if not.
    bool TryCreateRandomTree(int maxDepth, int depth, TreeNode* node) const
    {
        if (node->type == TreeNodeType::NonTerminal)
        {
            // Create children nodes based on the current node production rule.
            std::vector<TreeNode*> newNodes;
            for (const ProductionElement& pe : node->generatorPR.to)
            {
                if (pe.type == ProductionElementType::NonTerminal)
                    newNodes.push_back(node->AddChildNode(pe.nonterm, GetRandomCompatibleRule(pe.nonterm.id)));
                if (pe.type == ProductionElementType::Terminal)
                    newNodes.push_back(node->AddChildNode(pe.term));
                if (pe.type == ProductionElementType::Unassigned)
                    throw std::runtime_error("Unassigned production element type");
            }

            // Create subtrees for children nodes.
            if (depth != maxDepth)
            {
                for (TreeNode* n : newNodes)
                {
                    bool branchCreationSuccess = TryCreateRandomTree(maxDepth, depth + 1, n);
                    if (!branchCreationSuccess)
                        return false;
                }
            }
            else
                return false;
        }
        else
            node->termValue = node->termInstance.GetRandomValue();

        return true;
    }

    /// Create random tree based on the production rules described in the variable _grammarRules.
    /// \param maxDepth Maximum allowed tree depth.
    /// \return True if creation is successful, false if not.
    bool TryCreateRandomTree(SyntaxTree& syntaxTree, int maxDepth, const std::optional<ProductionRule>& rootRule) const
    {
        syntaxTree.SetRootRule(rootRule != std::nullopt ? rootRule.value() : GetRootRule());

        // Create children nodes based on the selected rule.
        std::vector<TreeNode*> newNodes;
        for (const ProductionElement& pe : syntaxTree.Root()->generatorPR.to)
        {
            if (pe.type == ProductionElementType::NonTerminal)
                newNodes.push_back(syntaxTree.Root()->AddChildNode(pe.nonterm, GetRandomCompatibleRule(pe.nonterm.id)));
            if (pe.type == ProductionElementType::Terminal)
                newNodes.push_back(syntaxTree.Root()->AddChildNode(pe.term));
            if (pe.type == ProductionElementType::Unassigned)
                throw std::runtime_error("Unassigned production element type");
        }

        for (TreeNode* n : newNodes)
        {
            bool branchCreationSuccess = TryCreateRandomTree(maxDepth, 1, n);
            if (!branchCreationSuccess)
                return false;
        }

        return true;
    }

    /// Ensure the creation of a random tree by creating random trees until there is a success.
    /// \param maxDepth Maximum allowed tree depth.
    void CreateRandomTree(SyntaxTree& syntaxTree, int maxDepth = 50, const std::optional<ProductionRule>& rootRule = std::nullopt) const
    {
        bool success = false;
        while (!success)
        {
            syntaxTree.Destroy();
            success = this->TryCreateRandomTree(syntaxTree, maxDepth, rootRule);
        }
    }

    /// Applies the grammar prune rules repeatedly until no further simplification can be performed.
    /// \param syntaxTree The target syntax tree that will be pruned.
    void PruneTree(SyntaxTree& syntaxTree) const
    {
        bool canBePruned = false;
        do
            canBePruned = ApplyPruneRules(syntaxTree);
        while (canBePruned);
    }
};