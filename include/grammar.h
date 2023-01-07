#pragma once
#include <utility>
#include "prune_rule.h"

namespace gbgp
{
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
        /// Empty constructor.
        Grammar() = default;

        /// Copy constructor.
        Grammar(const Grammar& other) = default;

        /// Only production rules initializer list constructor.
        /// \param productionRuleList
        Grammar(std::initializer_list<ProductionRule> productionRuleList) : _grammarRules(productionRuleList) {}

        /// Initializer list constructor.
        /// \param productionRuleList
        /// \param pruneRuleList
        Grammar(std::initializer_list<ProductionRule> productionRuleList,
                std::initializer_list<PruneRule> pruneRuleList)
                : _grammarRules(productionRuleList), _pruneRules(pruneRuleList) {}

        /// Only production rules constructor.
        /// \param productionRuleList
        explicit Grammar(const std::vector<ProductionRule>& productionRuleList)
        {
            _grammarRules = productionRuleList;
        }

        /// Parameter by parameter constructor.
        /// \param productionRuleList
        /// \param pruneRuleList
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

        /// Get the number of production rules of this grammar.
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
            // TODO: Enforce a terminal node selection when limit is reached.
            if (node->type == NodeType::NonTerminal)
            {
                // Create children nodes based on the current node production rule.
                std::vector<TreeNode*> newNodes;
                for (const ProductionElement& pe : node->generatorPR.to)
                {
                    if (pe.type == ProductionElementType::NonTerminal)
                        newNodes.push_back(node->AddChildTerm(pe.nonterm, GetRandomCompatibleRule(pe.nonterm.id)));
                    if (pe.type == ProductionElementType::Terminal)
                        newNodes.push_back(node->AddChildTerm(pe.term));
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
            syntaxTree.SetRootRule(rootRule.has_value() ? rootRule.value() : GetRootRule());

            // Create children nodes based on the selected rule.
            std::vector<TreeNode*> newNodes;
            for (const ProductionElement& pe : syntaxTree.Root()->generatorPR.to)
            {
                if (pe.type == ProductionElementType::NonTerminal)
                    newNodes.push_back(syntaxTree.Root()->AddChildTerm(pe.nonterm, GetRandomCompatibleRule(pe.nonterm.id)));
                if (pe.type == ProductionElementType::Terminal)
                    newNodes.push_back(syntaxTree.Root()->AddChildTerm(pe.term));
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

        /// Create random tree safely by creating random trees until there is a success.
        /// \param syntaxTree The target tree.
        void CreateRandomTree(SyntaxTree& syntaxTree) const
        {
            CreateRandomTree(syntaxTree, 50);
        }

        /// Create random tree safely by creating random trees until there is a success.
        /// \param syntaxTree The target tree.
        /// \param maxDepth Maximum allowed tree depth.
        void CreateRandomTree(SyntaxTree& syntaxTree, int maxDepth) const
        {
            CreateRandomTree(syntaxTree, maxDepth, std::nullopt);
        }

        /// Create random tree safely by creating random trees until there is a success.
        /// \param syntaxTree The target tree.
        /// \param maxDepth Maximum allowed tree depth.
        /// \param rootRule The root rule of the grammar.
        void CreateRandomTree(SyntaxTree& syntaxTree, int maxDepth, const std::optional<ProductionRule>& rootRule) const
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

        [[nodiscard]]
        std::string ToString() const
        {
            return "rules='" + std::to_string(_grammarRules.size()) + "'";
        }

        /// Since functions cannot be serialized-deserialized, this utility restore the correct semanticAction to the
        /// target production rule.
        /// \param target The target production rule.
        /// \return True if a match was found and the semantic action was restored. False otherwise.
        bool RestoreSemanticAction(ProductionRule& target) const
        {
            for (const ProductionRule& rule : _grammarRules)
            {
                if (rule.SameRule(target))
                {
                    target.semanticAction = rule.semanticAction;
                    return true;
                }
            }

            return false;
        }

        /// Restores the unserializable function pointer of the ProductionRule of a node. Always call this method
        /// when deserializing a node with a production rule that contained a semantic action.
        /// \param target The target node.
        /// \return True if a match was found and the semantic action was restored. False otherwise.
        bool RestoreSemanticAction(Node& target) const
        {
            return RestoreSemanticAction(target.generatorPR);
        }

        /// Restores the unserializable function pointer of the ProductionRule of nodes inside a graph.
        /// Always call this method when deserializing a Graph that contains a node with a production rule with
        /// a semantic action.
        /// \param target The target graph.
        /// \return True if a match was found on any node and the semantic action was restored. False otherwise.
        bool RestoreSemanticAction(Graph& target) const
        {
            bool result = false;
            for (auto &node: target.GetNodes())
                result |= RestoreSemanticAction(node);

            return result;
        }
    };
}