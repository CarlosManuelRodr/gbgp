#pragma once
#include <string>
#include "production_rule.h"

namespace gbgp
{
    /// A node can be either a Terminal or a NonTerminal. A None type is provided to instantiate empty nodes.
    enum class NodeType
    {
        None, NonTerminal, Terminal
    };

    /// Simple node that contains either a Terminal or NonTerminal with the ProductionRule used to create it.
    /// This struct is intended for exporting and serialization.
    struct Node
    {
        /// Type of the node.
        NodeType type;

        /// NonTerminal instance used when the node is of NonTerminal type.
        NonTerminal nonTermInstance;

        /// Terminal instance used when the node is of Terminal type.
        Terminal termInstance;

        /// Production rule from which this node is part of.
        ProductionRule generatorPR;

        /// Value of the terminal used when the node is of Terminal type.
        std::string termValue;

        /// Empty constructor.
        Node()
        {
            type = NodeType::None;
            termInstance = Terminal();
            nonTermInstance = NonTerminal();
            generatorPR = ProductionRule();
            termValue = std::string();
        }

        /// NonTerminal node constructor.
        /// \param nt The NonTerminal type.
        explicit Node(const NonTerminal& nt)
        {
            type = NodeType::NonTerminal;
            nonTermInstance = nt;
            termInstance = Terminal();
            generatorPR = ProductionRule();
            termValue = std::string();
        }

        /// Terminal node constructor.
        /// \param t The terminal.
        explicit Node(const Terminal& t)
        {
            type = NodeType::Terminal;
            nonTermInstance = NonTerminal();
            termInstance = t;
            generatorPR = ProductionRule();
            termValue = std::string();
        }

        /// NonTerminal node constructor with generator production rule.
        /// \param productionRule The production rule that builds this node.
        /// \param nt The NonTerminal type.
        Node(const ProductionRule& productionRule, const NonTerminal& nt)
        {
            type = NodeType::NonTerminal;
            nonTermInstance = nt;
            termInstance = Terminal();
            generatorPR = productionRule;
            termValue = std::string();
        }

        /// Terminal node with value constructor.
        /// \param t The terminal.
        /// \param value The value.
        Node(const Terminal& t, const std::string& value)
        {
            type = NodeType::Terminal;
            nonTermInstance = NonTerminal();
            termInstance = t;
            termValue = value;
            generatorPR = ProductionRule();
        }

        /// Copy constructor.
        /// \param other The node to be copied.
        Node(const Node& other)
        {
            type = other.type;
            nonTermInstance = other.nonTermInstance;
            termInstance = other.termInstance;
            generatorPR = other.generatorPR;
            termValue = other.termValue;
        }

        /// Parameter by parameter constructor.
        Node(NodeType ptype, const NonTerminal& pNonTermInstance, const Terminal& pTermInstance,
             const ProductionRule& pGeneratorPR, const std::string& pTermValue)
        {
            type = ptype;
            nonTermInstance = pNonTermInstance;
            termInstance = pTermInstance;
            generatorPR = pGeneratorPR;
            termValue = pTermValue;
        }

        bool operator==(const Node& other) const
        {
            const bool sameType = this->type == other.type;
            const bool sameTerm = this->termInstance == other.termInstance;
            const bool sameNonTerm = this->nonTermInstance == other.nonTermInstance;
            const bool sameValue = this->termValue == other.termValue;
            const bool sameGeneratorPR = this->generatorPR == other.generatorPR;
            return sameType && sameTerm && sameNonTerm && sameValue && sameGeneratorPR;
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
            return (type == NodeType::NonTerminal) ?
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

            if (type == NodeType::NonTerminal)
                return this->nonTermInstance == other.nonTermInstance;
            else if (type == NodeType::Terminal)
                return this->termInstance == other.termInstance;
            else
                return false;
        }

        [[nodiscard]]
        virtual std::string GetTypeString() const
        {
            switch (type) {
                case NodeType::NonTerminal:
                    return "NonTerminal";
                case NodeType::Terminal:
                    return "Terminal";
                case NodeType::None:
                default:
                    return "None";
            }
        }

        /// Get node representation as string.
        [[nodiscard]]
        virtual std::string ToString() const
        {
            return "Node(type=" + GetTypeString() + ", label=" + GetLabel() + ", generatorPR="
                   + generatorPR.ToString() + ")";
        }

        /// Serialization hook.
        template<class Archive> void serialize(Archive& ar)
        {
            ar(type, nonTermInstance, termInstance, generatorPR, termValue);
        }
    };

    /// Represents a node of an n-ary tree. This struct is not serializable.
    struct TreeNode final : Node
    {
        /// Parent of the node. If the node is a root, its value will be null.
        TreeNode* parent;

        /// List of references to the children of this node.
        std::vector<TreeNode*> children;

        /// Result of the synthesis of this node.
        std::string expressionSynthesis;

        /// Result of the evaluation of this node.
        std::string expressionEvaluation;

        /// Capture ID for copying terminals on prune rules.
        std::optional<int> captureID = std::nullopt;

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
                AddChildNode(new TreeNode(c));
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
                AddChildNode(new TreeNode(c));
        }

        /// Terminal node constructor.
        /// \param t The terminal.
        explicit TreeNode(const Terminal& t) : Node(t)
        {
            expressionSynthesis = "";
            expressionEvaluation = "";
            parent = nullptr;
        }

        /// Terminal node constructor.
        /// \param t The terminal.
        /// \param pCaptureID The capture ID for copying values. If none, set to -1.
        TreeNode(const Terminal& t, int pCaptureID) : Node(t)
        {
            captureID = pCaptureID;
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
            captureID = other.captureID;

            for (auto c : other.children)
                AddChildNode(new TreeNode(*c));
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
            copyNode->captureID = other->captureID;
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
        void AddChildNode(TreeNode* node)
        {
            node->parent = this;
            children.push_back(node);
        }

        /// Add child NonTerminal node to the target.
        /// \param nonTerm NonTerminal instance.
        /// \param generatorPR Production rule from which this node is part of.
        /// \return Pointer to the newly created node.
        TreeNode* AddChildTerm(const NonTerminal& nonTerm, const ProductionRule& pgeneratorPR)
        {
            auto* newNode = new TreeNode(pgeneratorPR, nonTerm);
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
            auto* newNode = new TreeNode(term, term.GetRandomValue());
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
            auto* newNode = new TreeNode(term, ptermValue);
            this->AddChildNode(newNode);
            return newNode;
        }

        /// Check if the node has a capture ID.
        [[nodiscard]]
        bool HasCaptureID() const
        {
            return captureID.has_value();
        }

        /// Check if both tree nodes have the same capture ID.
        /// \param other The other node.
        /// \return Do both nodes have the same capture ID?
        [[nodiscard]]
        bool SameCaptureID(const TreeNode& other) const
        {
            return captureID == other.captureID;
        }

        /// Get node representation as string.
        [[nodiscard]]
        std::string ToString() const override
        {
            std::string output = "TreeNode(";

            output += GetLabel();
            output += " -> ";

            for (auto child : children)
                output += child->GetLabel() + ((child == children.back()) ? "" : ", ");

            output += ")";
            return output;
        }

        [[nodiscard]]
        std::string ToStringDeep() const
        {
            std::string output = "TreeNode(";

            output += GetLabel();

            if (!children.empty())
            {
                output += " -> ";
                for (auto child: children)
                    output += child->ToStringDeep();
            }

            output += ")";
            return output;
        }
    };
}