#pragma once
#include <iostream>
#include <ostream>
#include <queue>
#include "grammar.h"

/****************************
*    Node implementation    *
****************************/

/// A node can be either a Terminal or a NonTerminal. A None type is provided for convenience.
enum class TreeNodeType
{
    None, NonTerminal, Terminal
};

/// Represents a node of an n-ary tree
/// \param TerminalType enum class with the Terminal IDs specified by the user.
/// \param NonTerminalType enum class with the NonTerminal IDs specified by the user.
template <typename TerminalType, typename NonTerminalType> class TreeNode
{
private:
    /// Generate an Unique Identifier for this node.
    /// \return A std::string containing the generated UUID.
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
    /// Type of the node.
    TreeNodeType type;

    /// NonTerminal instance used if the node is of NonTerminal type.
    NonTerminal<NonTerminalType> nonTermInstance;

    /// Terminal instance used if the node is of Terminal type.
    Terminal<TerminalType> termInstance;

    /// Production rule from which this node is part of.
    ProductionRule<TerminalType, NonTerminalType> generatorPR;

    /// Parent of the node. If the node is a root, its value will be null.
    TreeNode<TerminalType, NonTerminalType>* parent;

    /// List of references to the children of this node.
    std::vector<TreeNode<TerminalType, NonTerminalType>*> children;

    /// Result of the synthesis of this node.
    std::string synthesis;

    /// Value of the terminal used if the node is of Terminal type.
    std::string termValue;

    /// Unique identifier of this node.
    std::string uuid;

    /// Constructor of an empty node.
    TreeNode()
    {
        type = TreeNodeType::None;
        termInstance = Terminal<TerminalType>();
        nonTermInstance = NonTerminal<NonTerminalType>();
        synthesis = "";
        parent = nullptr;
        uuid = GenerateUUID();
    }

    /// Constructor of a NonTerminal node.
    explicit TreeNode(const NonTerminal<NonTerminalType>& nt)
    {
        type = TreeNodeType::NonTerminal;
        nonTermInstance = nt;
        termInstance = Terminal<TerminalType>();
        synthesis = "";
        parent = nullptr;
        uuid = GenerateUUID();
    }

    /// Constructor of a Terminal node.
    explicit TreeNode(const Terminal<TerminalType>& t)
    {
        type = TreeNodeType::Terminal;
        nonTermInstance = NonTerminal<NonTerminalType>();
        termInstance = t;
        synthesis = "";
        parent = nullptr;
        uuid = GenerateUUID();
    }

    /// Copy constructor.
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

    /// Copy constructor by reference.
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

    /// Check if this node has been synthesized.
    bool IsSynthesized()
    {
        return !synthesis.empty();
    }

    /// Add the node as a child.
    /// \param node Reference to the child node.
    void AddChildNode(TreeNode<TerminalType, NonTerminalType>* node)
    {
        children.push_back(node);
    }

    /// Returns a formatted label of the node.
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
*    CST Implementation     *
****************************/

/// Contains the expression tree, provides interfaces for manipulating its structure and synthesize the expression into a std::string.
/// \tparam TerminalType enum class with the Terminal IDs specified by the user.
/// \tparam NonTerminalType enum class with the NonTerminal IDs specified by the user.
template <typename TerminalType, typename NonTerminalType> class ConcreteSyntaxTree
{
private:
    /// Root of the tree.
    TreeNode<TerminalType, NonTerminalType>* root;

    /// Formal grammar that this instance of ConcreteSyntaxTree uses.
    Grammar<TerminalType, NonTerminalType> treeGrammar;

    /// Check if all the nodes have been synthesized.
    bool AllNodesAreSynthesized(const std::vector<TreeNode<TerminalType, NonTerminalType>*>& nodes)
    {
        for (TreeNode<TerminalType, NonTerminalType>* n : nodes)
        {
            if (n->type == TreeNodeType::NonTerminal && !n->IsSynthesized())
                return false;
        }
        return true;
    }

    /// Print node in the terminal with the specified depth.
    void PrintNodeAsTree(TreeNode<TerminalType, NonTerminalType>* node, int depth, bool showUUID = false)
    {
        std::string output = "|";
        output.append(depth, '-');
        output += node->GetLabel(showUUID);
        std::cout << output << std::endl;
    }

    /// Find the first position of a NonTerminal of type id.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    /// \param id Identifier of the non terminal.
    /// \param avoid List of positions to avoid.
    /// \param currentPosition Current position of the cursor.
    /// \param elementsToSynthesize Number of nodes left to be synthesized.
    /// \return Position as index. If the search fails, returns -1.
    int FirstPositionOfID(const std::vector<TreeNode<TerminalType, NonTerminalType>*>& dfspo, NonTerminalType id,
                          const std::vector<unsigned>& avoid, int currentPosition, int elementsToSynthesize)
    {
        for (unsigned i = currentPosition - elementsToSynthesize; i < dfspo.size(); i++)
        {
            if (dfspo[i]->type == TreeNodeType::NonTerminal &&
                dfspo[i]->nonTermInstance.id == id && !vector_contains_q(avoid, i))
                return static_cast<int>(i);
        }
        return -1;
    }

    /// Find the first position of a Terminal of type id.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    /// \param id Identifier of the terminal.
    /// \param avoid List of positions to avoid.
    /// \param currentPosition Current position of the cursor.
    /// \param elementsToSynthesize Number of nodes left to be synthesized.
    /// \return Position as index. If the search fails, returns -1.
    int  FirstPositionOfID(const std::vector<TreeNode<TerminalType, NonTerminalType>*>& dfspo, TerminalType id,
                           const std::vector<unsigned>& avoid, int currentPosition, int elementsToSynthesize)
    {
        for (unsigned i = currentPosition - elementsToSynthesize; i < dfspo.size(); i++)
        {
            if (dfspo[i]->type == TreeNodeType::Terminal && dfspo[i]->termInstance.id == id &&
                !vector_contains_q(avoid, i))
                return static_cast<int>(i);
        }
        return -1;
    }

    /// Find the index of the first non-synthesized non-terminal.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    /// \return The index if the first non-synthesized non-terminal.
    unsigned NextToSynthesize(std::vector<TreeNode<TerminalType, NonTerminalType>*>& dfspo)
    {
        for (unsigned i = 0; i < dfspo.size(); i++)
            if (dfspo[i]->type == TreeNodeType::NonTerminal && !dfspo[i]->IsSynthesized())
                return i;
        return dfspo.size();
    }

    /// Synthesize the first non-synthesized node and deletes the consumed nodes.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    void SynthesizeFirst(std::vector<TreeNode<TerminalType, NonTerminalType>*>& dfspo)
    {
        unsigned nextIndex = NextToSynthesize(dfspo);
        if (nextIndex == dfspo.size())
            return;

        ProductionRule<TerminalType, NonTerminalType> rule = dfspo[nextIndex]->generatorPR;
        std::string synthesis;
        std::vector<unsigned> toErase;

        for (SemanticElement<TerminalType, NonTerminalType> se : rule.semanticRules)
        {
            if (se.type == SemanticElementType::String)
                synthesis += se.string;
            if (se.type == SemanticElementType::NonTerminal)
            {
                const int pos = FirstPositionOfID(dfspo, se.nonterm.id, toErase, nextIndex, rule.ElementsToSynthesize());
                if (pos != -1)
                {
                    synthesis += dfspo[pos]->synthesis;
                    toErase.push_back(pos);
                }
                else
                {
                    std::string errorReport = "Could not find any NonTerm node of type " + se.nonterm.label;
                    errorReport += " during synthesis of node with UUID: " + dfspo[nextIndex]->uuid;

                    throw std::runtime_error(errorReport);
                }
            }
            if (se.type == SemanticElementType::Terminal)
            {
                const int pos = FirstPositionOfID(dfspo, se.term.id, toErase, nextIndex, rule.ElementsToSynthesize());
                if (pos != -1)
                {
                    synthesis += dfspo[pos]->termValue;
                    toErase.push_back(pos);
                }
                else
                    throw std::runtime_error("Could not find any Term node of type " + se.term.label + " during synthesis");
            }
        }
        dfspo[nextIndex]->synthesis = synthesis;
        delete_indexes(dfspo, toErase);
    }

    /// Copy a tree by creating new instances of all the nodes.
    /// \param copyTree Pointer to the tree that holds the copy.
    /// \param originalTree Pointer to the original tree that will be copied.
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

    /// Recursive implementation. Create random tree based on the production rules described in the variable grammarRules.
    /// \param maxDepth Maximum allowed tree depth.
    /// \param depth Current depth. If while creating a random tree, the depth reaches the maxDepth value, it will fail and return false.
    /// \param node Node from where the random tree will be created.
    /// \return True if creation is successful, false if not.
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

    /// Creates an empty ConcreteSyntaxTree.
    /// \param grammar The formal grammar of the ConcreteSyntaxTree.
    explicit ConcreteSyntaxTree(Grammar<TerminalType, NonTerminalType> grammar)
    {
        treeGrammar = grammar;
        const NonTerminal<NonTerminalType> rootNonTerm = grammar.GetRoot();
        root = new TreeNode<TerminalType, NonTerminalType>(rootNonTerm);
        root->parent = nullptr;
    }

    /// Copy constructor that uses the root of the tree to copy.
    /// \param grammar The formal grammar of the ConcreteSyntaxTree.
    /// \param proot Root of the tree to be copied.
    ConcreteSyntaxTree(Grammar<TerminalType, NonTerminalType> grammar, TreeNode<TerminalType, NonTerminalType>* proot)
    {
        treeGrammar = grammar;
        root = new TreeNode<TerminalType, NonTerminalType>(proot);
        root->parent = nullptr;
        this->CopyTree(root, proot);
        this->ClearSynthesis();
    }

    /// Copy constructor.
    /// \param other ConcreteSyntaxTree to be copied.
    ConcreteSyntaxTree(const ConcreteSyntaxTree<TerminalType, NonTerminalType>& other)
    {
        root = new TreeNode<TerminalType, NonTerminalType>(other.root);
        root->parent = nullptr;
        this->CopyTree(root, other.root);
        this->ClearSynthesis();
    }

    /// Copy constructor from reference.
    /// \param other Pointer to the ConcreteSyntaxTree to be copied.
    [[maybe_unused]] explicit ConcreteSyntaxTree(ConcreteSyntaxTree<TerminalType, NonTerminalType>* other)
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

    /// Reset the tree to its empty state.
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

    /// Destroy the tree by deleting all its nodes.
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

    /// Remove the synthesis of every node.
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

    /// Check if the tree is empty.
    [[maybe_unused]] bool IsEmpty()
    {
        return (root->children.size() == 0);
    }

    /// Returns a reference to the root.
    /// \return Pointer to the root node.
    TreeNode<TerminalType, NonTerminalType>* GetRoot()
    {
        return root;
    }

    /// Add child NonTerminal node to the target.
    /// \param target Node where the child will be placed.
    /// \param nonTerm NonTerminal instance.
    /// \param generatorPR Production rule from which this node is part of.
    /// \return Pointer to the newly created node.
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

    /// Add child Terminal node to the target. If the Terminal has only one possible value, sets it as the termValue.
    /// If not, leaves the value empty.
    /// \param target Node where the child will be placed.
    /// \param term Terminal instance.
    /// \return Pointer to the newly created node.
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

    /// Add child Terminal node to the target.
    /// \param target Node where the child will be placed.
    /// \param term Terminal instance.
    /// \param termValue Terminal value.
    /// \return Pointer to the newly created node.
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

    /// Removes the subtree starting from rootOfSubtree.
    /// \param rootOfSubtree Pointer to the root of the subtree to be deleted.
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

    /// Get subtree starting from subTreeStartNode.
    /// \param subTreeStartNode Root node of subtree.
    /// \return A new ConcreteSyntaxTree starting from the subTreeStartNode.
    ConcreteSyntaxTree<TerminalType, NonTerminalType> GetSubtree(TreeNode<TerminalType, NonTerminalType>* subTreeStartNode)
    {
        return ConcreteSyntaxTree<TerminalType, NonTerminalType>(treeGrammar, subTreeStartNode);
    }

    /// Insert a copy of the subtree into the position at insertNode.
    /// \param insertNode">Node where the subtree will be inserted.
    /// \param subtreeStartNode">Pointer to the subtree to copy and insert.
    void InsertSubtree(TreeNode<TerminalType, NonTerminalType>* insertNode, TreeNode<TerminalType, NonTerminalType>* subtreeStartNode)
    {
        // Check that both nodes are NonTerminals
        if (insertNode->type == TreeNodeType::NonTerminal && subtreeStartNode->type == TreeNodeType::NonTerminal)
        {
            // Check that both nodes are of the same type.
            if (insertNode->nonTermInstance.id == subtreeStartNode->nonTermInstance.id)
            {
                auto* copySubtreeStartNode = new TreeNode<TerminalType, NonTerminalType>();
                CopyTree(copySubtreeStartNode, subtreeStartNode);

                // Find parent and replace child reference of insertNode to subtreeStartNode.
                for (TreeNode<TerminalType, NonTerminalType>*& child : insertNode->parent->children)
                {
                    if (child == insertNode)
                    {
                        child = copySubtreeStartNode;
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

    /// Create random tree based on the production rules described in the variable grammarRules.
    /// \param maxDepth Maximum allowed tree depth.
    /// \return True if creation is successful, false if not.
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

    /// Ensure the creation of a random tree by creating random trees until there is a success.
    /// \param maxDepth">Maximum allowed tree depth.
    void CreateRandomTreeSafe(int maxDepth = 10)
    {
        bool success = false;
        while (!success)
        {
            this->Reset();
            success = this->CreateRandomTree(maxDepth);
        }
    }

    /// Prints the tree in the console.
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

    /// Traverses the tree in a depth first pre-order.
    /// \param node Start node. The default value is the root of the tree.
    /// \return List of references of the traversed nodes.
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

    /// Traverses the tree in a depth first post-order.
    /// \param node Start node. The default value is the root of the tree.
    /// \return List of references of the traversed nodes.
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

    /// Traverses the tree in a breadth first order.
    /// \return List of references of the traversed nodes.
    [[maybe_unused]] std::vector<TreeNode<TerminalType, NonTerminalType>*> BreadthFirstScan()
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

    /// Synthesizes the tree into an expression using the semantic rules of the grammar.
    /// \return The synthesized expression as a std::string.
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