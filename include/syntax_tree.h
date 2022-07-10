#pragma once
#include <iostream>
#include <ostream>
#include <sstream>
#include <queue>
#include <functional>
#include <string>
#include <exception>
#include <utility>
#include "grammar.h"
#include "tree_node.h"

//**********************************************
//*    Concrete syntax tree implementation     *
//*********************************************/

/// Contains the expression tree, provides interfaces for manipulating its structure, synthesizing and evaluating the tree.
/// \tparam TerminalType enum class with the Terminal IDs specified by the user.
/// \tparam NonTerminalType enum class with the NonTerminal IDs specified by the user.
class SyntaxTree
{
private:
    /// Root of the tree.
    TreeNode* root;

    /// Find the first position of a NonTerminal of type id.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    /// \param id Identifier of the non terminal.
    /// \param avoid List of positions to avoid.
    /// \param currentPosition Current position of the cursor.
    /// \param elementsToSynthesize Number of nodes left to be synthesized.
    /// \return Position as index. If the search fails, returns -1.
    [[nodiscard]]
    static int FindIndexOfNonTerm(const std::vector<TreeNode*>& dfspo, int id, const std::vector<unsigned>& avoid,
                                  unsigned currentPosition, int elementsToSynthesize)
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
    static int FindIndexOfTerm(const std::vector<TreeNode*>& dfspo, int id, const std::vector<unsigned>& avoid,
                               unsigned currentPosition, int elementsToSynthesize)
    {
        for (unsigned i = currentPosition - elementsToSynthesize; i < dfspo.size(); i++)
        {
            if (dfspo[i]->type == TreeNodeType::Terminal && dfspo[i]->termInstance.id == id &&
                !vector_contains_q(avoid, i))
                return static_cast<int>(i);
        }
        return -1;
    }

    /// Copy a tree by creating new instances of all the nodes.
    /// \param copyTree Pointer to the tree that holds the copy.
    /// \param originalTree Pointer to the original tree that will be copied.
    void CopyTree(TreeNode* copyTree, TreeNode* originalTree) const
    {
        if (originalTree != nullptr)
        {
            copyTree->type = originalTree->type;
            copyTree->termInstance = originalTree->termInstance;
            copyTree->termValue = originalTree->termValue;
            copyTree->nonTermInstance = originalTree->nonTermInstance;
            copyTree->generatorPR = originalTree->generatorPR;
            copyTree->expressionSynthesis = copyTree->expressionSynthesis;

            for (TreeNode* n : originalTree->children)
            {
                auto* copyNode = new TreeNode(n);
                copyTree->AddChildNode(copyNode);
                CopyTree(copyNode, n);
            }
        }
    }

    /// Print node in the terminal with the specified depth.
    /// \param stream target stream to print.
    /// \param node current node to print.
    /// \param depth level of indent of the printed node.
    static void PrintNodeAsTree(std::ostream& stream, TreeNode* node, int depth)
    {
        std::string output = "|";
        output.append(depth, '-');
        output += node->GetLabel();
        std::cout << output << std::endl;
    }

    /// <summary>
    /// Recursive implementation. Prints the tree in the output stream.
    /// </summary>
    /// <param name="node">Node to print.</param>
    /// <param name="depth">Current depth.</param>
    void PrintTree(std::ostream& stream, TreeNode* node, int depth) const
    {
        for (TreeNode* n : node->children)
        {
            SyntaxTree::PrintNodeAsTree(stream, n, depth);
            PrintTree(stream, n, depth + 1);
        }
    }

public:
    //***************************************************
    //*     Tree construction and state management      *
    //**************************************************/

    /// Creates an empty SyntaxTree.
    /// \param grammar The formal grammar of the SyntaxTree.
    SyntaxTree()
    {
        root = nullptr;
    }

    /// Builds a tree from a root node.
    /// \param proot Pointer to the root of the tree.
    /// \param deepCopy If set to true, it will copy the tree into a new instance.
    explicit SyntaxTree(TreeNode* proot, bool deepCopy = false)
    {
        if (deepCopy)
        {
            root = new TreeNode(proot);
            root->parent = nullptr;
            this->CopyTree(root, proot);
        }
        else
            root = proot;

        this->ClearEvaluation();
    }

    /// Builds a tree from a root node.
    /// \param proot Root of the tree.
    /// \param deepCopy If set to true, it will copy the tree into a new instance.
    explicit SyntaxTree(TreeNode proot, bool deepCopy = false)
    {
        root = new TreeNode(proot);
        root->parent = nullptr;
        if (deepCopy)
            this->CopyTree(root, &proot);
        this->ClearEvaluation();
    }

    /// Copy constructor.
    /// \param other SyntaxTree to be copied.
    SyntaxTree(const SyntaxTree& other)
    {
        root = new TreeNode(other.root);
        root->parent = nullptr;
        this->CopyTree(root, other.root);
        this->ClearEvaluation();
    }

    /// Copy constructor from reference.
    /// \param other Pointer to the SyntaxTree to be copied.
    explicit SyntaxTree(SyntaxTree* other)
    {
        root = new TreeNode(other->root);
        root->parent = nullptr;
        this->CopyTree(root, other->root);
        this->ClearEvaluation();
    }

    ~SyntaxTree()
    {
        this->Destroy();
    }

    void SetRootRule(const ProductionRule& startRule)
    {
        root = new TreeNode(startRule.from);
        root->parent = nullptr;
        root->generatorPR = startRule;
    }

    /// Reset the tree to its empty state.
    void Reset()
    {
        if (root != nullptr)
        {
            std::vector<TreeNode*> nodeList = DepthFirstScanPostorder();
            for (TreeNode* n : nodeList)
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
            std::vector<TreeNode*> nodeList = this->DepthFirstScanPostorder(root);
            for (TreeNode*& n : nodeList)
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
        std::vector<TreeNode*> nodeList = this->DepthFirstScanPostorder(root);
        for (TreeNode* n : nodeList)
        {
            if (n->type == TreeNodeType::NonTerminal)
            {
                n->expressionSynthesis = "";
                n->expressionEvaluation = "";
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
        return root->children.empty();
    }

    /// Returns a reference to the root.
    /// \return Pointer to the root node.
    [[nodiscard]]
    TreeNode* Root() const
    {
        return root;
    }

    /// Add child NonTerminal node to the target.
    /// \param target Node where the child will be placed.
    /// \param nonTerm NonTerminal instance.
    /// \param generatorPR Production rule from which this node is part of.
    /// \return Pointer to the newly created node.
    static TreeNode* AddNode(TreeNode* target, const NonTerminal& nonTerm, const ProductionRule& generatorPR)
    {
        auto* newNode = new TreeNode(nonTerm);
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
    static TreeNode* AddNode(TreeNode* target, const Terminal& term)
    {
        auto* newNode = new TreeNode(term);
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
    static TreeNode* AddNode(TreeNode* target, const Terminal& term, const std::string& termValue)
    {
        auto* newNode = new TreeNode(term);
        newNode->termValue = termValue;
        newNode->parent = target;
        target->AddChildNode(newNode);
        return newNode;
    }

    /// Removes the subtree starting from rootOfSubtree.
    /// \param rootOfSubtree Pointer to the root of the subtree to be deleted.
    void RemoveSubtree(TreeNode* rootOfSubtree)
    {
        std::vector<TreeNode*> nodeList = this->DepthFirstScanPostorder(rootOfSubtree);
        nodeList.pop_back();
        for (TreeNode*& n : nodeList)
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
    /// \return A new SyntaxTree starting from the subTreeStartNode.
    [[nodiscard]]
    static SyntaxTree GetSubtree(TreeNode* subTreeStartNode)
    {
        return SyntaxTree(subTreeStartNode, true);
    }

    /// Insert a copy of the subtree into the position at insertNode.
    /// \param insertNode Node where the subtree will be inserted.
    /// \param subtreeStartNode Pointer to the subtree to copy and insert.
    void InsertSubtree(TreeNode* insertNode, TreeNode* subtreeStartNode)
    {
        // Check that both nodes are NonTerminals
        if (insertNode->type == TreeNodeType::NonTerminal && subtreeStartNode->type == TreeNodeType::NonTerminal)
        {
            // Check that both nodes are of the same type.
            if (insertNode->nonTermInstance.id == subtreeStartNode->nonTermInstance.id)
            {
                auto* copySubtreeStartNode = new TreeNode();
                CopyTree(copySubtreeStartNode, subtreeStartNode);

                // Find parent and replace child reference of insertNode to subtreeStartNode.
                for (TreeNode*& child : insertNode->parent->children)
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

    /// Recursive implementation. Create random tree based on the production rules described in grammarRules.
    /// \param maxDepth Maximum allowed tree depth.
    /// \param depth Current depth. If while creating a random tree, the depth reaches the maxDepth value, it will fail and return false.
    /// \param node Node from where the random tree will be created.
    /// \return True if creation is successful, false if not.
    bool TryCreateRandomTree(const Grammar& treeGrammar, int maxDepth, int depth, TreeNode* node)
    {
        if (node->type == TreeNodeType::NonTerminal)
        {
            // Create children nodes based on the current node production rule.
            std::vector<TreeNode*> newNodes;
            for (const ProductionElement& pe : node->generatorPR.to)
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
                for (TreeNode* n : newNodes)
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
    bool TryCreateRandomTree(const Grammar& treeGrammar, int maxDepth = 10)
    {
        this->SetRootRule(treeGrammar.GetRootRule());

        // Create children nodes based on the selected rule
        std::vector<TreeNode*> newNodes;
        for (const ProductionElement& pe : root->generatorPR.to)
        {
            if (pe.type == ProductionElementType::NonTerminal)
                newNodes.push_back(AddNode(root, pe.nonterm, treeGrammar.GetRandomCompatibleRule(pe.nonterm.id)));
            if (pe.type == ProductionElementType::Terminal)
                newNodes.push_back(AddNode(root, pe.term));
            if (pe.type == ProductionElementType::Unassigned)
                throw std::runtime_error("Unassigned production element type");
        }

        for (TreeNode* n : newNodes)
        {
            bool branchCreationSuccess = TryCreateRandomTree(treeGrammar, maxDepth, 1, n);
            if (!branchCreationSuccess)
                return false;
        }

        return true;
    }

    /// Ensure the creation of a random tree by creating random trees until there is a success.
    /// \param maxDepth Maximum allowed tree depth.
    void CreateRandomTree(const Grammar& treeGrammar, int maxDepth = 10)
    {
        bool success = false;
        while (!success)
        {
            this->Reset();
            success = this->TryCreateRandomTree(treeGrammar, maxDepth);
        }
    }

    /// Prints the tree in the output stream.
    void PrintTree(std::ostream& stream = std::cout) const
    {
        SyntaxTree::PrintNodeAsTree(stream, root, 0);
        
        for (TreeNode* n : root->children)
        {
            SyntaxTree::PrintNodeAsTree(stream, n, 1);
            PrintTree(stream, n, 2);
        }
    }

    /// Performs a shallow copy of each node.
    /// \param other The traversal of the tree to copy.
    /// \return The traversal of the copied tree.
    static std::vector<TreeNode*> CopyTreeTraversal(const std::vector<TreeNode*>& other)
    {
        std::vector<TreeNode*> copyNodes;
        for (auto node : other)
            copyNodes.push_back(TreeNode::ShallowCopy(node));
        return copyNodes;
    }

    /// Find the index of a traversal subsequence inside another traversal.
    /// \param traversal The traversal to perform the search on.
    /// \param subsequence The traversal subsequence to find.
    /// \return The distance to the subsequence if found, else the last index of traversal.
    static unsigned FindIndexOfTraversalSubsequence(const std::vector<TreeNode*>& traversal,
                                                    const std::vector<TreeNode*>& subsequence)
    {
        auto it =  std::search(traversal.begin(), traversal.end(),
                                          subsequence.begin(), subsequence.end(),
                                          [](TreeNode* n1, TreeNode* n2) { return n1->SameID(*n2); });

        return std::distance(traversal.begin(), it);
    }

    /// Replaces a traversal subsequence. Used for pruning the tree.
    /// \param traversal The traversal to perform the replacement.
    /// \param replaceFrom The source replacement sequence.
    /// \param replaceTo The target replacement sequence.
    /// \return A new traversal with the replaced sequence.
    static std::vector<TreeNode*> ReplaceTraversalSubsequence(const std::vector<TreeNode*>& traversal,
                                                              const std::vector<TreeNode*>& replaceFrom,
                                                              const std::vector<TreeNode*>& replaceTo)
    {
        std::vector<TreeNode*> copyNodes = SyntaxTree::CopyTreeTraversal(traversal);

        const unsigned replaceFromLength = replaceFrom.size();
        const unsigned replaceToLength = replaceTo.size();
        const unsigned replaceIndex = SyntaxTree::FindIndexOfTraversalSubsequence(copyNodes, replaceFrom);

        // Delete and replace.
        copyNodes.erase(copyNodes.begin() + replaceIndex, copyNodes.begin() + replaceIndex + replaceFromLength);
        copyNodes.insert(copyNodes.begin() + replaceIndex, replaceTo.begin(), replaceTo.end());

        // Transfer values.
        for (unsigned i = replaceIndex; i < replaceIndex + replaceToLength; i++)
        {
            for (unsigned j = replaceIndex; j < replaceIndex + replaceFromLength; j++)
            {
                if (copyNodes[i]->SameID(traversal[j]))
                {
                    copyNodes[i]->termValue = traversal[j]->termValue;
                    break;
                }
            }
        }

        return copyNodes;
    }

    //***************************
    //*     Tree traversals     *
    //**************************/

    /// Traverses the tree in a depth first post-order.
    /// \param node Start node. The default value is the root of the tree.
    /// \return List of references of the traversed nodes.
    std::vector<TreeNode*> DepthFirstScanPostorder(TreeNode* node = nullptr) const
    {
        /*
          Algorithm Postorder(tree)
          1. Traverse the left subtree, i.e., call Postorder(left-subtree)
          2. Traverse the right subtree, i.e., call Postorder(right-subtree)
          3. Visit the root.
        */

        std::vector<TreeNode*> output;

        if (node == nullptr)
            node = root;

        if (node->children.empty())
        {
            output.push_back(node);
            return output;
        }

        for (TreeNode* n : node->children)
        {
            const std::vector<TreeNode*> childrenTreeNodes = DepthFirstScanPostorder(n);
            output.insert(output.end(), childrenTreeNodes.begin(), childrenTreeNodes.end());
        }
        output.push_back(node);

        return output;
    }

    /// Find the index of the first non-synthesized non-terminal.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    /// \return The index if the first non-synthesized non-terminal.
    [[nodiscard]]
    static unsigned NextToBuild(std::vector<TreeNode*>& dfspo)
    {
        for (unsigned i = 0; i < dfspo.size(); i++)
            if (dfspo[i]->type == TreeNodeType::NonTerminal && !dfspo[i]->HasChildren())
                return i;
        return dfspo.size();
    }

    /// Builds the first non-terminal node with no children.
    /// \param dfspo The tree traversal.
    static void BuildFirst(std::vector<TreeNode*>& dfspo)
    {
        unsigned nextIndex = NextToBuild(dfspo);
        if (nextIndex == dfspo.size())
            return;

        TreeNode* nodeToBuild = dfspo[nextIndex];
        ProductionRule rule = dfspo[nextIndex]->generatorPR;
        std::vector<unsigned> toErase;

        for (const ProductionElement& se : rule.to)
        {
            const int pos = (se.type == ProductionElementType::NonTerminal) ?
                    FindIndexOfNonTerm(dfspo, se.nonterm.id, toErase, nextIndex, rule.NumberOfRules()) :
                    FindIndexOfTerm(dfspo, se.term.id, toErase, nextIndex, rule.NumberOfRules());

            if (pos != -1)
            {
                nodeToBuild->AddChildNode(dfspo[pos], nodeToBuild);
                toErase.push_back(pos);
            }
            else
            {
                if (se.type == ProductionElementType::NonTerminal)
                {
                    std::string errorReport = "Could not find any NonTerm node of type " + se.nonterm.label;
                    errorReport += " during expression evaluation of node: " + dfspo[nextIndex]->ToString();
                    throw std::runtime_error(errorReport);
                }
                else if (se.type == ProductionElementType::Terminal)
                    throw std::runtime_error("Could not find any Term node of type " + se.term.label + " during expression evaluation");
            }
        }

        delete_indexes(dfspo, toErase);
    }

    /// Builds a SyntaxTree from a depth first post order traversal.
    /// \param dfspo The traversal.
    /// \return The tree built from the traversal.
    static SyntaxTree BuildFromTraversal(std::vector<TreeNode*>& dfspo)
    {
        std::vector<TreeNode*> copyNodes = CopyTreeTraversal(dfspo);

        while (copyNodes.size() > 1)
            BuildFirst(copyNodes);

        return SyntaxTree(copyNodes.back());
    }

    //***************************
    //*     Tree evaluation     *
    //**************************/

    /// Find the index of the first non-synthesized non-terminal.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    /// \return The index if the first non-synthesized non-terminal.
    [[nodiscard]]
    static unsigned NextToSynthesize(std::vector<TreeNode*>& dfspo)
    {
        for (unsigned i = 0; i < dfspo.size(); i++)
            if (dfspo[i]->type == TreeNodeType::NonTerminal && !dfspo[i]->IsSynthesized())
                return i;
        return dfspo.size();
    }

    /// Synthesize the first non-synthesized node and deletes the consumed nodes.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    static void SynthesizeFirst(std::vector<TreeNode*>& dfspo)
    {
        // Get the rule of the element to be evaluated.
        unsigned nextIndex = NextToSynthesize(dfspo);
        if (nextIndex == dfspo.size())
            return;

        ProductionRule rule = dfspo[nextIndex]->generatorPR;
        std::string synthesis;
        std::vector<unsigned> toErase;

        // Process the elements of dfspo that match the semantic rules.
        for (const SemanticElement& se : rule.semanticRules)
        {
            if (se.type == SemanticElementType::String)
                synthesis += se.string;
            else if (se.type == SemanticElementType::NonTerminal)
            {
                const int pos = FindIndexOfNonTerm(dfspo, se.nonterm.id, toErase, nextIndex, rule.NumberOfRules());
                if (pos != -1)
                {
                    synthesis += dfspo[pos]->expressionSynthesis;
                    toErase.push_back(pos);
                }
                else
                {
                    std::string errorReport = "Could not find any NonTerm node of type " + se.nonterm.label;
                    errorReport += " during synthesis of node : " + dfspo[nextIndex]->ToString();
                    throw std::runtime_error(errorReport);
                }
            }
            else if (se.type == SemanticElementType::Terminal)
            {
                const int pos = FindIndexOfTerm(dfspo, se.term.id, toErase, nextIndex, rule.NumberOfRules());
                if (pos != -1)
                {
                    synthesis += dfspo[pos]->termValue;
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
    std::string SynthesizeExpression() const
    {
        std::vector<TreeNode*> dfspo = this->DepthFirstScanPostorder();
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

    /// Find the index of the first non-evaluated non-terminal.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    /// \return The index if the first non-evaluated non-terminal.
    [[nodiscard]]
    static unsigned NextToEvaluate(std::vector<TreeNode*>& dfspo)
    {
        for (unsigned i = 0; i < dfspo.size(); i++)
            if (dfspo[i]->type == TreeNodeType::NonTerminal && !dfspo[i]->IsEvaluated())
                return i;
        return dfspo.size();
    }

    /// Evaluate the first non-evaluate node and deletes the consumed nodes.
    /// \param dfspo List of nodes traversed in DepthFirst PostOrder.
    /// \param evaluationContext pointer to the evaluation context.
    static void EvaluateFirst(std::vector<TreeNode*>& dfspo, EvaluationContext* evaluationContext)
    {
        // Get the rule of the element to be evaluated.
        unsigned nextIndex = NextToEvaluate(dfspo);
        if (nextIndex == dfspo.size())
            return;

        ProductionRule rule = dfspo[nextIndex]->generatorPR;
        std::vector<unsigned> toErase;

        evaluationContext->Prepare();

        // Push to the context the elements of dfspo that match the production rules.
        for (const ProductionElement& se : rule.to)
        {
            if (se.type == ProductionElementType::NonTerminal)
            {
                const int pos = FindIndexOfNonTerm(dfspo, se.nonterm.id, toErase, nextIndex, rule.NumberOfRules());
                if (pos != -1)
                {
                    evaluationContext->PushSemanticValue(dfspo[pos]->expressionEvaluation);
                    toErase.push_back(pos);
                }
                else
                {
                    std::string errorReport = "Could not find any NonTerm node of type " + se.nonterm.label;
                    errorReport += " during expression evaluation of node: " + dfspo[nextIndex]->ToString();
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
    bool Evaluate(EvaluationContext* evaluationContext) const
    {
        std::vector<TreeNode*> dfspo = this->DepthFirstScanPostorder();
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