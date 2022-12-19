#pragma once
#include <utility>
#include "tree_node.h"

/// A graph is an intermediate data structure for a syntax tree used for plotting and serialization/deserialization.
class Graph
{
private:
    std::vector<Node> _nodes;
    std::vector<std::pair<int, int>> _edges;

public:
    Graph(const std::vector<Node>& nodes, const std::vector<std::pair<int, int>>& edges)
    {
        _nodes = nodes;
        _edges = edges;
    }

    std::vector<Node> GetNodes()
    {
        return _nodes;
    }

    std::vector<std::pair<int, int>> GetEdges()
    {
        return _edges;
    }

    [[nodiscard]]
    std::vector<TreeNode*> GetTreeNodes() const
    {
        std::vector<TreeNode*> treeNodes;
        treeNodes.reserve(_nodes.size());

        for (const auto & _node : _nodes)
            treeNodes.push_back(new TreeNode(_node));

        for (int i = 0; i < treeNodes.size(); i++)
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
};