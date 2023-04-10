#pragma once
#include <iostream>
#include <ostream>
#include <sstream>
#include <queue>
#include <functional>
#include <string>
#include <exception>
#include <utility>
#include <optional>
#include "graph.h"

namespace gbgp
{
    /// Alias for a traversal that consists of a list of traversed nodes.
    using Traversal = std::vector<TreeNode*>;

    /// Contains the expression tree, provides interfaces for manipulating its structure, synthesizing and evaluating
    /// the tree.
    class SyntaxTree
    {
    private:
        /// Root of the tree.
        TreeNode* _root{};

        /// Find the first position of a NonTerminal of type id.
        /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
        /// \param id Identifier of the non terminal.
        /// \param avoid List of positions to avoid.
        /// \param currentPosition Current position of the cursor.
        /// \param elementsToSynthesize Number of nodes left to be synthesized.
        /// \return Position as index. If the search fails, returns nullopt.
        [[nodiscard]]
        static std::optional<size_t> FindIndexOfNonTerm(const Traversal& treeTraversal, int id,
                                                        const std::vector<size_t>& avoid, unsigned currentPosition,
                                                        int elementsToSynthesize)
        {
            for (size_t i = currentPosition - elementsToSynthesize; i < treeTraversal.size(); i++)
            {
                if (treeTraversal[i]->type == NodeType::NonTerminal &&
                    treeTraversal[i]->nonTermInstance.id == id && !vector_contains_q(avoid, i))
                    return i;
            }
            return std::nullopt;
        }

        /// Find the first position of a term of type id.
        /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
        /// \param id Identifier of the term.
        /// \param avoid List of positions to avoid.
        /// \param currentPosition Current position of the cursor.
        /// \param elementsToSynthesize Number of nodes left to be synthesized.
        /// \return Position as index. If the search fails, returns nullopt.
        [[nodiscard]]
        static std::optional<size_t> FindIndexOfTerm(const Traversal& treeTraversal, int id,
                                                     const std::vector<size_t>& avoid, unsigned currentPosition,
                                                     int elementsToSynthesize)
        {
            for (size_t i = currentPosition - elementsToSynthesize; i < treeTraversal.size(); i++)
            {
                if (treeTraversal[i]->type == NodeType::Terminal && treeTraversal[i]->termInstance.id == id &&
                    !vector_contains_q(avoid, i))
                    return i;
            }
            return std::nullopt;
        }

        /// Print node in the terminal with the specified depth.
        /// \param stream target stream to print.
        /// \param node current node to print.
        /// \param depth level of indent of the printed node.
        static void PrintNodeAsTree(std::ostream& stream, TreeNode* node, int depth)
        {
            std::string output = "|";

            if (depth > 2)
            {
                output.append(depth - 2, '-');
                output.append("|-");
            }
            else if (depth == 2)
                output.append("-|-");
            else if (depth == 1)
                output.append("-");

            output += node->GetLabel();
            stream << output << "\n";
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

        /// Internal static method to generate the Depth-First PreOrder traversal.
        /// \param node The node.
        /// \param isRoot Is node the root?
        /// \return The traversal.
        static Traversal GetPreOrderTreeTraversal(TreeNode* node, bool isRoot)
        {
            /*
              Algorithm Preorder(tree)
              1. Visit the root.
              2. Traverse the left subtree, i.e., call Preorder(left-subtree)
              3. Traverse the right subtree, i.e., call Preorder(right-subtree)
            */
            // TODO: Replace recursive implementation with iterative.
            Traversal output;

            if (isRoot)
                output.push_back(node);

            for (TreeNode* n : node->children)
            {
                output.push_back(n);
                const Traversal childrenTreeNodes = GetPreOrderTreeTraversal(n, false);
                output.insert(output.end(), childrenTreeNodes.begin(), childrenTreeNodes.end());
            }

            return output;
        }

    public:
        //***************************************************
        //*     Tree construction and state management      *
        //**************************************************/

        /// Creates an empty SyntaxTree.
        SyntaxTree()
        {
            _root = nullptr;
        }

        /// Builds a tree from a root node.
        /// \param root Pointer to the root of the tree.
        explicit SyntaxTree(TreeNode* root)
        {
            _root = root;
            _root->parent = nullptr;
            this->ClearEvaluation();
        }

        /// Builds a tree from a root node.
        /// \param root Root of the tree.
        explicit SyntaxTree(const TreeNode& root)
        {
            _root = new TreeNode(root);
            _root->parent = nullptr;
            this->ClearEvaluation();
        }

        /// Copy constructor.
        /// \param other SyntaxTree to be copied.
        SyntaxTree(const SyntaxTree& other)
        {
            _root = new TreeNode(*other._root);
            _root->parent = nullptr;
            this->ClearEvaluation();
        }

        /// Graph constructor.
        /// \param graph The graph container.
        explicit SyntaxTree(const Graph& graph)
        {
            _root = FromGraph(graph);
        }

        ~SyntaxTree()
        {
            this->Destroy();
        }

        SyntaxTree& operator=(const SyntaxTree& other)
        {
            // Self-assignment check
            if (this == &other)
                return *this;

            _root = new TreeNode(*other._root);
            _root->parent = nullptr;
            this->ClearEvaluation();

            return *this;
        }

        /// Destroy the tree by deleting all its nodes.
        void Destroy()
        {
            if (_root != nullptr)
            {
                Traversal nodeList = GetPostOrderTreeTraversal(_root);
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
        void ClearEvaluation() const
        {
            Traversal nodeList = this->GetPostOrderTreeTraversal();
            for (TreeNode* n : nodeList)
            {
                if (n->type == NodeType::NonTerminal)
                {
                    n->expressionSynthesis = "";
                    n->expressionEvaluation = "";
                }
            }
        }

        //**************************
        //*    Tree manipulation   *
        //*************************/

        /// Set the production rule of the root node.
        /// \param startRule The production rule.
        void SetRootRule(const ProductionRule& startRule)
        {
            _root = new TreeNode(startRule.from);
            _root->parent = nullptr;
            _root->generatorPR = startRule;
        }

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

        /// Return the root node instance.
        [[nodiscard]]
        TreeNode GetRoot() const
        {
            return *_root;
        }

        /// Set the root node.
        /// \param rootNode Pointer to the root node.
        void SetRoot(TreeNode* rootNode)
        {
            if (_root != nullptr)
                Destroy();

            _root = rootNode;
        }

        /// Removes the subtree starting from rootOfSubtree.
        /// \param rootOfSubtree Pointer to the root of the subtree to be deleted.
        void DeleteSubtree(TreeNode* rootOfSubtree) const
        {
            Traversal nodeList = SyntaxTree::GetPostOrderTreeTraversal(rootOfSubtree);
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
        static SyntaxTree CopySubtree(TreeNode* subTreeStartNode)
        {
            auto* subtreeRoot = new TreeNode(*subTreeStartNode);
            subtreeRoot->parent = nullptr;
            return SyntaxTree(subtreeRoot);
        }

        /// Insert a copy of the subtree into the position at insertNode.
        /// \param insertNode Node where the subtree will be inserted.
        /// \param subtreeStartNode Pointer to the subtree to copy and insert.
        void InsertSubtree(TreeNode* insertNode, TreeNode* subtreeStartNode) const
        {
            // Check that both nodes are NonTerminals
            if (insertNode->type == NodeType::NonTerminal && subtreeStartNode->type == NodeType::NonTerminal)
            {
                // Check that both nodes are of the same type.
                if (insertNode->nonTermInstance.id == subtreeStartNode->nonTermInstance.id)
                {
                    auto* copySubtreeStartNode = new TreeNode(*subtreeStartNode);

                    // Find parent and replace child reference of insertNode to subtreeStartNode.
                    bool inserted = false;
                    for (TreeNode*& child : insertNode->parent->children)
                    {
                        if (child == insertNode)
                        {
                            child = copySubtreeStartNode;
                            child->parent = insertNode->parent;
                            inserted = true;
                            break;
                        }
                    }

                    // Avoid memory leak if no insert node found.
                    if (!inserted)
                    {
                        delete copySubtreeStartNode;
                        throw std::runtime_error("Insert node was not found.");
                    }
                    else
                        this->ClearEvaluation();
                }
                else
                    throw std::runtime_error("Cannot insert subtree of different type of insertNode.");
            }
            else
            {
                if (insertNode->type != NodeType::NonTerminal)
                    throw std::runtime_error("Cannot insert subtree in Terminal " + insertNode->termInstance.label);
                if (subtreeStartNode->type != NodeType::NonTerminal)
                    throw std::runtime_error("Cannot insert subtree of type Terminal " + subtreeStartNode->termInstance.label);
            }
        }

        /// Insert a copy of the subtree into the position at insertNode.
        /// \param insertNode Node where the subtree will be inserted.
        /// \param subtree Reference to the subtree to copy and insert.
        void InsertSubtree(TreeNode* insertNode, const SyntaxTree& subtree)
        {
            InsertSubtree(insertNode, subtree.Root());
        }

        //************************
        //*      Conversions     *
        //***********************/

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

        /// Get string representation.
        [[nodiscard]]
        std::string ToString() const
        {
            std::stringstream ss;
            ss << "\n";
            PrintTree(ss);
            return std::string("SyntaxTree(") + ss.str() + ")";
        }

        /// Export the tree into a graph.
        /// \return A Graph object.
        [[nodiscard]]
        Graph ToGraph() const
        {
            std::vector<Node> nodes;
            std::vector<std::pair<int, int>> edges;

            // Get nodes.
            Traversal treeNodes = GetPreOrderTreeTraversal();
            nodes.reserve(treeNodes.size());
            for (TreeNode* node : treeNodes)
            {
                nodes.push_back(*node);
            }

            // Get edges.
            for (int i = 0; i < static_cast<int>(treeNodes.size()); i++)
            {
                for (int j = 0; j < static_cast<int>(treeNodes.size()); j++)
                {
                    if (treeNodes[j]->parent == treeNodes[i])
                        edges.emplace_back(i, j);
                }
            }

            return {nodes, edges};
        }

        /// Build syntax tree from a graph.
        /// \param graph The graph that contains the structure of the tree to be built.
        /// \return The root node of the rebuilt tree.
        static TreeNode* FromGraph(const Graph& graph)
        {
            std::vector<TreeNode*> treeNodes = graph.GetTreeNodes();
            return treeNodes.front();
        }

        //***************************
        //*     Tree traversals     *
        //**************************/

        /// Performs a shallow copy of each node.
        /// \param other The traversal of the tree to copy.
        /// \return The traversal of the copied tree.
        static Traversal CopyTreeTraversal(const Traversal& other)
        {
            Traversal copyNodes;
            copyNodes.reserve(other.size());
            for (auto node : other)
                copyNodes.push_back(TreeNode::ShallowCopy(node));
            return copyNodes;
        }

        static void DeleteTreeTraversal(const Traversal& traversal)
        {
            for (auto node : traversal)
                delete node;
        }

        /// Find the index of a traversal subsequence inside another traversal.
        /// \param traversal The traversal to perform the search on.
        /// \param subsequence The traversal subsequence to find.
        /// \return The distance to the subsequence if found, else the last index of traversal.
        static unsigned FindIndexOfTraversalSubsequence(const Traversal& traversal,
                                                        const Traversal& subsequence)
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
        static Traversal ReplaceTraversalSubsequence(const Traversal& traversal, const Traversal& replaceFrom,
                                                     const Traversal& replaceTo)
        {
            Traversal copyNodes = SyntaxTree::CopyTreeTraversal(traversal);
            Traversal replacementNodes = SyntaxTree::CopyTreeTraversal(replaceTo);

            const unsigned replaceFromLength = replaceFrom.size();
            const unsigned replaceToLength = replaceTo.size();
            const unsigned replaceIndex = SyntaxTree::FindIndexOfTraversalSubsequence(copyNodes, replaceFrom);

            if (replaceIndex == copyNodes.size()) // No subsequence to replace. Return source traversal.
            {
                DeleteTreeTraversal(copyNodes);
                return traversal;
            }

            // Copy capture ID to traversal.
            for (unsigned i = replaceIndex; i < replaceIndex + replaceToLength; i++)
            {
                if (replaceFrom[i - replaceIndex]->HasCaptureID())
                    traversal[i]->captureID = replaceFrom[i - replaceIndex]->captureID;
            }

            // Delete and replace.
            copyNodes.erase(copyNodes.begin() + replaceIndex, copyNodes.begin() + replaceIndex + replaceFromLength);
            copyNodes.insert(copyNodes.begin() + replaceIndex, replacementNodes.begin(), replacementNodes.end());

            // Transfer values of nodes with same ID and capture ID..
            for (unsigned i = replaceIndex; i < replaceIndex + replaceToLength; i++)
            {
                for (unsigned j = replaceIndex; j < replaceIndex + replaceFromLength; j++)
                {
                    if (copyNodes[i]->SameID(*traversal[j]) && copyNodes[i]->SameCaptureID(*traversal[j]))
                    {
                        copyNodes[i]->termValue = traversal[j]->termValue;
                        break;
                    }
                }
            }

            return copyNodes;
        }

        /// Traverses the tree in a depth first pre-order.
        /// \param node Start node. The default value is the root of the tree.
        /// \return List of references of the traversed nodes.
        [[nodiscard]]
        Traversal GetPreOrderTreeTraversal() const
        {
            return GetPreOrderTreeTraversal(_root);
        }

        /// Traverses the tree in a depth first pre-order.
        /// \param node Start node. The default value is the root of the tree.
        /// \return List of references of the traversed nodes.
        static Traversal GetPreOrderTreeTraversal(TreeNode* node)
        {
            return GetPreOrderTreeTraversal(node, true);
        }

        /// Traverses the tree in a depth first pre-order.
        /// \return List of references of the traversed nodes.
        [[nodiscard]]
        Traversal GetPostOrderTreeTraversal() const
        {
            return GetPostOrderTreeTraversal(_root);
        }

        /// Traverses the tree in a depth first post-order.
        /// \param node Start node. The default value is the root of the tree.
        /// \return List of references of the traversed nodes.
        static Traversal GetPostOrderTreeTraversal(TreeNode* node)
        {
            /*
              Algorithm Postorder(tree)
              1. Traverse the left subtree, i.e., call Postorder(left-subtree)
              2. Traverse the right subtree, i.e., call Postorder(right-subtree)
              3. Visit the root.
            */
            // TODO: Replace recursive implementation with iterative.
            Traversal output;

            if (node->children.empty())
            {
                output.push_back(node);
                return output;
            }

            for (TreeNode* n : node->children)
            {
                const Traversal childrenTreeNodes = GetPostOrderTreeTraversal(n);
                output.insert(output.end(), childrenTreeNodes.begin(), childrenTreeNodes.end());
            }
            output.push_back(node);

            return output;
        }

        /// Traverses the tree in a breadth first order.
        /// \return List of references of the traversed nodes.
        [[nodiscard]]
        Traversal GetBreadthFirstTreeTraversal() const
        {
            return GetBreadthFirstTreeTraversal(_root);
        }

        /// Traverses the tree in a breadth first order.
        /// \param node The root node.
        /// \return List of references of the traversed nodes.
        static Traversal GetBreadthFirstTreeTraversal(TreeNode* node)
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

            Traversal output;
            std::queue<TreeNode*> q;
            q.push(node);

            while (!q.empty())
            {
                TreeNode* current = q.front();
                output.push_back(current);
                q.pop();

                for (TreeNode* n : current->children)
                    q.push(n);
            }

            return output;

        }

        /// Get the string representation of a traversal.
        /// \param traversal The traversal.
        /// \return The string representation of the traversal.
        static std::string TraversalToString(const Traversal& traversal)
        {
            std::string output;
            for (int i = 0; i < traversal.size(); i++)
            {
                output += traversal[i]->GetLabel();
                if (i < traversal.size() - 1)
                    output += ", ";
            }
            return "[" + output + "]";
        }

        /// Check if the tree with root nodeB exists as a base tree of the tree given by nodeA.
        /// \param nodeA The root node of treeA.
        /// \param nodeB The root node of treeB.
        /// \return True if the base trees are equal.
        static bool HasSameBaseTree(TreeNode* nodeA, TreeNode* nodeB)
        {
            Traversal traversalA = GetBreadthFirstTreeTraversal(nodeA);
            Traversal traversalB = GetBreadthFirstTreeTraversal(nodeB);

            for (int i = 0; i < traversalB.size(); i++)
            {
                if (i < traversalA.size())
                {
                    if (!traversalA[i]->SameID(*traversalB[i]))
                        return false;
                }
                else
                    return false;
            }

            return true;
        }

        [[nodiscard]]
        Traversal GetTermsOfType(NodeType type) const
        {
            Traversal traversal = GetPostOrderTreeTraversal();
            std::vector<size_t> terminalIndexes = find_indexes_if(traversal, [type](TreeNode* node) { return node->type == type; });

            return extract_elements_at_indexes(traversal, terminalIndexes);
        }

        /// Find the index of the first non-synthesized non-terminal.
        /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
        /// \return The index if the first non-synthesized non-terminal.
        [[nodiscard]]
        static unsigned NextToBuild(Traversal& treeTraversal)
        {
            for (unsigned i = 0; i < treeTraversal.size(); i++)
                if (treeTraversal[i]->type == NodeType::NonTerminal && !treeTraversal[i]->HasChildren())
                    return i;

            return treeTraversal.size();
        }

        /// Builds the first non-terminal node with no children.
        /// \param treeTraversal The tree traversal.
        static void BuildFirst(Traversal& treeTraversal)
        {
            unsigned nextIndex = NextToBuild(treeTraversal);
            if (nextIndex == treeTraversal.size())
                return;

            TreeNode* nodeToBuild = treeTraversal[nextIndex];
            ProductionRule rule = nodeToBuild->generatorPR;
            std::vector<size_t> toErase;

            for (const ProductionElement& se : rule.to)
            {
                const std::optional<size_t> pos = (se.type == ProductionElementType::NonTerminal) ?
                        FindIndexOfNonTerm(treeTraversal, se.nonterm.id, toErase, nextIndex,rule.NumberOfProductionElements()) :
                        FindIndexOfTerm(treeTraversal, se.term.id, toErase, nextIndex, rule.NumberOfProductionElements());

                if (pos.has_value())
                {
                    nodeToBuild->AddChildNode(treeTraversal[pos.value()]);
                    toErase.push_back(pos.value());
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

            delete_elements_at_indexes(treeTraversal, toErase);
        }

        /// Builds a SyntaxTree from a depth first post order traversal.
        /// \param treeTraversal The traversal.
        /// \return The tree built from the traversal.
        static void BuildFromTraversal(SyntaxTree& targetTree, Traversal& treeTraversal)
        {
            while (treeTraversal.size() > 1)
                BuildFirst(treeTraversal);

            targetTree.SetRoot(treeTraversal.back());
        }

        //***************************
        //*     Tree evaluation     *
        //**************************/

        /// Find the index of the first non-synthesized non-terminal.
        /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
        /// \return The index if the first non-synthesized non-terminal.
        [[nodiscard]]
        static unsigned NextToSynthesize(Traversal& treeTraversal)
        {
            for (unsigned i = 0; i < treeTraversal.size(); i++)
                if (treeTraversal[i]->type == NodeType::NonTerminal && !treeTraversal[i]->IsSynthesized())
                    return i;

            return treeTraversal.size();
        }

        /// Synthesize the first non-synthesized node and deletes the consumed nodes.
        /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
        static void SynthesizeFirst(Traversal& treeTraversal)
        {
            // Get the rule of the element to be evaluated.
            unsigned nextIndex = NextToSynthesize(treeTraversal);
            if (nextIndex == treeTraversal.size())
                return;

            ProductionRule rule = treeTraversal[nextIndex]->generatorPR;
            std::string synthesis;
            std::vector<size_t> toErase;

            // Process the elements of treeTraversal that match the semantic rules.
            for (const ProductionElement& se : rule.to)
            {
                if (se.type == ProductionElementType::NonTerminal)
                {
                    const std::optional<size_t> pos = FindIndexOfNonTerm(treeTraversal, se.nonterm.id, toErase, nextIndex,
                                                                         rule.NumberOfProductionElements());
                    if (pos.has_value())
                    {
                        synthesis += treeTraversal[pos.value()]->expressionSynthesis;
                        toErase.push_back(pos.value());
                    }
                    else
                    {
                        std::string errorReport = "Could not find any NonTerm node of type " + se.nonterm.label;
                        errorReport += " during synthesis of node : " + treeTraversal[nextIndex]->ToString();
                        throw std::runtime_error(errorReport);
                    }
                }
                else if (se.type == ProductionElementType::Terminal)
                {
                    const std::optional<size_t> pos = FindIndexOfTerm(treeTraversal, se.term.id, toErase, nextIndex,
                                                                      rule.NumberOfProductionElements());
                    if (pos.has_value())
                    {
                        synthesis += treeTraversal[pos.value()]->termValue;
                        toErase.push_back(pos.value());
                    }
                    else
                        throw std::runtime_error("Could not find any Term node of type " + se.term.label + " during expressionSynthesis");
                }
            }

            // Consume and delete
            treeTraversal[nextIndex]->expressionSynthesis = synthesis;
            delete_elements_at_indexes(treeTraversal, toErase);
        }

        /// Synthesizes the tree into an expression using the production rules of the grammar.
        /// \return The synthesized expression as a std::string.
        [[nodiscard]]
        std::string SynthesizeExpression() const
        {
            Traversal treeTraversal = this->GetPostOrderTreeTraversal();
            for (auto node : treeTraversal) node->ClearSynthesis();

            while (treeTraversal.size() > 1)
                SynthesizeFirst(treeTraversal);

            return treeTraversal.back()->expressionSynthesis;
        }

        /// Find the index of the first non-evaluated non-terminal.
        /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
        /// \return The index if the first non-evaluated non-terminal.
        [[nodiscard]]
        static unsigned NextToEvaluate(Traversal& treeTraversal)
        {
            for (unsigned i = 0; i < treeTraversal.size(); i++)
                if (treeTraversal[i]->type == NodeType::NonTerminal && !treeTraversal[i]->IsEvaluated())
                    return i;
            return treeTraversal.size();
        }

        /// ExternalEvaluate the first non-evaluate node and deletes the consumed nodes.
        /// \param treeTraversal List of nodes traversed in DepthFirst PostOrder.
        /// \param evaluationContext reference to the evaluation context.
        static void EvaluateFirst(Traversal& treeTraversal, EvaluationContext& evaluationContext)
        {
            // Get the rule of the element to be evaluated.
            unsigned nextIndex = NextToEvaluate(treeTraversal);
            if (nextIndex == treeTraversal.size())
                return;

            ProductionRule rule = treeTraversal[nextIndex]->generatorPR;
            std::vector<size_t> toErase;

            evaluationContext.Prepare();

            // Push to the context the elements of treeTraversal that match the production rules.
            for (const ProductionElement& se : rule.to)
            {
                if (se.type == ProductionElementType::NonTerminal)
                {
                    const std::optional<size_t> pos = FindIndexOfNonTerm(treeTraversal, se.nonterm.id, toErase, nextIndex,
                                                                         rule.NumberOfProductionElements());
                    if (pos.has_value())
                    {
                        evaluationContext.PushSemanticValue(treeTraversal[pos.value()]->expressionEvaluation);
                        toErase.push_back(pos.value());
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
                    const std::optional<size_t> pos = FindIndexOfTerm(treeTraversal, se.term.id, toErase, nextIndex,
                                                                      rule.NumberOfProductionElements());
                    if (pos.has_value() && pos != -1)
                    {
                        evaluationContext.PushSemanticValue(treeTraversal[pos.value()]->termValue);
                        toErase.push_back(pos.value());
                    }
                    else
                        throw std::runtime_error("Could not find any Term node of type " + se.term.label + " during expression evaluation");
                }
            }

            // Execute semantic action and delete.
            if (rule.semanticAction != nullptr)
            {
                rule.semanticAction(evaluationContext);
                treeTraversal[nextIndex]->expressionEvaluation = evaluationContext.result();
            }
            else
                throw std::runtime_error("There is no semantic action for rule " + rule.ToString());

            delete_elements_at_indexes(treeTraversal, toErase);
        }

        /// Evaluates the tree using the semantic actions of the grammar.
        /// \param ctx reference to the evaluation context.
        /// \return true if expression was evaluated correctly, false if not.
        void Evaluate(EvaluationContext& ctx) const
        {
            Traversal treeTraversal = this->GetPostOrderTreeTraversal();
            for (auto node : treeTraversal) node->ClearEvaluation();

            while (treeTraversal.size() > 1)
                EvaluateFirst(treeTraversal, ctx);
        }

        /// Evaluates the tree using an external evaluator.
        /// \tparam ReturnType The return type of the evaluator.
        /// \param evaluator The function pointer to the evaluator.
        /// \param result Reference to the variable where the result will be stored.
        /// \return true if expression was evaluated correctly, false if not.
        template<typename ReturnType = std::string> ReturnType ExternalEvaluate(std::function<ReturnType(std::string)> evaluator) const
        {
            std::string synthesis = SynthesizeExpression();
            return evaluator(synthesis);
        }
    };
}