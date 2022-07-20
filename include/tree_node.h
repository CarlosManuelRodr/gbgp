#pragma once
#include <string>
#include "production_rule.h"

//*****************************
//*    Node implementation    *
//****************************/

/// A node can be either a Terminal or a NonTerminal. A None type is provided to instantiate empty _tree nodes.
enum class TreeNodeType
{
    None, NonTerminal, Terminal
};

/// Represents a node of an n-ary _tree
class TreeNode
{
public:
    /// Type of the node.
    TreeNodeType type;

    /// NonTerminal instance used if the node is of NonTerminal type.
    NonTerminal nonTermInstance;

    /// Terminal instance used if the node is of Terminal type.
    Terminal termInstance;

    /// Production rule from which this node is part of.
    ProductionRule generatorPR;

    /// Parent of the node. If the node is a _root, its value will be null.
    TreeNode* parent;

    /// List of references to the children of this node.
    std::vector<TreeNode*> children;

    /// Value of the terminal used if the node is of Terminal type.
    std::string termValue;

    /// Result of the synthesis of this node.
    std::string expressionSynthesis;

    /// Result of the evaluation of this node.
    std::string expressionEvaluation;

    /// Constructor of an empty node.
    TreeNode()
    {
        type = TreeNodeType::None;
        termInstance = Terminal();
        nonTermInstance = NonTerminal();
        expressionSynthesis = "";
        expressionEvaluation = "";
        parent = nullptr;
    }

    /// Constructor of a NonTerminal node.
    explicit TreeNode(const NonTerminal& nt)
    {
        type = TreeNodeType::NonTerminal;
        nonTermInstance = nt;
        termInstance = Terminal();
        expressionSynthesis = "";
        expressionEvaluation = "";
        parent = nullptr;
    }

    TreeNode(const NonTerminal& nt, const std::vector<TreeNode>& children)
    {
        type = TreeNodeType::NonTerminal;
        nonTermInstance = nt;
        termInstance = Terminal();
        expressionSynthesis = "";
        expressionEvaluation = "";
        parent = nullptr;

        for (const auto& c : children)
            AddChildNode(new TreeNode(c), this);
    }

    TreeNode(ProductionRule productionRule, const NonTerminal& nt)
    {
        type = TreeNodeType::NonTerminal;
        nonTermInstance = nt;
        termInstance = Terminal();
        expressionSynthesis = "";
        termValue = "";
        expressionEvaluation = "";
        parent = nullptr;
        generatorPR = std::move(productionRule);
    }

    TreeNode(ProductionRule productionRule, const NonTerminal& nt, const std::vector<TreeNode>& children)
    {
        type = TreeNodeType::NonTerminal;
        nonTermInstance = nt;
        termInstance = Terminal();
        expressionSynthesis = "";
        termValue = "";
        expressionEvaluation = "";
        parent = nullptr;
        generatorPR = std::move(productionRule);

        for (const auto& c : children)
            AddChildNode(new TreeNode(c), this);
    }

    /// Constructor of a Terminal node.
    explicit TreeNode(const Terminal& t)
    {
        type = TreeNodeType::Terminal;
        nonTermInstance = NonTerminal();
        termInstance = t;
        expressionSynthesis = "";
        expressionEvaluation = "";
        parent = nullptr;
    }

    TreeNode(const Terminal& t, const std::string& value)
    {
        type = TreeNodeType::Terminal;
        nonTermInstance = NonTerminal();
        termInstance = t;
        expressionSynthesis = "";
        termValue = value;
        expressionEvaluation = "";
        parent = nullptr;
    }

    /// Copy constructor.
    TreeNode(const TreeNode& other)
    {
        type = other.type;
        nonTermInstance = other.nonTermInstance;
        termInstance = other.termInstance;
        generatorPR = other.generatorPR;
        termValue = other.termValue;
        parent = other.parent;
        children = other.children;
    }

    /// Copy constructor by reference.
    explicit TreeNode(TreeNode* other)
    {
        type = other->type;
        nonTermInstance = other->nonTermInstance;
        termInstance = other->termInstance;
        generatorPR = other->generatorPR;
        termValue = other->termValue;
        parent = other->parent;
    }

    /// Performs a copy of the node term without children.
    static TreeNode* ShallowCopy(TreeNode* other)
    {
        auto* copyNode = new TreeNode();
        copyNode->type = other->type;
        copyNode->nonTermInstance = other->nonTermInstance;
        copyNode->termInstance = other->termInstance;
        copyNode->generatorPR = other->generatorPR;
        copyNode->termValue = other->termValue;
        return copyNode;
    }

    ~TreeNode()
    {
        parent = nullptr;
        children.clear();
    }

    bool operator==(const TreeNode& other) const
    {
        const bool sameType = this->type == other.type;
        const bool sameTerm = this->termInstance == other.termInstance;
        const bool sameNonTerm = this->nonTermInstance == other.nonTermInstance;
        const bool sameValue = this->termValue == other.termValue;
        return sameType && sameTerm && sameNonTerm && sameValue;
    }
    bool operator!=(const TreeNode& other) const
    {
        return !(*this == other);
    }

    /// Check if both nodes have the same term ID.
    /// \param other The other node.
    /// \return Whether nodes have the same ID.
    [[nodiscard]]
    bool SameID(TreeNode* other) const
    {
        return SameID(*other);
    }

    /// Check if both nodes have the same term ID.
    /// \param other The other node.
    /// \return Whether nodes have the same ID.
    [[nodiscard]]
    bool SameID(const TreeNode& other) const
    {
        bool sameType = this->type == other.type;
        if (!sameType)
            return false;

        if (type == TreeNodeType::NonTerminal)
            return this->nonTermInstance == other.nonTermInstance;
        else if (type == TreeNodeType::Terminal)
            return this->termInstance == other.termInstance;
        else
            return false;
    }

    /// Reset the synthesis of this node
    void ClearSynthesis()
    {
        expressionSynthesis.clear();
    }

    /// Check if this node has been synthesized.
    [[nodiscard]]
    bool IsSynthesized() const
    {
        return !expressionSynthesis.empty();
    }

    /// Reset the evaluation of this node
    void ClearEvaluation()
    {
        expressionEvaluation.clear();
    }

    /// Check if this node has been evaluated.
    [[nodiscard]]
    bool IsEvaluated() const
    {
        return !expressionEvaluation.empty();
    }

    /// Check if this node has children.
    [[nodiscard]]
    bool HasChildren() const
    {
        return !children.empty();
    }

    /// Add the node as a child.
    /// \param node Reference to the child node.
    void AddChildNode(TreeNode* node, TreeNode* nodeParent = nullptr)
    {
        if (nodeParent != nullptr)
            node->parent = nodeParent;
        children.push_back(node);
    }

    /// Returns the value of the node.
    [[nodiscard]]
    std::string GetValue() const
    {
        return termValue;
    }

    /// Returns a formatted label of the node.
    [[nodiscard]]
    std::string GetLabel() const
    {
        return (type == TreeNodeType::NonTerminal) ?
               nonTermInstance.label :
               termInstance.label + " [" + termValue + "]";
    }

    /// Get node representation as string.
    [[nodiscard]]
    std::string ToString() const
    {
        std::string output = GetLabel();
        output += " -> ";
        for (auto child : children)
            output += child->GetLabel() + + ((child == children.back()) ? "" : ", ");
        return output;
    }
};