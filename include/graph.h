#pragma once
#include <utility>
#include <map>
#include "tree_node.h"

/// A graph is an intermediate data structure for a syntax tree used for plotting and serialization/deserialization.
class Graph
{
private:
    std::vector<Node> _nodes;
    std::vector<std::pair<int, int>> _edges;

    /// Utility to convert an edge to a string.
    /// \param edge The edge.
    /// \return The edge as string.
    [[nodiscard]]
    static std::string EdgeToString(std::pair<int, int> edge)
    {
        return "{" + std::to_string(edge.first) + ", " + std::to_string(edge.second) + "}";
    }

public:
    /// Empty constructor.
    Graph() = default;

    /// Graph constructor.
    /// \param nodes The list of nodes.
    /// \param edges The list of edges.
    Graph(const std::vector<Node>& nodes, const std::vector<std::pair<int, int>>& edges)
    {
        _nodes = nodes;
        _edges = edges;
    }

    /// Getter for the nodes.
    std::vector<Node> GetNodes()
    {
        return _nodes;
    }

    /// Get the indexes of the nodes.
    /// \return A list of indexes.
    std::vector<int> GetNodeIndexes()
    {
        std::vector<int> nodeIndexes(_nodes.size());
        std::iota(nodeIndexes.begin(), nodeIndexes.end(), 1);
        return nodeIndexes;
    }

    /// Getter for the edges.
    std::vector<std::pair<int, int>> GetEdges()
    {
        return _edges;
    }

    /// Get the labels as a map.
    /// \return The label map.
    std::map<int, std::string> GetLabels()
    {
        std::map<int, std::string> labels;
        for (int i = 0; i < _nodes.size(); i++)
            labels[i] = _nodes[i].GetLabel();

        return labels;
    }

    /// Rebuild the TreeNode pointer chain from this graph.
    /// \return A list of pointers to the instantiated tree nodes.
    [[nodiscard]]
    std::vector<TreeNode*> GetTreeNodes() const
    {
        std::vector<TreeNode*> treeNodes;
        treeNodes.reserve(_nodes.size());

        for (const auto & _node : _nodes)
            treeNodes.push_back(new TreeNode(_node));

        for (int i = 0; i < static_cast<int>(treeNodes.size()); i++)
        {
            for (auto & _edge : _edges)
            {
                if (i == _edge.first)
                {
                    int childIndex = _edge.second;
                    treeNodes[i]->AddChildNode(treeNodes[childIndex]);
                }
            }
        }

        return treeNodes;
    }

    /// Get string representation.
    [[nodiscard]]
    std::string ToString() const
    {
        // Nodes.
        std::vector<std::string> nodesAsStr;
        for (const Node &node: _nodes)
            nodesAsStr.push_back(node.ToString());

        std::string output = "nodes=" + vector_to_string(nodesAsStr);

        // Edges.
        std::vector<std::string> edgesAsStr;
        for (const std::pair<int, int>& edge : _edges)
            edgesAsStr.push_back(EdgeToString(edge));

        output += ", edges=" + vector_to_string(edgesAsStr);

        return output;
    }

    /// Serialization hook.
    template<class Archive> void serialize(Archive& ar)
    {
        ar(_nodes, _edges);
    }
};