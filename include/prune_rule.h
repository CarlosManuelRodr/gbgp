#pragma once
#include "syntax_tree.h"

namespace gbgp
{
    /// Contains a rule and logic for simplifying a SyntaxTree.
    class PruneRule
    {
    private:
        std::vector<TreeNode*> _pruneRuleFromTraversal;
        std::vector<TreeNode*> _pruneRuleToTraversal;

        static std::string TraversalToString(std::vector<TreeNode*> traversal)
        {
            std::string out = "[";
            for (auto& node : traversal)
            {
                size_t i = &node - &traversal[0];
                out += node->ToString();
                if (i != traversal.size() - 1)
                    out += ", ";
            }
            out += "]";
            return out;
        }

    public:
        /// Constructor by SyntaxTree.
        /// \param pruneRuleFrom Prune rule from as SyntaxTree.
        /// \param pruneRuleTo Prune rule to as SyntaxTree.
        PruneRule(const SyntaxTree& pruneRuleFrom, const SyntaxTree& pruneRuleTo)
        {
            _pruneRuleFromTraversal = pruneRuleFrom.GetPostOrderTreeTraversal();
            _pruneRuleToTraversal = pruneRuleTo.GetPostOrderTreeTraversal();
        }

        /// Does the target tree can be simplified further with this rule?
        bool CanBeApplied(const SyntaxTree& target)
        {
            std::vector<TreeNode*> treeTraversal = target.GetPostOrderTreeTraversal();
            unsigned index = SyntaxTree::FindIndexOfTraversalSubsequence(treeTraversal, _pruneRuleFromTraversal);
            return index != treeTraversal.size();
        }

        /// Create a new SyntaxTree where the prune rule has been applied.
        /// \param target The SyntaxTree where the prune rule will be applied.
        /// \return The pruned SyntaxTree.
        void Apply(SyntaxTree& target)
        {
            std::vector<TreeNode*> treeTraversal = target.GetPostOrderTreeTraversal();
            std::vector<TreeNode*> replacedTraversal = SyntaxTree::ReplaceTraversalSubsequence(treeTraversal,
                                                                                               _pruneRuleFromTraversal,
                                                                                               _pruneRuleToTraversal);
            SyntaxTree::BuildFromTraversal(target, replacedTraversal);
        }

        /// Get string representation.
        [[nodiscard]]
        std::string ToString() const
        {
            return "PruneRule(from=" + TraversalToString(_pruneRuleFromTraversal) + ", to="
                   + TraversalToString(_pruneRuleToTraversal) + ")";
        }
    };
}