#pragma once
#include <string>
#include "production_rule.h"

/// A node can be either a Terminal or a NonTerminal. A None type is provided to instantiate empty tree nodes.
enum class TreeNodeType
{
    None, NonTerminal, Terminal
};

struct Node
{
    /// Type of the node.
    TreeNodeType type;

    /// NonTerminal instance used if the node is of NonTerminal type.
    NonTerminal nonTermInstance;

    /// Terminal instance used if the node is of Terminal type.
    Terminal termInstance;

    /// Production rule from which this node is part of.
    ProductionRule generatorPR;

    /// Value of the terminal used if the node is of Terminal type.
    std::string termValue;

    Node()
    {
        type = TreeNodeType::None;
        termInstance = Terminal();
        nonTermInstance = NonTerminal();
    }

    /// NonTerminal node constructor.
    /// \param nt The NonTerminal type.
    explicit Node(const NonTerminal& nt)
    {
        type = TreeNodeType::NonTerminal;
        nonTermInstance = nt;
        termInstance = Terminal();
    }

    /// Terminal node constructor.
    /// \param t The terminal.
    explicit Node(const Terminal& t)
    {
        type = TreeNodeType::Terminal;
        nonTermInstance = NonTerminal();
        termInstance = t;
    }

    /// NonTerminal node constructor with generator production rule.
    /// \param productionRule The production rule that builds this node.
    /// \param nt The NonTerminal type.
    Node(const ProductionRule& productionRule, const NonTerminal& nt)
    {
        type = TreeNodeType::NonTerminal;
        nonTermInstance = nt;
        termInstance = Terminal();
        generatorPR = productionRule;
    }

    /// Terminal node with value constructor.
    /// \param t The terminal.
    /// \param value The value.
    Node(const Terminal& t, const std::string& value)
    {
        type = TreeNodeType::Terminal;
        nonTermInstance = NonTerminal();
        termInstance = t;
        termValue = value;
    }

    Node(const Node& other)
    {
        type = other.type;
        nonTermInstance = other.nonTermInstance;
        termInstance = other.termInstance;
        generatorPR = other.generatorPR;
        termValue = other.termValue;
    }

    bool operator==(const Node& other) const
    {
        const bool sameType = this->type == other.type;
        const bool sameTerm = this->termInstance == other.termInstance;
        const bool sameNonTerm = this->nonTermInstance == other.nonTermInstance;
        const bool sameValue = this->termValue == other.termValue;
        return sameType && sameTerm && sameNonTerm && sameValue;
    }
    bool operator!=(const Node& other) const
    {
        return !(*this == other);
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

    /// Check if both nodes have the same term ID.
    /// \param other The other node.
    /// \return Whether nodes have the same ID.
    [[nodiscard]]
    bool SameID(const Node& other) const
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
};

/// Represents a node of an n-ary tree
struct TreeNode : Node
{
    /// Parent of the node. If the node is a root, its value will be null.
    TreeNode* parent;

    /// List of references to the children of this node.
    std::vector<TreeNode*> children;

    /// Result of the synthesis of this node.
    std::string expressionSynthesis;

    /// Result of the evaluation of this node.
    std::string expressionEvaluation;

    /// Constructor of an empty node.
    TreeNode() : Node()
    {
        expressionSynthesis = "";
        expressionEvaluation = "";
        parent = nullptr;
    }

    /// Node copy constructor.
    explicit TreeNode(const Node& other) : Node(other)
    {
        expressionSynthesis = "";
        expressionEvaluation = "";
        parent = nullptr;
    }

    /// NonTerminal node constructor.
    /// \param nt The NonTerminal type.
    explicit TreeNode(const NonTerminal& nt) : Node(nt)
    {
        expressionSynthesis = "";
        expressionEvaluation = "";
        parent = nullptr;
    }

    /// NonTerminal node constructor.
    /// \param nt The NonTerminal type.
    /// \param children Children instances that tells this node how to build its own children nodes.
    TreeNode(const NonTerminal& nt, const std::vector<TreeNode>& children) : Node(nt)
    {
        expressionSynthesis = "";
        expressionEvaluation = "";
        parent = nullptr;

        for (const auto& c : children)
            AddChildNode(new TreeNode(c), this);
    }

    /// NonTerminal node constructor with generator production rule.
    /// \param productionRule The production rule that builds this node.
    /// \param nt The NonTerminal type.
    TreeNode(const ProductionRule& productionRule, const NonTerminal& nt) : Node(productionRule, nt)
    {
        expressionSynthesis = "";
        expressionEvaluation = "";
        parent = nullptr;
    }

    /// NonTerminal node constructor that takes TreeNode instances as children and uses them as a blueprint to
    /// construct its own children nodes.
    /// \param productionRule The production rule that builds this node.
    /// \param nt The NonTerminal type.
    /// \param children Children instances that tells this node how to build its own children nodes.
    TreeNode(const ProductionRule& productionRule, const NonTerminal& nt, const std::vector<TreeNode>& children)
        : Node(productionRule, nt)
    {
        expressionSynthesis = "";
        expressionEvaluation = "";
        parent = nullptr;

        for (const auto& c : children)
            AddChildNode(new TreeNode(c), this);
    }

    /// Terminal node constructor.
    /// \param t The terminal.
    explicit TreeNode(const Terminal& t) : Node(t)
    {
        expressionSynthesis = "";
        expressionEvaluation = "";
        parent = nullptr;
    }

    /// Terminal node with value constructor.
    /// \param t The terminal.
    /// \param value The value.
    TreeNode(const Terminal& t, const std::string& value) : Node(t, value)
    {
        expressionSynthesis = "";
        expressionEvaluation = "";
        parent = nullptr;
    }

    /// Copy constructor that copies all linked nodes.
    /// \param other The other node.
    TreeNode(const TreeNode& other) : Node(other)
    {
        parent = nullptr;
        for (auto c : other.children)
            AddChildNode(new TreeNode(*c), this);
    }

    /// Performs a copy of the node term without children.
    /// \param other The other node.
    /// \return A copy of the node without its children.
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

    /// Reset the synthesis of this node.
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

    /// Reset the evaluation of this node.
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

    /// Add node as a child.
    /// \param node Reference to the child node.
    void AddChildNode(TreeNode* node, TreeNode* nodeParent = nullptr)
    {
        if (nodeParent != nullptr)
            node->parent = nodeParent;
        children.push_back(node);
    }

    /// Add child NonTerminal node to the target.
    /// \param nonTerm NonTerminal instance.
    /// \param generatorPR Production rule from which this node is part of.
    /// \return Pointer to the newly created node.
    TreeNode* AddChildTerm(const NonTerminal& nonTerm, const ProductionRule& pgeneratorPR)
    {
        auto* newNode = new TreeNode(nonTerm);
        newNode->generatorPR = pgeneratorPR;
        newNode->parent = this;
        AddChildNode(newNode);
        return newNode;
    }

    /// Add child Terminal node to the target. If the Terminal has only one possible value, sets it as the termValue.
    /// If not, leaves the value empty.
    /// \param target Node where the child will be placed.
    /// \param term Terminal instance.
    /// \return Pointer to the newly created node.
    TreeNode* AddChildTerm(const Terminal& term)
    {
        auto* newNode = new TreeNode(term);
        newNode->termValue = term.GetRandomValue();
        newNode->parent = this;
        AddChildNode(newNode);
        return newNode;
    }

    /// Add child Terminal node to the target.
    /// \param target Node where the child will be placed.
    /// \param term Terminal instance.
    /// \param termValue Terminal value.
    /// \return Pointer to the newly created node.
    TreeNode* AddChildTerm(const Terminal& term, const std::string& ptermValue)
    {
        auto* newNode = new TreeNode(term);
        newNode->termValue = ptermValue;
        newNode->parent = this;
        this->AddChildNode(newNode);
        return newNode;
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