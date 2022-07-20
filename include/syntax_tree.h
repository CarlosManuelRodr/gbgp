#pragma once
#include <iostream>
#include <ostream>
#include <sstream>
#include <queue>
#include <functional>
#include <string>
#include <exception>
#include <utility>
#include "tree_node.h"

//**********************************************
//*    Concrete syntax tree implementation     *
//*********************************************/

/// Contains the expression tree, provides interfaces for manipulating its structure, synthesizing and evaluating the tree.
class SyntaxTree
{
private:
    /// Root of the tree.
    TreeNode* _root;

    /// Find the first position of a NonTerminal of type id.
    /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
    /// \param id Identifier of the non terminal.
    /// \param avoid List of positions to avoid.
    /// \param currentPosition Current position of the cursor.
    /// \param elementsToSynthesize Number of nodes left to be synthesized.
    /// \return Position as index. If the search fails, returns -1.
    [[nodiscard]]
    static int FindIndexOfNonTerm(const std::vector<TreeNode*>& treeTraversal, int id, const std::vector<unsigned>& avoid,
                                  unsigned currentPosition, int elementsToSynthesize)
    {
        for (unsigned i = currentPosition - elementsToSynthesize; i < treeTraversal.size(); i++)
        {
            if (treeTraversal[i]->type == TreeNodeType::NonTerminal &&
                treeTraversal[i]->nonTermInstance.id == id && !vector_contains_q(avoid, i))
                return static_cast<int>(i);
        }
        return -1;
    }

    /// Find the first position of a term of type id.
    /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
    /// \param id Identifier of the term.
    /// \param avoid List of positions to avoid.
    /// \param currentPosition Current position of the cursor.
    /// \param elementsToSynthesize Number of nodes left to be synthesized.
    /// \return Position as index. If the search fails, returns -1.
    [[nodiscard]]
    static int FindIndexOfTerm(const std::vector<TreeNode*>& treeTraversal, int id, const std::vector<unsigned>& avoid,
                               unsigned currentPosition, int elementsToSynthesize)
    {
        for (unsigned i = currentPosition - elementsToSynthesize; i < treeTraversal.size(); i++)
        {
            if (treeTraversal[i]->type == TreeNodeType::Terminal && treeTraversal[i]->termInstance.id == id &&
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
        _root = nullptr;
    }

    /// Builds a tree from a root node.
    /// \param proot Pointer to the root of the tree.
    /// \param deepCopy If set to true, it will copy the tree into a new instance.
    explicit SyntaxTree(TreeNode* proot, bool deepCopy = false)
    {
        if (deepCopy)
        {
            _root = new TreeNode(proot);
            _root->parent = nullptr;
            this->CopyTree(_root, proot);
        }
        else
            _root = proot;

        this->ClearEvaluation();
    }

    /// Builds a tree from a root node.
    /// \param proot Root of the tree.
    /// \param deepCopy If set to true, it will copy the tree into a new instance.
    explicit SyntaxTree(TreeNode proot, bool deepCopy = false)
    {
        _root = new TreeNode(proot);
        _root->parent = nullptr;
        if (deepCopy)
            this->CopyTree(_root, &proot);
        this->ClearEvaluation();
    }

    /// Copy constructor.
    /// \param other SyntaxTree to be copied.
    SyntaxTree(const SyntaxTree& other)
    {
        _root = new TreeNode(other._root);
        _root->parent = nullptr;
        this->CopyTree(_root, other._root);
        this->ClearEvaluation();
    }

    /// Copy constructor from reference.
    /// \param other Pointer to the SyntaxTree to be copied.
    explicit SyntaxTree(SyntaxTree* other)
    {
        _root = new TreeNode(other->_root);
        _root->parent = nullptr;
        this->CopyTree(_root, other->_root);
        this->ClearEvaluation();
    }

    ~SyntaxTree()
    {
        this->Destroy();
    }

    SyntaxTree& operator=(const SyntaxTree& other)
    {
        // self-assignment check
        if (this == &other)
            return *this;

        _root = new TreeNode(other._root);
        _root->parent = nullptr;
        this->CopyTree(_root, other._root);
        this->ClearEvaluation();

        return *this;
    }

    void SetRootRule(const ProductionRule& startRule)
    {
        _root = new TreeNode(startRule.from);
        _root->parent = nullptr;
        _root->generatorPR = startRule;
    }

    /// Reset the tree to its empty state.
    void Reset()
    {
        if (_root != nullptr)
        {
            std::vector<TreeNode*> nodeList = GetTreeTraversal();
            for (TreeNode* n : nodeList)
            {
                if (n != nullptr)
                {
                    delete n;
                    n = nullptr;
                }
            }

            _root = nullptr;
        }
    }

    /// Destroy the tree by deleting all its nodes.
    void Destroy()
    {
        if (_root != nullptr)
        {
            std::vector<TreeNode*> nodeList = this->GetTreeTraversal(_root);
            for (TreeNode*& n : nodeList)
            {
                if (n != nullptr)
                {
                    delete n;
                    n = nullptr;
                }
            }
            _root = nullptr;
        }
    }

    /// Clear the expressionSynthesis and expressionEvaluation of every node.
    void ClearEvaluation()
    {
        std::vector<TreeNode*> nodeList = this->GetTreeTraversal(_root);
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
        return _root == nullptr;
    }

    /// Returns a reference to the root.
    /// \return Pointer to the root node.
    [[nodiscard]]
    TreeNode* Root() const
    {
        return _root;
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
        std::vector<TreeNode*> nodeList = this->GetTreeTraversal(rootOfSubtree);
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

    /// Prints the tree in the output stream.
    void PrintTree(std::ostream& stream = std::cout) const
    {
        SyntaxTree::PrintNodeAsTree(stream, _root, 0);
        
        for (TreeNode* n : _root->children)
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

    static void DeleteTreeTraversal(const std::vector<TreeNode*>& traversal)
    {
        for (auto node : traversal)
            delete node;
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
        std::vector<TreeNode*> replacementNodes = SyntaxTree::CopyTreeTraversal(replaceTo);

        const unsigned replaceFromLength = replaceFrom.size();
        const unsigned replaceToLength = replaceTo.size();
        const unsigned replaceIndex = SyntaxTree::FindIndexOfTraversalSubsequence(copyNodes, replaceFrom);

        if (replaceIndex == copyNodes.size()) // No subsequence to replace. Return source traversal.
        {
            DeleteTreeTraversal(copyNodes);
            return traversal;
        }

        // Delete and replace.
        copyNodes.erase(copyNodes.begin() + replaceIndex, copyNodes.begin() + replaceIndex + replaceFromLength);
        copyNodes.insert(copyNodes.begin() + replaceIndex, replacementNodes.begin(), replacementNodes.end());

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
    std::vector<TreeNode*> GetTreeTraversal(TreeNode* node = nullptr) const
    {
        /*
          Algorithm Postorder(tree)
          1. Traverse the left subtree, i.e., call Postorder(left-subtree)
          2. Traverse the right subtree, i.e., call Postorder(right-subtree)
          3. Visit the root.
        */
        // TODO: Replace recursive implementation with iterative.
        std::vector<TreeNode*> output;

        if (node == nullptr)
            node = _root;

        if (node->children.empty())
        {
            output.push_back(node);
            return output;
        }

        for (TreeNode* n : node->children)
        {
            const std::vector<TreeNode*> childrenTreeNodes = GetTreeTraversal(n);
            output.insert(output.end(), childrenTreeNodes.begin(), childrenTreeNodes.end());
        }
        output.push_back(node);

        return output;
    }

    /// Find the index of the first non-synthesized non-terminal.
    /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
    /// \return The index if the first non-synthesized non-terminal.
    [[nodiscard]]
    static unsigned NextToBuild(std::vector<TreeNode*>& treeTraversal)
    {
        for (unsigned i = 0; i < treeTraversal.size(); i++)
            if (treeTraversal[i]->type == TreeNodeType::NonTerminal && !treeTraversal[i]->HasChildren())
                return i;
        return treeTraversal.size();
    }

    /// Builds the first non-terminal node with no children.
    /// \param treeTraversal The tree traversal.
    static void BuildFirst(std::vector<TreeNode*>& treeTraversal)
    {
        unsigned nextIndex = NextToBuild(treeTraversal);
        if (nextIndex == treeTraversal.size())
            return;

        TreeNode* nodeToBuild = treeTraversal[nextIndex];
        ProductionRule rule = nodeToBuild->generatorPR;
        std::vector<unsigned> toErase;

        for (const ProductionElement& se : rule.to)
        {
            const int pos = (se.type == ProductionElementType::NonTerminal) ?
                    FindIndexOfNonTerm(treeTraversal, se.nonterm.id, toErase, nextIndex, rule.NumberOfRules()) :
                    FindIndexOfTerm(treeTraversal, se.term.id, toErase, nextIndex, rule.NumberOfRules());

            if (pos != -1)
            {
                nodeToBuild->AddChildNode(treeTraversal[pos], nodeToBuild);
                toErase.push_back(pos);
            }
            else
            {
                if (se.type == ProductionElementType::NonTerminal)
                {
                    std::string errorReport = "Could not find any NonTerm node of type " + se.nonterm.label;
                    errorReport += " during expression evaluation of node: " + treeTraversal[nextIndex]->ToString();
                    throw std::runtime_error(errorReport);
                }
                else if (se.type == ProductionElementType::Terminal)
                    throw std::runtime_error("Could not find any Term node of type " + se.term.label + " during expression evaluation");
            }
        }

        delete_elements_at_index(treeTraversal, toErase);
    }

    /// Builds a SyntaxTree from a depth first post order traversal.
    /// \param treeTraversal The traversal.
    /// \return The tree built from the traversal.
    static SyntaxTree BuildFromTraversal(std::vector<TreeNode*>& treeTraversal)
    {
        while (treeTraversal.size() > 1)
            BuildFirst(treeTraversal);

        return SyntaxTree(treeTraversal.back());
    }

    //***************************
    //*     Tree evaluation     *
    //**************************/

    /// Find the index of the first non-synthesized non-terminal.
    /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
    /// \return The index if the first non-synthesized non-terminal.
    [[nodiscard]]
    static unsigned NextToSynthesize(std::vector<TreeNode*>& treeTraversal)
    {
        for (unsigned i = 0; i < treeTraversal.size(); i++)
            if (treeTraversal[i]->type == TreeNodeType::NonTerminal && !treeTraversal[i]->IsSynthesized())
                return i;
        return treeTraversal.size();
    }

    /// Synthesize the first non-synthesized node and deletes the consumed nodes.
    /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
    static void SynthesizeFirst(std::vector<TreeNode*>& treeTraversal)
    {
        // Get the rule of the element to be evaluated.
        unsigned nextIndex = NextToSynthesize(treeTraversal);
        if (nextIndex == treeTraversal.size())
            return;

        ProductionRule rule = treeTraversal[nextIndex]->generatorPR;
        std::string synthesis;
        std::vector<unsigned> toErase;

        // Process the elements of treeTraversal that match the semantic rules.
        for (const SemanticElement& se : rule.semanticRules)
        {
            if (se.type == SemanticElementType::String)
                synthesis += se.string;
            else if (se.type == SemanticElementType::NonTerminal)
            {
                const int pos = FindIndexOfNonTerm(treeTraversal, se.nonterm.id, toErase, nextIndex, rule.NumberOfRules());
                if (pos != -1)
                {
                    synthesis += treeTraversal[pos]->expressionSynthesis;
                    toErase.push_back(pos);
                }
                else
                {
                    std::string errorReport = "Could not find any NonTerm node of type " + se.nonterm.label;
                    errorReport += " during synthesis of node : " + treeTraversal[nextIndex]->ToString();
                    throw std::runtime_error(errorReport);
                }
            }
            else if (se.type == SemanticElementType::Terminal)
            {
                const int pos = FindIndexOfTerm(treeTraversal, se.term.id, toErase, nextIndex, rule.NumberOfRules());
                if (pos != -1)
                {
                    synthesis += treeTraversal[pos]->termValue;
                    toErase.push_back(pos);
                }
                else
                    throw std::runtime_error("Could not find any Term node of type " + se.term.label + " during expressionSynthesis");
            }
        }

        // Consume and delete
        treeTraversal[nextIndex]->expressionSynthesis = synthesis;
        delete_elements_at_index(treeTraversal, toErase);
    }

    /// Synthesizes the tree into an expression using the semantic rules of the grammar.
    /// \return The synthesized expression as a std::string.
    [[nodiscard]]
    std::string SynthesizeExpression() const
    {
        std::vector<TreeNode*> treeTraversal = this->GetTreeTraversal();
        for (auto node : treeTraversal) node->ClearSynthesis();

        try
        {
            while (treeTraversal.size() > 1)
                SynthesizeFirst(treeTraversal);

            return treeTraversal.back()->expressionSynthesis;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Failed to synthesize expression: " << e.what() << std::endl;
            return "";
        }
    }

    /// Find the index of the first non-evaluated non-terminal.
    /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
    /// \return The index if the first non-evaluated non-terminal.
    [[nodiscard]]
    static unsigned NextToEvaluate(std::vector<TreeNode*>& treeTraversal)
    {
        for (unsigned i = 0; i < treeTraversal.size(); i++)
            if (treeTraversal[i]->type == TreeNodeType::NonTerminal && !treeTraversal[i]->IsEvaluated())
                return i;
        return treeTraversal.size();
    }

    /// Evaluate the first non-evaluate node and deletes the consumed nodes.
    /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
    /// \param evaluationContext reference to the evaluation context.
    static void EvaluateFirst(std::vector<TreeNode*>& treeTraversal, EvaluationContext& evaluationContext)
    {
        // Get the rule of the element to be evaluated.
        unsigned nextIndex = NextToEvaluate(treeTraversal);
        if (nextIndex == treeTraversal.size())
            return;

        ProductionRule rule = treeTraversal[nextIndex]->generatorPR;
        std::vector<unsigned> toErase;

        evaluationContext.Prepare();

        // Push to the context the elements of treeTraversal that match the production rules.
        for (const ProductionElement& se : rule.to)
        {
            if (se.type == ProductionElementType::NonTerminal)
            {
                const int pos = FindIndexOfNonTerm(treeTraversal, se.nonterm.id, toErase, nextIndex, rule.NumberOfRules());
                if (pos != -1)
                {
                    evaluationContext.PushSemanticValue(treeTraversal[pos]->expressionEvaluation);
                    toErase.push_back(pos);
                }
                else
                {
                    std::string errorReport = "Could not find any NonTerm node of type " + se.nonterm.label;
                    errorReport += " during expression evaluation of node: " + treeTraversal[nextIndex]->ToString();
                    throw std::runtime_error(errorReport);
                }
            }
            else if (se.type == ProductionElementType::Terminal)
            {
                const int pos = FindIndexOfTerm(treeTraversal, se.term.id, toErase, nextIndex, rule.NumberOfRules());
                if (pos != -1)
                {
                    evaluationContext.PushSemanticValue(treeTraversal[pos]->termValue);
                    toErase.push_back(pos);
                }
                else
                    throw std::runtime_error("Could not find any Term node of type " + se.term.label + " during expression evaluation");
            }
        }

        // Execute semantic action and delete.
        if (rule.semanticAction != nullptr)
        {
            rule.semanticAction(&evaluationContext);
            treeTraversal[nextIndex]->expressionEvaluation = evaluationContext.result();
        }
        else
            throw std::runtime_error("There is no semantic action for rule " + rule.ToString());

        delete_elements_at_index(treeTraversal, toErase);
    }

    /// Evaluates the tree using the production rules of the grammar.
    /// \param evaluationContext reference to the evaluation context.
    /// \return true if expression was evaluated correctly, false if not.
    bool Evaluate(EvaluationContext& evaluationContext) const
    {
        std::vector<TreeNode*> treeTraversal = this->GetTreeTraversal();
        for (auto node : treeTraversal) node->ClearEvaluation();

        try
        {
            while (treeTraversal.size() > 1)
                EvaluateFirst(treeTraversal, evaluationContext);

            return true;
        }
        catch (std::runtime_error& e)
        {
            std::cout << "Failed to evaluate expression: " << e.what() << std::endl;
            return false;
        }
    }

    /// Evaluates the tree using an external evaluator.
    /// \tparam ReturnType The return type of the evaluator.
    /// \param evaluator The function pointer to the evaluator.
    /// \param result Reference to the variable where the result will be stored.
    /// \return true if expression was evaluated correctly, false if not.
    template<typename ReturnType> bool Evaluate(std::function<ReturnType(std::string)> evaluator, ReturnType& result)
    {
        std::string synthesis = SynthesizeExpression();
        if (synthesis.empty())
            return false;
        else
        {
            result = evaluator(synthesis);
            return true;
        }
    }
};