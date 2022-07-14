#pragma once
#include <utility>
#include "prune_rule.h"

//*********************************
//*   Formal grammar container    *
//********************************/

/// Defines a formal grammar that contains the rules needed to build, synthesize and evaluate syntax trees.
class Grammar
{
private:
    std::vector<ProductionRule> grammarRules;
    std::vector<PruneRule> pruneRules;

public:
    Grammar() = default;

    Grammar(std::initializer_list<ProductionRule> productionRuleList) : grammarRules(productionRuleList) {}

    Grammar(std::initializer_list<ProductionRule> productionRuleList,
            std::initializer_list<PruneRule> pruneRuleList)
            : grammarRules(productionRuleList), pruneRules(pruneRuleList) {}

    Grammar(const Grammar& other) : grammarRules(other.grammarRules), pruneRules(other.pruneRules) {}

    explicit Grammar(const std::vector<ProductionRule>& productionRuleList)
    {
        grammarRules = productionRuleList;
    }
    Grammar(const std::vector<ProductionRule>& productionRuleList, const std::vector<PruneRule>& pruneRuleList)
    {
        grammarRules = productionRuleList;
        pruneRules = pruneRuleList;
    }

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

    /// Returns the number of production rules of this grammar.
    [[nodiscard]]
    unsigned Size() const
    {
        return static_cast<unsigned>(grammarRules.size());
    }

    //*******************************
    //*   Random tree generation    *
    //*****************************/

    /// Gets a random rule that is compatible with the specified Non-Terminal type.
    /// \param fromNonTermType The type of the Non-Terminal to find an appropriate rule.
    /// \return The selected random rule.
    [[nodiscard]]
    ProductionRule GetRandomCompatibleRule(int fromNonTermType) const
    {
        std::vector<ProductionRule> compatibleRules = this->GetCompatibleRules(fromNonTermType);
        return *random_choice(compatibleRules.begin(), compatibleRules.end());
    }

    /// Recursive implementation. Create random tree based on the production rules described in grammarRules.
    /// \param maxDepth Maximum allowed tree depth.
    /// \param depth Current depth. If while creating a random tree, the depth reaches the maxDepth value, it will fail and return false.
    /// \param node Node from where the random tree will be created.
    /// \return True if creation is successful, false if not.
    bool TryCreateRandomTree(int maxDepth, int depth, TreeNode* node)
    {
        if (node->type == TreeNodeType::NonTerminal)
        {
            // Create children nodes based on the current node production rule.
            std::vector<TreeNode*> newNodes;
            for (const ProductionElement& pe : node->generatorPR.to)
            {
                if (pe.type == ProductionElementType::NonTerminal)
                    newNodes.push_back(SyntaxTree::AddNode(node, pe.nonterm, GetRandomCompatibleRule(pe.nonterm.id)));
                if (pe.type == ProductionElementType::Terminal)
                    newNodes.push_back(SyntaxTree::AddNode(node, pe.term));
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
            node->termValue = node->termInstance.GetValue();

        return true;
    }

    /// Create random tree based on the production rules described in the variable grammarRules.
    /// \param maxDepth Maximum allowed tree depth.
    /// \return True if creation is successful, false if not.
    bool TryCreateRandomTree(SyntaxTree& syntaxTree, int maxDepth = 10)
    {
        syntaxTree.SetRootRule(GetRootRule());

        // Create children nodes based on the selected rule
        std::vector<TreeNode*> newNodes;
        for (const ProductionElement& pe : syntaxTree.Root()->generatorPR.to)
        {
            if (pe.type == ProductionElementType::NonTerminal)
                newNodes.push_back(SyntaxTree::AddNode(syntaxTree.Root(), pe.nonterm, GetRandomCompatibleRule(pe.nonterm.id)));
            if (pe.type == ProductionElementType::Terminal)
                newNodes.push_back(SyntaxTree::AddNode(syntaxTree.Root(), pe.term));
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
    void CreateRandomTree(SyntaxTree& syntaxTree, int maxDepth = 10)
    {
        bool success = false;
        while (!success)
        {
            syntaxTree.Reset();
            success = this->TryCreateRandomTree(syntaxTree, maxDepth);
        }
    }
};