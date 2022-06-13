#pragma once
#include <iostream>
#include <ostream>
#include <queue>
#include "grammar.h"

/****************************
*                           *
*    Node implementation    *
*                           *
****************************/

/// <summary>
/// A node can be either a Terminal or a NonTerminal. A None type is provided for convenience.
/// </summary>
enum class TreeNodeType
{
    None, NonTerminal, Terminal
};

/// <summary>
/// Represents a node of an n-ary tree 
/// </summary>
/// <typeparam name="TerminalType">enum class with the Terminal IDs specified by the user.</typeparam>
/// <typeparam name="NonTerminalType">enum class with the NonTerminal IDs specified by the user.</typeparam>
template <typename TerminalType, typename NonTerminalType> class TreeNode
{
private:
    /// <summary>
    /// Generate an Unique Identifier for this node.
    /// </summary>
    /// <returns>A std::string containing the generated UUID.</returns>
    std::string GenerateUUID()
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
    /// <summary>
    /// Type of the node.
    /// </summary>
    TreeNodeType type;

    /// <summary>
    /// NonTerminal instance used if the node is of NonTerminal type.
    /// </summary>
    NonTerminal<NonTerminalType> nonTermInstance;

    /// <summary>
    /// Terminal instance used if the node is of Terminal type.
    /// </summary>
    Terminal<TerminalType> termInstance;

    /// <summary>
    /// Production rule from which this node is part of.
    /// </summary>
    ProductionRule<TerminalType, NonTerminalType> generatorPR;

    /// <summary>
    /// Parent of the node. If the node is a root, its value will be null.
    /// </summary>
    TreeNode<TerminalType, NonTerminalType>* parent;

    /// <summary>
    /// List of references to the children of this node.
    /// </summary>
    std::vector<TreeNode<TerminalType, NonTerminalType>*> children;

    /// <summary>
    /// Result of the synthesis of this node.
    /// </summary>
    std::string synthesis;

    /// <summary>
    /// Value of the terminal used if the node is of Terminal type.
    /// </summary>
    std::string termValue;

    /// <summary>
    /// Unique identifier of this node.
    /// </summary>
    std::string uuid;

    /// <summary>
    /// Constructor of an empty node.
    /// </summary>
    TreeNode()
    {
        type = TreeNodeType::None;
        termInstance = Terminal<TerminalType>();
        nonTermInstance = NonTerminal<NonTerminalType>();
        synthesis = "";
        parent = nullptr;
        uuid = GenerateUUID();
    }

    /// <summary>
    /// Constructor of a NonTerminal node.
    /// </summary>
    explicit TreeNode(const NonTerminal<NonTerminalType>& nt)
    {
        type = TreeNodeType::NonTerminal;
        nonTermInstance = nt;
        termInstance = Terminal<TerminalType>();
        synthesis = "";
        parent = nullptr;
        uuid = GenerateUUID();
    }

    /// <summary>
    /// Constructor of a Terminal node.
    /// </summary>
    explicit TreeNode(const Terminal<TerminalType>& t)
    {
        type = TreeNodeType::Terminal;
        nonTermInstance = NonTerminal<NonTerminalType>();
        termInstance = t;
        synthesis = "";
        parent = nullptr;
        uuid = GenerateUUID();
    }

    /// <summary>
    /// Copy constructor.
    /// </summary>
    TreeNode(const TreeNode<TerminalType, NonTerminalType>& other)
    {
        type = other.type;
        nonTermInstance = other.nonTermInstance;
        termInstance = other.termInstance;
        generatorPR = other.generatorPR;
        synthesis = other.synthesis;
        termValue = other.termValue;
        parent = other.parent;
        uuid = GenerateUUID();
    }

    /// <summary>
    /// Copy constructor by reference.
    /// </summary>
    explicit TreeNode(TreeNode<TerminalType, NonTerminalType>* other)
    {
        type = other->type;
        nonTermInstance = other->nonTermInstance;
        termInstance = other->termInstance;
        generatorPR = other->generatorPR;
        synthesis = other->synthesis;
        termValue = other->termValue;
        parent = other->parent;
        uuid = GenerateUUID();
    }

    ~TreeNode()
    {
        parent = nullptr;
        children.clear();
    }

    /// <summary>
    /// Check if this node has been synthesized.
    /// </summary>
    bool IsSynthesized()
    {
        return !synthesis.empty();
    }

    /// <summary>
    /// Add the node as a child.
    /// </summary>
    /// <param name="node">Reference to the child node.</param>
    void AddChildNode(TreeNode<TerminalType, NonTerminalType>* node)
    {
        children.push_back(node);
    }

    /// <summary>
    /// Returns a formatted label of the node.
    /// </summary>
    std::string GetLabel(bool showUUID = false)
    {
        std::string label;
        if (type == TreeNodeType::NonTerminal)
            label = nonTermInstance.label;
        else
            label = termInstance.label + " [" + termValue + "]";

        if (showUUID)
            label += " UUID: " + uuid;
        return label;
    }
};

/****************************
*                           *
*   Graph representation    *
*                           *
****************************/

template <typename TerminalType, typename NonTerminalType> struct GraphEdge
{
    int parent, child;

    GraphEdge()
    {
        parent = -1;
        child = -1;
    }
    GraphEdge(int pparent, int pchild)
    {
        parent = pparent;
        child = pchild;
    }
};

template <typename TerminalType, typename NonTerminalType> struct GraphNode
{
    TreeNodeType type;
    int nodeIndex;
    int generatorPRIndex;
    std::string id;
    std::string termValue;

    GraphNode(TreeNode<TerminalType, NonTerminalType>* node, int index)
    {
        type = node->type;
        nodeIndex = index;

        if (type == TreeNodeType::NonTerminal)
        {
            //generatorPRIndex = node->generatorPR; TODO: Fix this
            id = node->nonTermInstance.label;
            termValue = "";
        }
        else
        {
            generatorPRIndex = -1;
            id = node->termInstance.label;
            termValue = node->termValue;
        }
    }
};

template <typename TerminalType, typename NonTerminalType> struct Graph
{
    std::vector<GraphEdge<TerminalType, NonTerminalType>> edges;
    std::vector<GraphNode<TerminalType, NonTerminalType>> nodes;

    Graph() = default;
    Graph(std::vector<GraphEdge<TerminalType, NonTerminalType>> pedges, std::vector<GraphNode<TerminalType, NonTerminalType>> pnodes)
    {

    }
    void Clear()
    {

    }
    void PrintGraph()
    {

    }
    bool IsEmpty()
    {

    }
};


/****************************
*                           *
*    CST Implementation     *
*                           *
****************************/

/// <summary>
/// Contains the expression tree, provides interfaces for manipulating its structure and synthesize the expression into a std::string.
/// </summary>
/// <typeparam name="TerminalType">enum class with the Terminal IDs specified by the user.</typeparam>
/// <typeparam name="NonTerminalType">enum class with the NonTerminal IDs specified by the user.</typeparam>
template <typename TerminalType, typename NonTerminalType> class ConcreteSyntaxTree
{
private:
    /// <summary>
    /// Root of the tree.
    /// </summary>
    TreeNode<TerminalType, NonTerminalType>* root;

    /// <summary>
    /// Formal grammar that this ConcreteSyntaxTree uses.
    /// </summary>
    Grammar<TerminalType, NonTerminalType> treeGrammar;

    /// <summary>
    /// Check if all the nodes have been synthesized.
    /// </summary>
    bool AllNodesAreSynthesized(const std::vector<TreeNode<TerminalType, NonTerminalType>*>& nodes)
    {
        for (TreeNode<TerminalType, NonTerminalType>* n : nodes)
        {
            if (n->type == TreeNodeType::NonTerminal && !n->IsSynthesized())
                return false;
        }
        return true;
    }

    /// <summary>
    /// Print node in the terminal with the specified depth.
    /// </summary>
    void PrintNodeAsTree(TreeNode<TerminalType, NonTerminalType>* node, int depth, bool showUUID = false)
    {
        std::string output = "|";
        output.append(depth, '-');
        output += node->GetLabel(showUUID);
        std::cout << output << std::endl;
    }

    /// <summary>
    /// Find the first position of a NonTerminal of type id.
    /// </summary>
    /// <param name="dfspo">List of nodes traversed in DepthFirst PostOrder.</param>
    /// <param name="id">Identifier of the non terminal.</param>
    /// <param name="avoid">List of positions to avoid.</param>
    /// <param name="currentPosition">Current position of the cursor.</param>
    /// <param name="elementsToSynthesize">Number of nodes left to be synthesized.</param>
    /// <returns>Position as index. If the search fails, returns -1.</returns>
    int FirstPositionOfID(const std::vector<TreeNode<TerminalType, NonTerminalType>*>& dfspo, NonTerminalType id,
                          const std::vector<unsigned>& avoid, int currentPosition, int elementsToSynthesize)
    {
        for (unsigned i = currentPosition - elementsToSynthesize; i < dfspo.size(); i++)
        {
            if (dfspo[i]->type == TreeNodeType::NonTerminal && dfspo[i]->nonTermInstance.id == id && !vector_contains_q(avoid, i))
                return static_cast<int>(i);
        }
        return -1;
    }

    /// <summary>
    /// Find the first position of a Terminal of type id.
    /// </summary>
    /// <param name="dfspo">List of nodes traversed in DepthFirst PostOrder.</param>
    /// <param name="id">Identifier of the terminal.</param>
    /// <param name="avoid">List of positions to avoid.</param>
    /// <param name="currentPosition">Current position of the cursor.</param>
    /// <param name="elementsToSynthesize">Number of nodes left to be synthesized.</param>
    /// <returns>Position as index. If the search fails, returns -1.</returns>
    int  FirstPositionOfID(const std::vector<TreeNode<TerminalType, NonTerminalType>*>& dfspo, TerminalType id,
                           const std::vector<unsigned>& avoid, int currentPosition, int elementsToSynthesize)
    {
        for (unsigned i = currentPosition - elementsToSynthesize; i < dfspo.size(); i++)
        {
            if (dfspo[i]->type == TreeNodeType::Terminal && dfspo[i]->termInstance.id == id && !vector_contains_q(avoid, i))
                return static_cast<int>(i);
        }
        return -1;
    }

    /// <summary>
    /// Synthesize the first non-synthesized node and deletes the consumed nodes.
    /// </summary>
    /// <param name="dfspo">List of nodes traversed in DepthFirst PostOrder.</param>
    void SynthesizeFirst(std::vector<TreeNode<TerminalType, NonTerminalType>*>& dfspo)
    {
        for (unsigned i = 0; i < dfspo.size(); i++)
        {
            if (dfspo[i]->type == TreeNodeType::NonTerminal && !dfspo[i]->IsSynthesized())
            {
                ProductionRule<TerminalType, NonTerminalType> rule = dfspo[i]->generatorPR;

                std::string synthesization;
                std::vector<unsigned> toErase;

                for (SemanticElement<TerminalType, NonTerminalType> se : rule.semanticRules)
                {
                    if (se.type == SemanticElementType::String)
                        synthesization += se.string;
                    if (se.type == SemanticElementType::NonTerminal)
                    {
                        const int pos = FirstPositionOfID(dfspo, se.nonterm.id, toErase, i, rule.ElementsToSynthesize());
                        if (pos != -1)
                        {
                            synthesization += dfspo[pos]->synthesis;
                            toErase.push_back(pos);
                        }
                        else
                        {
                            std::string errorReport = "Could not find any NonTerm node of type " + se.nonterm.label;
                            errorReport += " during synthesis of node with UUID: " + dfspo[i]->uuid;

                            throw std::runtime_error(errorReport);
                        }
                    }
                    if (se.type == SemanticElementType::Terminal)
                    {
                        const int pos = FirstPositionOfID(dfspo, se.term.id, toErase, i, rule.ElementsToSynthesize());
                        if (pos != -1)
                        {
                            synthesization += dfspo[pos]->termValue;
                            toErase.push_back(pos);
                        }
                        else
                            throw std::runtime_error("Could not find any Term node of type " + se.term.label + " during synthesis");
                    }
                }
                dfspo[i]->synthesis = synthesization;
                delete_indexes(dfspo, toErase);
                return;
            }
        }
    }

    /// <summary>
    /// Copy a tree by creating new instances of all the nodes.
    /// </summary>
    /// <param name="copyTree">Pointer to the tree that holds the copy.</param>
    /// <param name="originalTree">Pointer to the original tree that will be copied.</param>
    void CopyTree(TreeNode<TerminalType, NonTerminalType>* copyTree, TreeNode<TerminalType, NonTerminalType>* originalTree)
    {
        if (originalTree != nullptr)
        {
            copyTree->type = originalTree->type;
            copyTree->termInstance = originalTree->termInstance;
            copyTree->termValue = originalTree->termValue;
            copyTree->nonTermInstance = originalTree->nonTermInstance;
            copyTree->generatorPR = originalTree->generatorPR;
            copyTree->synthesis = copyTree->synthesis;

            for (TreeNode<TerminalType, NonTerminalType>* n : originalTree->children)
            {
                auto* copyNode = new TreeNode<TerminalType, NonTerminalType>(n);
                copyTree->AddChildNode(copyNode);
                CopyTree(copyNode, n);
            }
        }
    }

    /// <summary>
    /// Recursive implementation. Create random tree based on the production rules described in the variable grammarRules.
    /// </summary>
    /// <param name="maxDepth">Maximum allowed tree depth.</param>
    /// <param name="depth">Current depth. If while creating a random tree, the depth reaches the maxDepth value, it will fail and return false.</param>
    /// <param name="node">Node from where the random tree will be created.</param>
    /// <returns>True if creation is successful, false if not.</returns>
    bool CreateRandomTree(int maxDepth, int depth, TreeNode<TerminalType, NonTerminalType>* node)
    {
        if (node->type == TreeNodeType::NonTerminal)
        {
            // Create children nodes based on the current node production rule.
            std::vector<TreeNode<TerminalType, NonTerminalType>*> newNodes;
            for (ProductionElement<TerminalType, NonTerminalType> pe : node->generatorPR.to)
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
                for (TreeNode<TerminalType, NonTerminalType>* n : newNodes)
                {
                    bool branchCreationSuccess = CreateRandomTree(maxDepth, depth + 1, n);
                    if (!branchCreationSuccess)
                        return false;
                }
            }
            else
                return false;
        }
        else
        {
            // Select a terminal randomly.
            std::vector<size_t> applicableTerminals = range<size_t>(node->termInstance.values.size());
            const size_t r = *select_randomly(applicableTerminals.begin(), applicableTerminals.end());
            const std::string selectedTerm = node->termInstance.values[r];
            node->termValue = selectedTerm;
        }

        return true;
    }

    /// <summary>
    /// Recursive implementation. Prints the tree in the console.
    /// </summary>
    /// <param name="node">Node to print.</param>
    /// <param name="depth">Current depth.</param>
    void PrintTree(TreeNode<TerminalType, NonTerminalType>* node, int depth, bool showUUID = false)
    {
        for (TreeNode<TerminalType, NonTerminalType>* n : node->children)
        {
            this->PrintNodeAsTree(n, depth, showUUID);
            PrintTree(n, depth + 1, showUUID);
        }
    }

public:
    /**************************************************
    *      CST Construction and status management     *
    **************************************************/

    /// <summary>
    /// Creates an empty ConcreteSyntaxTree.
    /// </summary>
    /// <param name="grammar">The formal grammar of the ConcreteSyntaxTree.</param>
    explicit ConcreteSyntaxTree(Grammar<TerminalType, NonTerminalType> grammar)
    {
        treeGrammar = grammar;
        const NonTerminal<NonTerminalType> rootNonTerm = grammar.GetRoot();
        root = new TreeNode<TerminalType, NonTerminalType>(rootNonTerm);
        root->parent = nullptr;
    }

    /// <summary>
    /// Copy constructor that uses the root of the tree to copy.
    /// </summary>
    /// <param name="grammar">The formal grammar of the ConcreteSyntaxTree.</param>
    /// <param name="proot">Root of the tree to be copied.</param>
    ConcreteSyntaxTree(Grammar<TerminalType, NonTerminalType> grammar, TreeNode<TerminalType, NonTerminalType>* proot)
    {
        treeGrammar = grammar;
        root = new TreeNode<TerminalType, NonTerminalType>(proot);
        root->parent = nullptr;
        this->CopyTree(root, proot);
        this->ClearSynthesis();
    }

    /// <summary>
    /// Copy constructor.
    /// </summary>
    /// <param name="other">ConcreteSyntaxTree to be copied.</param>
    ConcreteSyntaxTree(const ConcreteSyntaxTree<TerminalType, NonTerminalType>& other)
    {
        root = new TreeNode<TerminalType, NonTerminalType>(other.root);
        root->parent = nullptr;
        this->CopyTree(root, other.root);
        this->ClearSynthesis();
    }

    /// <summary>
    /// Copy constructor from reference.
    /// </summary>
    /// <param name="other">Pointer to the ConcreteSyntaxTree to be copied.</param>
    explicit ConcreteSyntaxTree(ConcreteSyntaxTree<TerminalType, NonTerminalType>* other)
    {
        root = new TreeNode<TerminalType, NonTerminalType>(other->root);
        root->parent = nullptr;
        this->CopyTree(root, other->root);
        this->ClearSynthesis();
    }

    ~ConcreteSyntaxTree()
    {
        this->Destroy();
    }

    /// <summary>
    /// Reset the tree to its empty state.
    /// </summary>
    void Reset()
    {
        if (root != nullptr)
        {
            std::vector<TreeNode<TerminalType, NonTerminalType>*> nodeList = DepthFirstScanPostorder();
            for (TreeNode<TerminalType, NonTerminalType>* n : nodeList)
            {
                if (n != nullptr)
                {
                    delete n;
                    n = nullptr;
                }
            }

            const NonTerminal<NonTerminalType> rootNonTerm = treeGrammar.GetRoot();
            root = new TreeNode<TerminalType, NonTerminalType>(rootNonTerm);
            root->parent = nullptr;
        }
    }

    /// <summary>
    /// Destroy the tree by deleting all its nodes.
    /// </summary>
    void Destroy()
    {
        if (root != nullptr)
        {
            std::vector<TreeNode<TerminalType, NonTerminalType>*> nodeList = this->DepthFirstScanPostorder(root);
            for (TreeNode<TerminalType, NonTerminalType>*& n : nodeList)
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

    /// <summary>
    /// Remove the synthesis of every node.
    /// </summary>
    void ClearSynthesis()
    {
        std::vector<TreeNode<TerminalType, NonTerminalType>*> nodeList = this->DepthFirstScanPostorder(root);
        for (TreeNode<TerminalType, NonTerminalType>* n : nodeList)
        {
            if (n->type == TreeNodeType::NonTerminal)
                n->synthesis = "";
        }
    }

    /*************************
    *    Tree manipulation   *
    *************************/

    /// <summary>
    /// Check if the tree is empty.
    /// </summary>
    bool IsEmpty()
    {
        return (root->children.size() == 0);
    }

    /// <summary>
    /// Returns a reference to the root.
    /// </summary>
    /// <returns>Pointer to the root node.</returns>
    TreeNode<TerminalType, NonTerminalType>* GetRoot()
    {
        return root;
    }

    /// <summary>
    /// Add child NonTerminal node to the target.
    /// </summary>
    /// <param name="target">Node where the child will be placed.</param>
    /// <param name="nonTerm">NonTerminal instance.</param>
    /// <param name="generatorPR">Production rule from which this node is part of.</param>
    /// <returns>Pointer to the newly created node.</returns>
    TreeNode<TerminalType, NonTerminalType>* AddNode(TreeNode<TerminalType, NonTerminalType>* target,
                                                     const NonTerminal<NonTerminalType>& nonTerm,
                                                     ProductionRule<TerminalType, NonTerminalType> generatorPR)
    {
        auto* newNode = new TreeNode<TerminalType, NonTerminalType>(nonTerm);
        newNode->generatorPR = generatorPR;
        newNode->parent = target;
        target->AddChildNode(newNode);
        return newNode;
    }

    /// <summary>
    /// Add child Terminal node to the target. If the Terminal has only one possible value, sets it as the termValue.
    /// If not, leaves the value empty.
    /// </summary>
    /// <param name="target">Node where the child will be placed.</param>
    /// <param name="term">Terminal instance.</param>
    /// <returns>Pointer to the newly created node.</returns>
    TreeNode<TerminalType, NonTerminalType>* AddNode(TreeNode<TerminalType, NonTerminalType>* target, 
                                                     const Terminal<TerminalType>& term)
    {
        auto* newNode = new TreeNode<TerminalType, NonTerminalType>(term);
        if (term.values.size() == 1)
            newNode->termValue = term.values.front();

        newNode->parent = target;
        target->AddChildNode(newNode);
        return newNode;
    }

    /// <summary>
    /// Add child Terminal node to the target.
    /// </summary>
    /// <param name="target">Node where the child will be placed.</param>
    /// <param name="term">Terminal instance.</param>
    /// <param name="termValue">Terminal value.</param>
    /// <returns>Pointer to the newly created node.</returns>
    TreeNode<TerminalType, NonTerminalType>* AddNode(TreeNode<TerminalType, NonTerminalType>* target,
                                                     const Terminal<TerminalType>& term,
                                                     std::string termValue)
    {
        if (vector_contains_q(term.values, termValue))
        {
            auto* newNode = new TreeNode<TerminalType, NonTerminalType>(term);
            newNode->termValue = termValue;
            newNode->parent = target;
            target->AddChildNode(newNode);
            return newNode;
        }
        else
            throw std::runtime_error("AddNode: The Terminal doesn't contain the termValue as one of its possible values.");
    }

    /// <summary>
    /// Removes the subtree starting from rootOfSubtree.
    /// </summary>
    /// <param name="rootOfSubtree">Pointer to the root of the subtree to be deleted.</param>
    void RemoveSubtree(TreeNode<TerminalType, NonTerminalType>* rootOfSubtree)
    {
        std::vector<TreeNode<TerminalType, NonTerminalType>*> nodeList = this->DepthFirstScanPostorder(rootOfSubtree);
        nodeList.pop_back();
        for (TreeNode<TerminalType, NonTerminalType>*& n : nodeList)
        {
            if (n != nullptr)
            {
                delete n;
                n = nullptr;
            }
        }
        rootOfSubtree->children.clear();

        ClearSynthesis();
    }

    /// <summary>
    /// Get subtree starting from subTreeStartNode.
    /// </summary>
    /// <param name="subTreeStartNode">Root node of subtree.</param>
    /// <returns>A new ConcreteSyntaxTree starting from the subTreeStartNode.</returns>
    ConcreteSyntaxTree<TerminalType, NonTerminalType> GetSubtree(TreeNode<TerminalType, NonTerminalType>* subTreeStartNode)
    {
        return ConcreteSyntaxTree<TerminalType, NonTerminalType>(treeGrammar, subTreeStartNode);
    }

    /// <summary>
    /// Insert a copy of the subtree into the position at insertNode.
    /// </summary>
    /// <param name="insertNode">Node where the subtree will be inserted.</param>
    /// <param name="subtreeStartNode">Pointer to the subtree to copy and insert.</param>
    void InsertSubtree(TreeNode<TerminalType, NonTerminalType>* insertNode, TreeNode<TerminalType, NonTerminalType>* subtreeStartNode)
    {
        // Check that both nodes are NonTerminals
        if (insertNode->type == TreeNodeType::NonTerminal && subtreeStartNode->type == TreeNodeType::NonTerminal)
        {
            // Check that both nodes are of the same type.
            if (insertNode->nonTermInstance.id == subtreeStartNode->nonTermInstance.id)
            {
                auto* copySubtreStartNode = new TreeNode<TerminalType, NonTerminalType>();
                CopyTree(copySubtreStartNode, subtreeStartNode);

                // Find parent and replace child reference of insertNode to subtreeStartNode.
                for (TreeNode<TerminalType, NonTerminalType>*& child : insertNode->parent->children)
                {
                    if (child == insertNode)
                    {
                        child = copySubtreStartNode;
                        child->parent = insertNode->parent;
                        break;
                    }
                }

                this->ClearSynthesis();
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


    /*********************
    *      Utilities     *
    *********************/

    /// <summary>
    /// Create random tree based on the production rules described in the variable grammarRules.
    /// </summary>
    /// <param name="maxDepth">Maximum allowed tree depth.</param>
    /// <returns>True if creation is successful, false if not.</returns>
    bool CreateRandomTree(int maxDepth = 10)
    {
        root->generatorPR = treeGrammar.GetRandomRootRule();

        // Create children nodes based on the selected rule
        std::vector<TreeNode<TerminalType, NonTerminalType>*> newNodes;
        for (ProductionElement<TerminalType, NonTerminalType> pe : root->generatorPR.to)
        {
            if (pe.type == ProductionElementType::NonTerminal)
                newNodes.push_back(AddNode(root, pe.nonterm, treeGrammar.GetRandomCompatibleRule(pe.nonterm.id)));
            if (pe.type == ProductionElementType::Terminal)
                newNodes.push_back(AddNode(root, pe.term));
            if (pe.type == ProductionElementType::Unassigned)
                throw std::runtime_error("Unassigned production element type");
        }

        for (TreeNode<TerminalType, NonTerminalType>* n : newNodes)
        {
            bool branchCreationSuccess = CreateRandomTree(maxDepth, 1, n);
            if (!branchCreationSuccess)
                return false;
        }

        return true;
    }

    /// <summary>
    /// Ensure the creation of a random tree by creating random trees until there is a success.
    /// </summary>
    /// <param name="maxDepth">Maximum allowed tree depth.</param>
    void CreateRandomTreeSafe(int maxDepth = 10)
    {
        bool success = false;
        while (!success)
        {
            this->Reset();
            success = this->CreateRandomTree(maxDepth);
        }
    }

    /// <summary>
    /// Prints the tree in the console.
    /// </summary>
    void PrintTree(bool showUUID = false)
    {
        this->PrintNodeAsTree(root, 0, showUUID);
        
        for (TreeNode<TerminalType, NonTerminalType>* n : root->children)
        {
            this->PrintNodeAsTree(n, 1, showUUID);
            PrintTree(n, 2, showUUID);
        }
    }

    /**************************
    *     Tree traversals     *
    **************************/

    /// <summary>
    /// Traverses the tree in a depth first pre-order.
    /// </summary>
    /// <param name="node">Start node. The default value is the root of the tree.</param>
    /// <returns>List of references of the traversed nodes.</returns>
    std::vector<TreeNode<TerminalType, NonTerminalType>*> DepthFirstScanPreorder(TreeNode<TerminalType, NonTerminalType>* node = nullptr)
    {
        /*
          Algorithm Preorder(tree)
          1. Visit the root.
          2. Traverse the left subtree, i.e., call Preorder(left-subtree)
          3. Traverse the right subtree, i.e., call Preorder(right-subtree)
        */
        std::vector<TreeNode<TerminalType, NonTerminalType>*> output;

        if (node == nullptr)
        {
            node = root;
            output.push_back(node);
        }

        for (TreeNode<TerminalType, NonTerminalType>* n : node->children)
        {
            output.push_back(n);
            const std::vector<TreeNode<TerminalType, NonTerminalType>*> childrenTreeNodes = DepthFirstScanPreorder(n);
            output.insert(output.end(), childrenTreeNodes.begin(), childrenTreeNodes.end());
        }

        return output;
    }
    
    /// <summary>
    /// Traverses the tree in a depth first post-order.
    /// </summary>
    /// <param name="node">Start node. The default value is the root of the tree.</param>
    /// <returns>List of references of the traversed nodes.</returns>
    std::vector<TreeNode<TerminalType, NonTerminalType>*> DepthFirstScanPostorder(TreeNode<TerminalType, NonTerminalType>* node = nullptr)
    {
        /*
          Algorithm Postorder(tree)
          1. Traverse the left subtree, i.e., call Postorder(left-subtree)
          2. Traverse the right subtree, i.e., call Postorder(right-subtree)
          3. Visit the root.
        */

        std::vector<TreeNode<TerminalType, NonTerminalType>*> output;

        if (node == nullptr)
            node = root;

        if (node->children.empty())
        {
            output.push_back(node);
            return output;
        }

        for (TreeNode<TerminalType, NonTerminalType>* n : node->children)
        {
            const std::vector<TreeNode<TerminalType, NonTerminalType>*> childrenTreeNodes = DepthFirstScanPostorder(n);
            output.insert(output.end(), childrenTreeNodes.begin(), childrenTreeNodes.end());
        }
        output.push_back(node);

        return output;
    }
    
    /// <summary>
    /// Traverses the tree in a breadth first order.
    /// </summary>
    /// <returns>List of references of the traversed nodes.</returns>
    std::vector<TreeNode<TerminalType, NonTerminalType>*> BreadthFirstScan()
    {
        /*
            Algorithm BFS(tree)
            1) Create an empty queue q
            2) temp_node = root
            3) Loop while temp_node is not NULL
                a) print temp_node->data.
                b) Enqueue temp_nodes children (first left then right children) to q
                c) Dequeue a node from q.
        */

        std::vector<TreeNode<TerminalType, NonTerminalType>*> output;
        std::queue<TreeNode<TerminalType, NonTerminalType>*> q;
        q.push(root);

        while (!q.empty())
        {
            TreeNode<TerminalType, NonTerminalType>* node = q.front();
            output.push_back(node);
            q.pop();

            for (TreeNode<TerminalType, NonTerminalType>* n : node->children)
                q.push(n);
        }

        return output;
    }
    
    /**************************
    *      Tree synthesis     *
    **************************/

    /// <summary>
    /// Synthesizes the tree into an expression using the semantic rules of the grammar.
    /// </summary>
    /// <returns>The synthesized expression as a std::string.</returns>
    std::string SynthesizeExpression()
    {
        std::vector<TreeNode<TerminalType, NonTerminalType>*> dfspo = this->DepthFirstScanPostorder();

        try
        {
            while (!AllNodesAreSynthesized(dfspo))
                SynthesizeFirst(dfspo);

            return dfspo.back()->synthesis;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Failed to synthesize expression: " << e.what() << std::endl;
            return "";
        }
    }
};