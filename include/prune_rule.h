#pragma once
#include "syntax_tree.h"

class PruneRule
{
private:
    std::vector<TreeNode*> pruneRuleFromTraversal;
    std::vector<TreeNode*> pruneRuleToTraversal;

public:
    PruneRule(const SyntaxTree& pruneRuleFrom, const SyntaxTree& pruneRuleTo)
    {
        pruneRuleFromTraversal = pruneRuleFrom.DepthFirstScanPostorder();
        pruneRuleToTraversal = pruneRuleTo.DepthFirstScanPostorder();
    }

    SyntaxTree Apply(const SyntaxTree& target)
    {
        std::vector<TreeNode*> treeTraversal = target.DepthFirstScanPostorder();
        std::vector<TreeNode*> replacedTraversal = SyntaxTree::ReplaceTraversalSubsequence(treeTraversal,
                                                                                           pruneRuleFromTraversal,
                                                                                           pruneRuleToTraversal);
        SyntaxTree reconstruction = SyntaxTree::BuildFromTraversal(replacedTraversal);
        return reconstruction;
    }
};