#pragma once
#include "syntax_tree.h"

/// Contains a rule and logic for simplifying a SyntaxTree.
class PruneRule
{
private:
    std::vector<TreeNode*> _pruneRuleFromTraversal;
    std::vector<TreeNode*> _pruneRuleToTraversal;

public:
    /// Constructor by SyntaxTree.
    /// \param pruneRuleFrom Prune rule from as SyntaxTree.
    /// \param pruneRuleTo Prune rule to as SyntaxTree.
    PruneRule(const SyntaxTree& pruneRuleFrom, const SyntaxTree& pruneRuleTo)
    {
        _pruneRuleFromTraversal = pruneRuleFrom.GetTreeTraversal();
        _pruneRuleToTraversal = pruneRuleTo.GetTreeTraversal();
    }

    bool CanBeApplied(const SyntaxTree& target)
    {
        std::vector<TreeNode*> treeTraversal = target.GetTreeTraversal();
        unsigned index = SyntaxTree::FindIndexOfTraversalSubsequence(treeTraversal, _pruneRuleFromTraversal);
        return index != treeTraversal.size();
    }

    /// Create a new SyntaxTree where the prune rule has been applied.
    /// \param target The SyntaxTree where the prune rule will be applied.
    /// \return The pruned SyntaxTree.
    void Apply(SyntaxTree& target)
    {
        std::vector<TreeNode*> treeTraversal = target.GetTreeTraversal();
        std::vector<TreeNode*> replacedTraversal = SyntaxTree::ReplaceTraversalSubsequence(treeTraversal,
                                                                                           _pruneRuleFromTraversal,
                                                                                           _pruneRuleToTraversal);
        SyntaxTree::BuildFromTraversal(target, replacedTraversal);
    }
};