#pragma once
#include <iostream>
#include <ostream>
#include <sstream>
#include <queue>
#include <functional>
#include <string>
#include <exception>
#include <type_traits>
#include "grammar.h"

//*****************************
//*    Node implementation    *
//****************************/

/// A node can be either a Terminal or a NonTerminal. A None type is provided for convenience.
enum class TreeNodeType
{
    None, NonTerminal, Terminal
};

/// Represents a node of an n-ary tree
/// \tparam TerminalType enum class with the Terminal IDs specified by the user.
/// \tparam NonTerminalType enum class with the NonTerminal IDs specified by the user.
template <typename TerminalType, typename NonTerminalType, typename ValueType = std::string> class TreeNode
{
private:
    /// Generate an Unique Identifier for this node.
    /// \return A std::string containing the generated UUID.
    static std::string GenerateUUID()
    {
        static std::random_device dev;
        static std::mt19937 rng(dev());

        std::uniform_int_distribution<int> dist(0, 15);

        const char* v = "0123456789abcdef";
        const bool dash[] = { false, false, false, false, true, false, true, false,
                              true, false, true, false, false, false, false, false };

        std::string res;
        for (bool i : dash) {
            if (i) res += "-";
            res += v[dist(rng)];
            res += v[dist(rng)];
        }
        return res;
    }
public:
    /// Type of the node.
    TreeNodeType type;

    /// NonTerminal instance used if the node is of NonTerminal type.
    NonTerminal<NonTerminalType> nonTermInstance;

    /// Terminal instance used if the node is of Terminal type.
    Terminal<TerminalType, ValueType> termInstance;

    /// Production rule from which this node is part of.
    ProductionRule<TerminalType, NonTerminalType, ValueType> generatorPR;

    /// Parent of the node. If the node is a root, its value will be null.
    TreeNode<TerminalType, NonTerminalType, ValueType>* parent;

    /// List of references to the children of this node.
    std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*> children;

    /// Value of the terminal used if the node is of Terminal type.
    ValueType termValue;

    /// Unique identifier of this node.
    std::string uuid;

    /// Result of the synthesis of this node.
    std::string expressionSynthesis;

    /// Result of the evaluation of this node.
    ValueType expressionEvaluation;

    /// Constructor of an empty node.
    TreeNode()
    {
        type = TreeNodeType::None;
        termInstance = Terminal<TerminalType, ValueType>();
        nonTermInstance = NonTerminal<NonTerminalType>();
        expressionSynthesis = "";
        termValue = ValueType();
        expressionEvaluation = ValueType();
        parent = nullptr;
        uuid = GenerateUUID();
    }

    /// Constructor of a NonTerminal node.
    explicit TreeNode(const NonTerminal<NonTerminalType>& nt)
    {
        type = TreeNodeType::NonTerminal;
        nonTermInstance = nt;
        termInstance = Terminal<TerminalType, ValueType>();
        expressionSynthesis = "";
        termValue = ValueType();
        expressionEvaluation = ValueType();
        parent = nullptr;
        uuid = GenerateUUID();
    }

    TreeNode(const NonTerminal<NonTerminalType>& nt,
             std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>> children)
    {
        type = TreeNodeType::NonTerminal;
        nonTermInstance = nt;
        termInstance = Terminal<TerminalType, ValueType>();
        expressionSynthesis = "";
        termValue = ValueType();
        expressionEvaluation = ValueType();
        parent = nullptr;
        uuid = GenerateUUID();

        for (auto c : children)
            AddChildNode(new TreeNode<TerminalType, NonTerminalType, ValueType>(c), this);
    }

    TreeNode(ProductionRule<TerminalType, NonTerminalType, ValueType> productionRule,
             const NonTerminal<NonTerminalType>& nt,
             std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>> children)
    {
        type = TreeNodeType::NonTerminal;
        nonTermInstance = nt;
        termInstance = Terminal<TerminalType, ValueType>();
        expressionSynthesis = "";
        termValue = ValueType();
        expressionEvaluation = ValueType();
        parent = nullptr;
        uuid = GenerateUUID();
        generatorPR = productionRule;

        for (auto c : children)
            AddChildNode(new TreeNode<TerminalType, NonTerminalType, ValueType>(c), this);
    }

    /// Constructor of a Terminal node.
    explicit TreeNode(const Terminal<TerminalType, ValueType>& t)
    {
        type = TreeNodeType::Terminal;
        nonTermInstance = NonTerminal<NonTerminalType>();
        termInstance = t;
        expressionSynthesis = "";
        termValue = ValueType();
        expressionEvaluation = ValueType();
        parent = nullptr;
        uuid = GenerateUUID();
    }

    TreeNode(const Terminal<TerminalType, ValueType>& t, ValueType value)
    {
        type = TreeNodeType::Terminal;
        nonTermInstance = NonTerminal<NonTerminalType>();
        termInstance = t;
        expressionSynthesis = "";
        termValue = value;
        expressionEvaluation = ValueType();
        parent = nullptr;
        uuid = GenerateUUID();
    }

    /// Copy constructor.
    TreeNode(const TreeNode<TerminalType, NonTerminalType, ValueType>& other)
    {
        type = other.type;
        nonTermInstance = other.nonTermInstance;
        termInstance = other.termInstance;
        generatorPR = other.generatorPR;
        expressionSynthesis = other.expressionSynthesis;
        expressionEvaluation = other.expressionEvaluation;
        termValue = other.termValue;
        parent = other.parent;
        children = other.children;
        uuid = GenerateUUID();
    }

    /// Copy constructor by reference.
    explicit TreeNode(TreeNode<TerminalType, NonTerminalType, ValueType>* other)
    {
        type = other->type;
        nonTermInstance = other->nonTermInstance;
        termInstance = other->termInstance;
        generatorPR = other->generatorPR;
        expressionSynthesis = other->expressionSynthesis;
        expressionEvaluation = other->expressionEvaluation;
        termValue = other->termValue;
        parent = other->parent;
        uuid = GenerateUUID();
    }

    ~TreeNode()
    {
        parent = nullptr;
        children.clear();
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
        expressionEvaluation = ValueType();
    }

    /// Check if this node has been evaluated..
    [[nodiscard]]
    bool IsEvaluated() const
    {
        return expressionEvaluation != ValueType();
    }

    /// Add the node as a child.
    /// \param node Reference to the child node.
    void AddChildNode(TreeNode<TerminalType, NonTerminalType, ValueType>* node,
                      TreeNode<TerminalType, NonTerminalType, ValueType>* nodeParent = nullptr)
    {
        if (nodeParent != nullptr)
            node->parent = nodeParent;
        children.push_back(node);
    }

    /// Returns a formatted label of the node.
    [[nodiscard]]
    std::string GetLabel(bool showUUID = false) const
    {
        std::string label = (type == TreeNodeType::NonTerminal) ? nonTermInstance.label : termInstance.label;
        if (showUUID)
            label += " UUID: " + uuid;
        return label;
    }
};

//*****************************
//*    CST Implementation     *
//****************************/

/// Contains the expression tree, provides interfaces for manipulating its structure, synthesizing and evaluating the tree.
/// \tparam TerminalType enum class with the Terminal IDs specified by the user.
/// \tparam NonTerminalType enum class with the NonTerminal IDs specified by the user.
template <typename TerminalType, typename NonTerminalType, typename ValueType = std::string> class ConcreteSyntaxTree
{
private:
    static constexpr bool stringValueType = std::is_same<ValueType, std::string>::value;

    /// Root of the tree.
    TreeNode<TerminalType, NonTerminalType, ValueType>* root;

    /// Find the first position of a NonTerminal of type id.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    /// \param id Identifier of the non terminal.
    /// \param avoid List of positions to avoid.
    /// \param currentPosition Current position of the cursor.
    /// \param elementsToSynthesize Number of nodes left to be synthesized.
    /// \return Position as index. If the search fails, returns -1.
    [[nodiscard]]
    int FindIndexOfTerm(const std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*>& dfspo,
                        NonTerminalType id, const std::vector<unsigned>& avoid, int currentPosition,
                        int elementsToSynthesize) const
    {
        for (unsigned i = currentPosition - elementsToSynthesize; i < dfspo.size(); i++)
        {
            if (dfspo[i]->type == TreeNodeType::NonTerminal &&
                dfspo[i]->nonTermInstance.id == id && !vector_contains_q(avoid, i))
                return static_cast<int>(i);
        }
        return -1;
    }

    /// Find the first position of a term of type id.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    /// \param id Identifier of the term.
    /// \param avoid List of positions to avoid.
    /// \param currentPosition Current position of the cursor.
    /// \param elementsToSynthesize Number of nodes left to be synthesized.
    /// \return Position as index. If the search fails, returns -1.
    [[nodiscard]]
    int FindIndexOfTerm(const std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*>& dfspo, TerminalType id,
                        const std::vector<unsigned>& avoid, int currentPosition, int elementsToSynthesize) const
    {
        for (unsigned i = currentPosition - elementsToSynthesize; i < dfspo.size(); i++)
        {
            if (dfspo[i]->type == TreeNodeType::Terminal && dfspo[i]->termInstance.id == id &&
                !vector_contains_q(avoid, i))
                return static_cast<int>(i);
        }
        return -1;
    }

    /// Find the index of the first non-evaluated non-terminal.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    /// \return The index if the first non-evaluated non-terminal.
    [[nodiscard]]
    unsigned NextToEvaluate(std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*>& dfspo) const
    {
        for (unsigned i = 0; i < dfspo.size(); i++)
            if (dfspo[i]->type == TreeNodeType::NonTerminal && !dfspo[i]->IsEvaluated())
                return i;
        return dfspo.size();
    }

    /// Find the index of the first non-synthesized non-terminal.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    /// \return The index if the first non-synthesized non-terminal.
    [[nodiscard]]
    unsigned NextToSynthesize(std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*>& dfspo) const
    {
        for (unsigned i = 0; i < dfspo.size(); i++)
            if (dfspo[i]->type == TreeNodeType::NonTerminal && !dfspo[i]->IsSynthesized())
                return i;
        return dfspo.size();
    }

    /// Copy a tree by creating new instances of all the nodes.
    /// \param copyTree Pointer to the tree that holds the copy.
    /// \param originalTree Pointer to the original tree that will be copied.
    void CopyTree(TreeNode<TerminalType, NonTerminalType, ValueType>* copyTree,
                  TreeNode<TerminalType, NonTerminalType, ValueType>* originalTree) const
    {
        if (originalTree != nullptr)
        {
            copyTree->type = originalTree->type;
            copyTree->termInstance = originalTree->termInstance;
            copyTree->termValue = originalTree->termValue;
            copyTree->nonTermInstance = originalTree->nonTermInstance;
            copyTree->generatorPR = originalTree->generatorPR;
            copyTree->expressionSynthesis = copyTree->expressionSynthesis;

            for (TreeNode<TerminalType, NonTerminalType, ValueType>* n : originalTree->children)
            {
                auto* copyNode = new TreeNode<TerminalType, NonTerminalType, ValueType>(n);
                copyTree->AddChildNode(copyNode);
                CopyTree(copyNode, n);
            }
        }
    }

    /// Print node in the terminal with the specified depth.
    /// \param stream target stream to print.
    /// \param node current node to print.
    /// \param depth level of indent of the printed node.
    /// \param showUUID whether to show the UUID of the node.
    void PrintNodeAsTree(std::ostream& stream, TreeNode<TerminalType, NonTerminalType, ValueType>* node, int depth,
                         bool showUUID = false) const
    {
        std::string output = "|";
        output.append(depth, '-');
        output += node->GetLabel(showUUID);
        std::cout << output << std::endl;
    }

    /// <summary>
    /// Recursive implementation. Prints the tree in the output stream.
    /// </summary>
    /// <param name="node">Node to print.</param>
    /// <param name="depth">Current depth.</param>
    void PrintTree(std::ostream& stream, TreeNode<TerminalType, NonTerminalType, ValueType>* node, int depth,
                   bool showUUID = false) const
    {
        for (TreeNode<TerminalType, NonTerminalType, ValueType>* n : node->children)
        {
            this->PrintNodeAsTree(stream, n, depth, showUUID);
            PrintTree(stream, n, depth + 1, showUUID);
        }
    }

public:
    //***************************************************
    //*      CST Construction and status management     *
    //**************************************************/

    /// Creates an empty ConcreteSyntaxTree.
    /// \param grammar The formal grammar of the ConcreteSyntaxTree.
    ConcreteSyntaxTree()
    {
        root = nullptr;
    }

    /// Builds a tree from a root node.
    /// \param proot Pointer to the root of the tree.
    /// \param deepCopy If set to true, it will copy the tree into a new instance.
    explicit ConcreteSyntaxTree(TreeNode<TerminalType, NonTerminalType, ValueType>* proot, bool deepCopy = false)
    {
        root = new TreeNode<TerminalType, NonTerminalType, ValueType>(proot);
        root->parent = nullptr;
        if (deepCopy)
            this->CopyTree(root, proot);
        this->ClearEvaluation();
    }

    /// Builds a tree from a root node.
    /// \param proot Root of the tree.
    /// \param deepCopy If set to true, it will copy the tree into a new instance.
    explicit ConcreteSyntaxTree(TreeNode<TerminalType, NonTerminalType, ValueType> proot, bool deepCopy = false)
    {
        root = new TreeNode<TerminalType, NonTerminalType, ValueType>(proot);
        root->parent = nullptr;
        if (deepCopy)
            this->CopyTree(root, &proot);
        this->ClearEvaluation();
    }

    /// Copy constructor.
    /// \param other ConcreteSyntaxTree to be copied.
    ConcreteSyntaxTree(const ConcreteSyntaxTree<TerminalType, NonTerminalType, ValueType>& other)
    {
        root = new TreeNode<TerminalType, NonTerminalType, ValueType>(other.root);
        root->parent = nullptr;
        this->CopyTree(root, other.root);
        this->ClearEvaluation();
    }

    /// Copy constructor from reference.
    /// \param other Pointer to the ConcreteSyntaxTree to be copied.
    explicit ConcreteSyntaxTree(ConcreteSyntaxTree<TerminalType, NonTerminalType, ValueType>* other)
    {
        root = new TreeNode<TerminalType, NonTerminalType, ValueType>(other->root);
        root->parent = nullptr;
        this->CopyTree(root, other->root);
        this->ClearEvaluation();
    }

    ~ConcreteSyntaxTree()
    {
        this->Destroy();
    }

    void SetRootRule(ProductionRule<TerminalType, NonTerminalType, ValueType> startRule)
    {
        root = new TreeNode<TerminalType, NonTerminalType, ValueType>(startRule.from);
        root->parent = nullptr;
        root->generatorPR = startRule;
    }

    /// Reset the tree to its empty state.
    void Reset()
    {
        if (root != nullptr)
        {
            std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*> nodeList = DepthFirstScanPostorder();
            for (TreeNode<TerminalType, NonTerminalType, ValueType>* n : nodeList)
            {
                if (n != nullptr)
                {
                    delete n;
                    n = nullptr;
                }
            }

            root = nullptr;
        }
    }

    /// Destroy the tree by deleting all its nodes.
    void Destroy()
    {
        if (root != nullptr)
        {
            std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*>
            nodeList = this->DepthFirstScanPostorder(root);

            for (TreeNode<TerminalType, NonTerminalType, ValueType>*& n : nodeList)
            {
                if (n != nullptr)
                {
                    delete n;
                    n = nullptr;
                }
            }
            root = nullptr;
        }
    }

    /// Clear the expressionSynthesis and expressionEvaluation of every node.
    void ClearEvaluation()
    {
        std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*>
        nodeList = this->DepthFirstScanPostorder(root);

        for (TreeNode<TerminalType, NonTerminalType, ValueType>* n : nodeList)
        {
            if (n->type == TreeNodeType::NonTerminal)
            {
                n->expressionSynthesis = "";
                n->expressionEvaluation = ValueType();
            }
        }
    }

    //**************************
    //*    Tree manipulation   *
    //*************************/

    /// Check if the tree is empty.
    [[nodiscard]]
    bool IsEmpty() const
    {
        return (root->children.size() == 0);
    }

    /// Returns a reference to the root.
    /// \return Pointer to the root node.
    [[nodiscard]]
    TreeNode<TerminalType, NonTerminalType, ValueType>* Root() const
    {
        return root;
    }

    /// Add child NonTerminal node to the target.
    /// \param target Node where the child will be placed.
    /// \param nonTerm NonTerminal instance.
    /// \param generatorPR Production rule from which this node is part of.
    /// \return Pointer to the newly created node.
    TreeNode<TerminalType, NonTerminalType, ValueType>*
    AddNode(TreeNode<TerminalType, NonTerminalType, ValueType>* target, const NonTerminal<NonTerminalType>& nonTerm,
            ProductionRule<TerminalType, NonTerminalType, ValueType> generatorPR)
    {
        auto* newNode = new TreeNode<TerminalType, NonTerminalType, ValueType>(nonTerm);
        newNode->generatorPR = generatorPR;
        newNode->parent = target;
        target->AddChildNode(newNode);
        return newNode;
    }

    /// Add child Terminal node to the target. If the Terminal has only one possible value, sets it as the termValue.
    /// If not, leaves the value empty.
    /// \param target Node where the child will be placed.
    /// \param term Terminal instance.
    /// \return Pointer to the newly created node.
    TreeNode<TerminalType, NonTerminalType, ValueType>*
    AddNode(TreeNode<TerminalType, NonTerminalType, ValueType>* target, const Terminal<TerminalType, ValueType>& term)
    {
        auto* newNode = new TreeNode<TerminalType, NonTerminalType, ValueType>(term);
        newNode->termValue = term.GetValue();
        newNode->parent = target;
        target->AddChildNode(newNode);
        return newNode;
    }

    /// Add child Terminal node to the target.
    /// \param target Node where the child will be placed.
    /// \param term Terminal instance.
    /// \param termValue Terminal value.
    /// \return Pointer to the newly created node.
    TreeNode<TerminalType, NonTerminalType, ValueType>*
    AddNode(TreeNode<TerminalType, NonTerminalType, ValueType>* target, const Terminal<TerminalType, ValueType>& term,
            const ValueType& termValue)
    {
        auto* newNode = new TreeNode<TerminalType, NonTerminalType, ValueType>(term);
        newNode->termValue = termValue;
        newNode->parent = target;
        target->AddChildNode(newNode);
        return newNode;
    }

    /// Removes the subtree starting from rootOfSubtree.
    /// \param rootOfSubtree Pointer to the root of the subtree to be deleted.
    void RemoveSubtree(TreeNode<TerminalType, NonTerminalType, ValueType>* rootOfSubtree)
    {
        std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*>
        nodeList = this->DepthFirstScanPostorder(rootOfSubtree);

        nodeList.pop_back();
        for (TreeNode<TerminalType, NonTerminalType, ValueType>*& n : nodeList)
        {
            if (n != nullptr)
            {
                delete n;
                n = nullptr;
            }
        }
        rootOfSubtree->children.clear();
        ClearEvaluation();
    }

    /// Get subtree starting from subTreeStartNode.
    /// \param subTreeStartNode Root node of subtree.
    /// \return A new ConcreteSyntaxTree starting from the subTreeStartNode.
    [[nodiscard]]
    ConcreteSyntaxTree<TerminalType, NonTerminalType, ValueType>
    GetSubtree(TreeNode<TerminalType, NonTerminalType, ValueType>* subTreeStartNode) const
    {
        return ConcreteSyntaxTree<TerminalType, NonTerminalType, ValueType>(subTreeStartNode, true);
    }

    /// Insert a copy of the subtree into the position at insertNode.
    /// \param insertNode Node where the subtree will be inserted.
    /// \param subtreeStartNode Pointer to the subtree to copy and insert.
    void InsertSubtree(TreeNode<TerminalType, NonTerminalType, ValueType>* insertNode,
                       TreeNode<TerminalType, NonTerminalType, ValueType>* subtreeStartNode)
    {
        // Check that both nodes are NonTerminals
        if (insertNode->type == TreeNodeType::NonTerminal && subtreeStartNode->type == TreeNodeType::NonTerminal)
        {
            // Check that both nodes are of the same type.
            if (insertNode->nonTermInstance.id == subtreeStartNode->nonTermInstance.id)
            {
                auto* copySubtreeStartNode = new TreeNode<TerminalType, NonTerminalType, ValueType>();
                CopyTree(copySubtreeStartNode, subtreeStartNode);

                // Find parent and replace child reference of insertNode to subtreeStartNode.
                for (TreeNode<TerminalType, NonTerminalType, ValueType>*& child : insertNode->parent->children)
                {
                    if (child == insertNode)
                    {
                        child = copySubtreeStartNode;
                        child->parent = insertNode->parent;
                        break;
                    }
                }

                this->ClearEvaluation();
            }
            else
                throw std::runtime_error("Cannot insert subtree of different type of insertNode");
        }
        else
        {
            if (insertNode->type != TreeNodeType::NonTerminal)
                throw std::runtime_error("Cannot insert subtree in Terminal " + insertNode->termInstance.label);
            if (subtreeStartNode->type != TreeNodeType::NonTerminal)
                throw std::runtime_error("Cannot insert subtree of type Terminal " + subtreeStartNode->termInstance.label);
        }
    }


    //**********************
    //*      Utilities     *
    //*********************/

    /// Recursive implementation. Create random tree based on the production rules described in the variable grammarRules.
    /// \param maxDepth Maximum allowed tree depth.
    /// \param depth Current depth. If while creating a random tree, the depth reaches the maxDepth value, it will fail and return false.
    /// \param node Node from where the random tree will be created.
    /// \return True if creation is successful, false if not.
    bool TryCreateRandomTree(const Grammar<TerminalType, NonTerminalType, ValueType>& treeGrammar, int maxDepth,
                             int depth, TreeNode<TerminalType, NonTerminalType, ValueType>* node)
    {
        if (node->type == TreeNodeType::NonTerminal)
        {
            // Create children nodes based on the current node production rule.
            std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*> newNodes;
            for (ProductionElement<TerminalType, NonTerminalType, ValueType> pe : node->generatorPR.to)
            {
                if (pe.type == ProductionElementType::NonTerminal)
                    newNodes.push_back(AddNode(node, pe.nonterm, treeGrammar.GetRandomCompatibleRule(pe.nonterm.id)));
                if (pe.type == ProductionElementType::Terminal)
                    newNodes.push_back(AddNode(node, pe.term));
                if (pe.type == ProductionElementType::Unassigned)
                    throw std::runtime_error("Unassigned production element type");
            }

            // Create subtrees for children nodes.
            if (depth != maxDepth)
            {
                for (TreeNode<TerminalType, NonTerminalType, ValueType>* n : newNodes)
                {
                    bool branchCreationSuccess = TryCreateRandomTree(treeGrammar, maxDepth, depth + 1, n);
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
    bool TryCreateRandomTree(const Grammar<TerminalType, NonTerminalType, ValueType>& treeGrammar, int maxDepth = 10)
    {
        this->SetRootRule(treeGrammar.GetRootRule());

        // Create children nodes based on the selected rule
        std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*> newNodes;
        for (ProductionElement<TerminalType, NonTerminalType, ValueType> pe : root->generatorPR.to)
        {
            if (pe.type == ProductionElementType::NonTerminal)
                newNodes.push_back(AddNode(root, pe.nonterm, treeGrammar.GetRandomCompatibleRule(pe.nonterm.id)));
            if (pe.type == ProductionElementType::Terminal)
                newNodes.push_back(AddNode(root, pe.term));
            if (pe.type == ProductionElementType::Unassigned)
                throw std::runtime_error("Unassigned production element type");
        }

        for (TreeNode<TerminalType, NonTerminalType, ValueType>* n : newNodes)
        {
            bool branchCreationSuccess = TryCreateRandomTree(treeGrammar, maxDepth, 1, n);
            if (!branchCreationSuccess)
                return false;
        }

        return true;
    }

    /// Ensure the creation of a random tree by creating random trees until there is a success.
    /// \param maxDepth Maximum allowed tree depth.
    void CreateRandomTree(const Grammar<TerminalType, NonTerminalType, ValueType>& treeGrammar, int maxDepth = 10)
    {
        bool success = false;
        while (!success)
        {
            this->Reset();
            success = this->TryCreateRandomTree(treeGrammar, maxDepth);
        }
    }

    /// Prints the tree in the output stream.
    void PrintTree(std::ostream& stream = std::cout, bool showUUID = false)
    {
        this->PrintNodeAsTree(stream, root, 0, showUUID);
        
        for (TreeNode<TerminalType, NonTerminalType, ValueType>* n : root->children)
        {
            this->PrintNodeAsTree(stream, n, 1, showUUID);
            PrintTree(stream, n, 2, showUUID);
        }
    }

    //***************************
    //*     Tree traversals     *
    //**************************/

    /// Traverses the tree in a depth first pre-order.
    /// \param node Start node. The default value is the root of the tree.
    /// \return List of references of the traversed nodes.
    std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*>
    DepthFirstScanPreorder(TreeNode<TerminalType, NonTerminalType, ValueType>* node = nullptr)
    {
        /*
          Algorithm Preorder(tree)
          1. Visit the root.
          2. Traverse the left subtree, i.e., call Preorder(left-subtree)
          3. Traverse the right subtree, i.e., call Preorder(right-subtree)
        */
        std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*> output;

        if (node == nullptr)
        {
            node = root;
            output.push_back(node);
        }

        for (TreeNode<TerminalType, NonTerminalType, ValueType>* n : node->children)
        {
            output.push_back(n);

            const std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*>
            childrenTreeNodes = DepthFirstScanPreorder(n);

            output.insert(output.end(), childrenTreeNodes.begin(), childrenTreeNodes.end());
        }

        return output;
    }

    /// Traverses the tree in a depth first post-order.
    /// \param node Start node. The default value is the root of the tree.
    /// \return List of references of the traversed nodes.
    std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*>
    DepthFirstScanPostorder(TreeNode<TerminalType, NonTerminalType, ValueType>* node = nullptr)
    {
        /*
          Algorithm Postorder(tree)
          1. Traverse the left subtree, i.e., call Postorder(left-subtree)
          2. Traverse the right subtree, i.e., call Postorder(right-subtree)
          3. Visit the root.
        */

        std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*> output;

        if (node == nullptr)
            node = root;

        if (node->children.empty())
        {
            output.push_back(node);
            return output;
        }

        for (TreeNode<TerminalType, NonTerminalType, ValueType>* n : node->children)
        {
            const std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*>
            childrenTreeNodes = DepthFirstScanPostorder(n);

            output.insert(output.end(), childrenTreeNodes.begin(), childrenTreeNodes.end());
        }
        output.push_back(node);

        return output;
    }

    /// Traverses the tree in a breadth first order.
    /// \return List of references of the traversed nodes.
    std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*> BreadthFirstScan()
    {
        /*
            Algorithm BFS(tree)
            1) Create an empty queue q
            2) temp_node = root
            3) Loop while temp_node is not NULL
                a) print temp_node->data.
                b) Enqueue temp_nodes children (first left then right children) to q
                c) Dequeue a node from q.
        */

        std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*> output;
        std::queue<TreeNode<TerminalType, NonTerminalType, ValueType>*> q;
        q.push(root);

        while (!q.empty())
        {
            TreeNode<TerminalType, NonTerminalType, ValueType>* node = q.front();
            output.push_back(node);
            q.pop();

            for (TreeNode<TerminalType, NonTerminalType, ValueType>* n : node->children)
                q.push(n);
        }

        return output;
    }

    //***************************
    //*     Tree evaluation     *
    //**************************/

    /// Synthesize the first non-synthesized node and deletes the consumed nodes.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    void SynthesizeFirst(std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*>& dfspo)
    {
        // Get the rule of the element to be evaluated.
        unsigned nextIndex = NextToSynthesize(dfspo);
        if (nextIndex == dfspo.size())
            return;

        ProductionRule<TerminalType, NonTerminalType, ValueType> rule = dfspo[nextIndex]->generatorPR;
        std::string synthesis;
        std::vector<unsigned> toErase;

        // Process the elements of dfspo that match the semantic rules.
        for (SemanticElement<TerminalType, NonTerminalType, ValueType> se : rule.semanticRules)
        {
            if (se.type == SemanticElementType::String)
                synthesis += se.string;
            else if (se.type == SemanticElementType::NonTerminal)
            {
                const int pos = FindIndexOfTerm(dfspo, se.nonterm.id, toErase, nextIndex, rule.NumberOfRules());
                if (pos != -1)
                {
                    synthesis += dfspo[pos]->expressionSynthesis;
                    toErase.push_back(pos);
                }
                else
                {
                    std::string errorReport = "Could not find any NonTerm node of type " + se.nonterm.label;
                    errorReport += " during synthesis of node with UUID: " + dfspo[nextIndex]->uuid;
                    throw std::runtime_error(errorReport);
                }
            }
            else if (se.type == SemanticElementType::Terminal)
            {
                const int pos = FindIndexOfTerm(dfspo, se.term.id, toErase, nextIndex, rule.NumberOfRules());
                if (pos != -1)
                {
                    if constexpr(stringValueType)
                        synthesis += dfspo[pos]->termValue;
                    else
                        synthesis += std::to_string(dfspo[pos]->termValue);

                    toErase.push_back(pos);
                }
                else
                    throw std::runtime_error("Could not find any Term node of type " + se.term.label + " during expressionSynthesis");
            }
        }

        // Consume and delete
        dfspo[nextIndex]->expressionSynthesis = synthesis;
        delete_indexes(dfspo, toErase);
    }

    /// Synthesizes the tree into an expression using the semantic rules of the grammar.
    /// \return The synthesized expression as a std::string.
    [[nodiscard]]
    std::string SynthesizeExpression()
    {
        std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*> dfspo = this->DepthFirstScanPostorder();
        for (auto node : dfspo) node->ClearSynthesis();

        try
        {
            while (dfspo.size() > 1)
                SynthesizeFirst(dfspo);

            return dfspo.back()->expressionSynthesis;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Failed to synthesize expression: " << e.what() << std::endl;
            return "";
        }
    }

    /// Evaluate the first non-evaluate node and deletes the consumed nodes.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    /// \param evaluationContext pointer to the evaluation context.
    void EvaluateFirst(std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*>& dfspo,
                       EvaluationContext<ValueType>* evaluationContext)
    {
        // Get the rule of the element to be evaluated.
        unsigned nextIndex = NextToEvaluate(dfspo);
        if (nextIndex == dfspo.size())
            return;

        ProductionRule<TerminalType, NonTerminalType, ValueType> rule = dfspo[nextIndex]->generatorPR;
        std::vector<unsigned> toErase;

        evaluationContext->Prepare();

        // Push to the context the elements of dfspo that match the production rules.
        for (ProductionElement<TerminalType, NonTerminalType, ValueType> se : rule.to)
        {
            if (se.type == ProductionElementType::NonTerminal)
            {
                const int pos = FindIndexOfTerm(dfspo, se.nonterm.id, toErase, nextIndex, rule.NumberOfRules());
                if (pos != -1)
                {
                    evaluationContext->PushSemanticValue(dfspo[pos]->expressionEvaluation);
                    toErase.push_back(pos);
                }
                else
                {
                    std::string errorReport = "Could not find any NonTerm node of type " + se.nonterm.label;
                    errorReport += " during expression evaluation of node with UUID: " + dfspo[nextIndex]->uuid;
                    throw std::runtime_error(errorReport);
                }
            }
            else if (se.type == ProductionElementType::Terminal)
            {
                const int pos = FindIndexOfTerm(dfspo, se.term.id, toErase, nextIndex, rule.NumberOfRules());
                if (pos != -1)
                {
                    evaluationContext->PushSemanticValue(dfspo[pos]->termValue);
                    toErase.push_back(pos);
                }
                else
                    throw std::runtime_error("Could not find any Term node of type " + se.term.label + " during expression evaluation");
            }
        }

        // Execute semantic action and delete.
        if (rule.semanticAction != nullptr)
        {
            rule.semanticAction(evaluationContext);
            dfspo[nextIndex]->expressionEvaluation = evaluationContext->result();
        }
        else
            throw std::runtime_error("There is no semantic action for rule " + rule.ToString());

        delete_indexes(dfspo, toErase);
    }

    /// Evaluates the tree using the production rules of the grammar.
    /// \param evaluationContext pointer to the evaluation context.
    /// \return true if expression was evaluated correctly, false if not.
    bool Evaluate(EvaluationContext<ValueType>* evaluationContext)
    {
        std::vector<TreeNode<TerminalType, NonTerminalType, ValueType>*> dfspo = this->DepthFirstScanPostorder();
        for (auto node : dfspo) node->ClearEvaluation();

        try
        {
            while (dfspo.size() > 1)
                EvaluateFirst(dfspo, evaluationContext);

            return true;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Failed to evaluate expression: " << e.what() << std::endl;
            return false;
        }
    }
};