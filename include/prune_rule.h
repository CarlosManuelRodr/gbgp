#pragma once
#include "syntax_tree.h"

/// Contains a rule and logic for simplifying a SyntaxTree.
class PruneRule
{
private:
    std::vector<TreeNode*> pruneRuleFromTraversal;
    std::vector<TreeNode*> pruneRuleToTraversal;

public:
    /// Constructor by SyntaxTree.
    /// \param pruneRuleFrom Prune rule from as SyntaxTree.
    /// \param pruneRuleTo Prune rule to as SyntaxTree.
    PruneRule(const SyntaxTree& pruneRuleFrom, const SyntaxTree& pruneRuleTo)
    {
        pruneRuleFromTraversal = pruneRuleFrom.GetTreeTraversal();
        pruneRuleToTraversal = pruneRuleTo.GetTreeTraversal();
    }

    /// Create a new SyntaxTree where the prune rule has been applied.
    /// \param target The SyntaxTree where the prune rule will be applied.
    /// \return The pruned SyntaxTree.
    SyntaxTree Apply(const SyntaxTree& target)
    {
        std::vector<TreeNode*> treeTraversal = target.GetTreeTraversal();
        std::vector<TreeNode*> replacedTraversal = SyntaxTree::ReplaceTraversalSubsequence(treeTraversal,
                                                                                           pruneRuleFromTraversal,
                                                                                           pruneRuleToTraversal);
        SyntaxTree reconstruction = SyntaxTree::BuildFromTraversal(replacedTraversal);
        return reconstruction;
    }
};